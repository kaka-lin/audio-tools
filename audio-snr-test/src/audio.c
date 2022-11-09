#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "audio.h"

//////////////////////////////////////////////////////////////////////////////////////////////
/* Capture */
void write_wav_header(struct audio *audio)
{
    struct wav_header header;

    memset(&header, 0, sizeof(audio));

    header.riff_id = ID_RIFF;
    header.riff_format = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_size = 16;
    header.audio_format = 1;
    header.num_channels = audio->channels;
    header.sample_rate = audio->sample_rate;
    header.bits_per_sample = audio->bits;
    header.byte_rate = audio->sample_rate * audio->channels * (header.bits_per_sample / 8);
    header.block_align = audio->channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;
    header.riff_size = audio->bytes + 44 - 8;
    header.data_size = audio->bytes;

    fwrite(&header, 1, sizeof(header), audio->cap_fd);
}

int start_capture(struct audio *audio)
{   
    void *buffer;
    unsigned int frame_counts;
    unsigned int frame_byte;

    struct pcm_config config = {
        .channels = audio->channels,
        .rate = audio->sample_rate,
        .format = audio->pcm_format,
        .period_size = audio->period_size,
        .period_count = audio->period_count,
        .start_threshold = 0,
        .stop_threshold = 0,
        .silence_threshold = 0,
    };

    struct pcm *pcm = pcm_open(
        audio->card, audio->device, PCM_IN, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        printf("Unable to open PCM device %u (%s)\n",
                audio->device, pcm_get_error(pcm));
        return -1;
    }
    
    frame_byte = pcm_frames_to_bytes(pcm, 1);

    buffer = malloc(audio->bytes);
    if (buffer == NULL) {
        fprintf(stderr, "failed to allocate frames\n");
        pcm_close(pcm);
        return EXIT_FAILURE;
    }

    printf("Recording: %d bits, Rate %d Hz, %d ch (%s)\n", 
        audio->bits, audio->sample_rate,
        audio->channels, audio->channels >= 2 ? "Stereo" : "Mono");
    
    while(audio->capturing) {        
        if (!audio->playing)
            continue;
            
        frame_counts = pcm_readi(pcm, buffer, audio->frames); 
        fwrite(buffer, frame_byte, frame_counts, audio->cap_fd);
    }
    
    printf("Capture Finished!\n");
    
    free(buffer);
    fclose(audio->cap_fd);
    pcm_close(pcm);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
/* Playback */
int read_file(struct audio *audio, void **frames)
{  
    if (audio->verbose_mode)
        printf("Parse WAV...\n");
    parse_wav(audio);

    *frames = malloc(audio->bytes);
    if (*frames == NULL) {
        fprintf(stderr, "failed to allocate frames\n");
        fclose(audio->play_fd);
        return -1;
    }

    if (fseek(audio->play_fd, 44, SEEK_SET) < 0) 
        return -1;
    fread(*frames, 1, audio->bytes, audio->play_fd);

    fclose(audio->play_fd);
    return 0;
}

void parse_wav(struct audio *audio)
{
    struct wav_header header;

    memset(&header, 0, sizeof(header));

    // fseek(audio->fd, 0, SEEK_END)s
    fread(&header.riff_id, sizeof(header.riff_id), 1, audio->play_fd);
    fread(&header.riff_size, sizeof(header.riff_size), 1, audio->play_fd);
    fread(&header.riff_format, sizeof(header.riff_format), 1, audio->play_fd);
    fread(&header.fmt_id, sizeof(header.fmt_id), 1, audio->play_fd);
    fread(&header.fmt_size, sizeof(header.fmt_size), 1, audio->play_fd);
    fread(&header.audio_format, sizeof(header.audio_format), 1, audio->play_fd);
    fread(&header.num_channels, sizeof(header.num_channels), 1, audio->play_fd);
    fread(&header.sample_rate, sizeof(header.sample_rate), 1, audio->play_fd);
    fread(&header.byte_rate, sizeof(header.byte_rate), 1, audio->play_fd);
    fread(&header.block_align, sizeof(header.block_align), 1, audio->play_fd);
    fread(&header.bits_per_sample, sizeof(header.bits_per_sample), 1, audio->play_fd);
    fread(&header.data_id, sizeof(header.data_id), 1, audio->play_fd);
    fread(&header.data_size, sizeof(header.data_size), 1, audio->play_fd);

    audio->channels = header.num_channels;
    audio->sample_rate = header.sample_rate;
    audio->bits = header.fmt_size;
    audio->bytes = header.data_size;
    audio->frames = audio->bytes / (audio->channels * (audio->bits / 8));
}

int start_play(struct audio *audio, const void *frames)
{   
    struct pcm_config config = {
        .channels = audio->channels,
        .rate = audio->sample_rate,
        .period_size = audio->period_size,
        .period_count = audio->period_count,
        .format = audio->pcm_format,
        .start_threshold = 0,
        .stop_threshold = 0,
        .silence_threshold = 0,
    };

    if (!sample_is_playable(audio)) {
        return -1;
    }

    struct pcm *pcm = pcm_open(
        audio->card, audio->device, PCM_OUT, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        printf("Unable to open PCM device %u (%s)\n",
                audio->device, pcm_get_error(pcm));
        return -1;
    }

    printf("Playing: %d bits, Rate %d Hz, %d ch (%s)\n", 
        audio->bits, audio->sample_rate,
        audio->channels, audio->channels >= 2 ? "Stereo" : "Mono");

    audio->playing = true;
    int err = pcm_writei(pcm, frames, audio->frames);
    if (err < 0) {
        printf("error: %d (%s)\n", err, pcm_get_error(pcm));
    }
    printf("Playing Finished!\n");

    audio->capturing = false;

    pcm_close(pcm);
    return 0;
}

int sample_is_playable(struct audio *audio)
{
    struct pcm_params *params;
    int can_play;
    
    params = pcm_params_get(
        audio->card, audio->device, audio->flags);
    
    if (params == NULL) {
        printf("Unable to open PCM device %u.\n", audio->device);
        return 0;
    }
    
    can_play = check_param(params, PCM_PARAM_RATE, audio->sample_rate, "Sample rate", "Hz");
    can_play &= check_param(params, PCM_PARAM_CHANNELS, audio->channels, "Sample", " channels");
    can_play &= check_param(params, PCM_PARAM_SAMPLE_BITS, audio->bits, "Bitrate", " bits");
    can_play &= check_param(params, PCM_PARAM_PERIOD_SIZE, audio->period_size, "Period size", "Hz");
    can_play &= check_param(params, PCM_PARAM_PERIODS, audio->period_count, "Period count", "Hz");
 
    pcm_params_free(params);
 
    return can_play;
}

int check_param(struct pcm_params *params, unsigned int param, unsigned int value,
                 char *param_name, char *param_unit)
{
    unsigned int min;
    unsigned int max;
    int is_within_bounds = 1;
 
    min = pcm_params_get_min(params, param);
    if (value < min) {
        printf("%s is %u%s, device only supports >= %u%s\n", param_name, value,
                param_unit, min, param_unit);
        is_within_bounds = 0;
    }
 
    max = pcm_params_get_max(params, param);
    if (value > max) {
        printf("%s is %u%s, device only supports <= %u%s\n", param_name, value,
                param_unit, max, param_unit);
        is_within_bounds = 0;
    }
 
    return is_within_bounds;
}
