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
    header.audio_format = 1; // pcm: 1
    header.num_channels = audio->channels;
    header.sample_rate = audio->sample_rate;
    header.bits_per_sample = audio->bits;
    header.byte_rate = audio->sample_rate * audio->channels * (header.bits_per_sample / 8);
    header.block_align = audio->channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;
    header.riff_size = header.byte_rate * audio->record_time + 44 - 8;
    header.data_size = header.byte_rate * audio->record_time;

    fwrite(&header, 1, sizeof(header), audio->fd);
}

int start_capture(struct audio *audio, void**frames)
{
    int frame_counts = 0;
    int byte_counts = 0;
    unsigned int frame_byte;
    unsigned int buffer_size, buffer_byte;

    struct pcm_config config = {
        .channels = audio->channels,
        .rate = audio->sample_rate,
        .format = audio->pcm_format,
        .period_size = audio->period_size,
        .period_count = audio->period_count,
        .start_threshold = audio->period_size,
        .stop_threshold = audio->period_size * audio->period_count,
        .silence_threshold = audio->period_size * audio->period_count,
    };

    struct pcm *pcm = pcm_open(
        audio->card, audio->device, PCM_IN, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        printf("Unable to open PCM device %u (%s)\n",
                audio->device, pcm_get_error(pcm));
        return -1;
    }
    
    frame_byte = pcm_frames_to_bytes(pcm, 1); // 1 frame = Sample(16bit,2byte) * channel(2) = 4 byte
    
    // dfualt: buffer_size = 1024 * 2
    //      => buffer_byte = 1024 * 2 * 4(4: Sample(16bit,2byte) * channel(2) = 4 byte)
    buffer_size = pcm_get_buffer_size(pcm);
    buffer_byte = pcm_frames_to_bytes(pcm, buffer_size);

    *frames = malloc(buffer_byte);
    if (*frames == NULL) {
        fprintf(stderr, "failed to allocate frames\n");
        pcm_close(pcm);
        return EXIT_FAILURE;
    }

    printf("Recording WAVE '%s': %d bits, Rate %d Hz, %d ch (%s)\n", 
        audio->name, audio->bits, audio->sample_rate,
        audio->channels, audio->channels >= 2 ? "Stereo" : "Mono");
    
    if (audio->record_time != 0) {
        unsigned int total_frames = audio->sample_rate * audio->record_time;
        audio->frames = total_frames;
        audio->bytes = audio->frames * audio->channels * (audio->bits / 8);

        while (total_frames) {
            if (buffer_size >= total_frames)
                buffer_size = total_frames;
            
            frame_counts = pcm_readi(pcm, *frames, buffer_size);
            byte_counts = pcm_frames_to_bytes(pcm, frame_counts);
            total_frames -= frame_counts;
            
            if (audio->verbose_mode)
                printf("Capture: %u frames, %u bytes, res: %u frames\n", 
                    frame_counts, byte_counts, total_frames);

            fwrite(*frames, frame_byte, frame_counts, audio->fd);
        }
    } else {
        while(audio->capturing) {
            frame_counts = pcm_readi(pcm, *frames, buffer_size);
            byte_counts = pcm_frames_to_bytes(pcm, frame_counts);
            audio->frames += frame_counts;
            audio->bytes  += byte_counts;

            if (audio->verbose_mode)
                printf("Capture: %u frames, %u bytes, total: %lu frames\n", 
                    frame_counts, byte_counts, audio->frames);
            
            fwrite(*frames, frame_byte, frame_counts, audio->fd);
        }

        /* Update WAV header: data size */
        unsigned int update[1];
        fseek(audio->fd, 4, SEEK_SET);
        update[0] = audio->bytes + 44 - 8;
        fwrite(update, 4, 1, audio->fd);
        fseek(audio->fd, 40, SEEK_SET);
        update[0] = audio->bytes;
        fwrite(update, 4, 1, audio->fd);
    }

    printf("Finished!\n");

    fclose(audio->fd);
    pcm_close(pcm);
    return 0;
}

int capture_streaming(struct audio *audio, void **frames)
{
    unsigned frame_count, byte_count;
    unsigned int total_frames = 0;
    unsigned int total_bytes = 0;
    unsigned int buffer_size, buffer_byte;

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
    
    buffer_size = pcm_get_buffer_size(pcm);
    buffer_byte = pcm_frames_to_bytes(pcm, buffer_size);
    audio->frames = buffer_size;
    audio->bytes = buffer_byte;

    *frames = malloc(buffer_byte);
    if (*frames == NULL) {
        fprintf(stderr, "failed to allocate frames\n");
        pcm_close(pcm);
        return EXIT_FAILURE;
    }

    printf("Recording WAVE: %d bits, Rate %d Hz, %d ch (%s)\n", 
        audio->bits, audio->sample_rate,
        audio->channels, audio->channels >= 2 ? "Stereo" : "Mono");
    
    while(audio->capturing) {
        frame_count = pcm_readi(pcm, *frames, buffer_size);
        byte_count = pcm_frames_to_bytes(pcm, frame_count);
        total_frames += frame_count;
        total_bytes += byte_count;  
    }

    printf("Finished!\n");

    audio->frames = total_frames;
    audio->bytes = total_bytes;

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
        fclose(audio->fd);
        return -1;
    }

    if (fseek(audio->fd, 44, SEEK_SET) < 0) 
        return -1;
    fread(*frames, 1, audio->bytes, audio->fd);

    fclose(audio->fd);
    return 0;
}

void parse_wav(struct audio *audio)
{
    struct wav_header header;

    memset(&header, 0, sizeof(header));

    // fseek(audio->fd, 0, SEEK_END)s
    fread(&header.riff_id, sizeof(header.riff_id), 1, audio->fd);
    fread(&header.riff_size, sizeof(header.riff_size), 1, audio->fd);
    fread(&header.riff_format, sizeof(header.riff_format), 1, audio->fd);
    fread(&header.fmt_id, sizeof(header.fmt_id), 1, audio->fd);
    fread(&header.fmt_size, sizeof(header.fmt_size), 1, audio->fd);
    fread(&header.audio_format, sizeof(header.audio_format), 1, audio->fd);
    fread(&header.num_channels, sizeof(header.num_channels), 1, audio->fd);
    fread(&header.sample_rate, sizeof(header.sample_rate), 1, audio->fd);
    fread(&header.byte_rate, sizeof(header.byte_rate), 1, audio->fd);
    fread(&header.block_align, sizeof(header.block_align), 1, audio->fd);
    fread(&header.bits_per_sample, sizeof(header.bits_per_sample), 1, audio->fd);
    fread(&header.data_id, sizeof(header.data_id), 1, audio->fd);
    fread(&header.data_size, sizeof(header.data_size), 1, audio->fd);

    audio->channels = header.num_channels;
    audio->sample_rate = header.sample_rate;
    audio->bits = header.fmt_size;
    audio->bytes = header.data_size;
}

int start_play(struct audio *audio, const void **frames)
{   
    struct pcm_config config = {
        .channels = audio->channels,
        .rate = audio->sample_rate,
        .period_size = audio->period_size,
        .period_count = audio->period_count,
        .format = audio->pcm_format,
        .start_threshold = audio->period_size,
        .stop_threshold = audio->period_size * audio->period_count,
        .silence_threshold = audio->period_size * audio->period_count,
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
 
    printf("Playing WAVE '%s': %d bits, Rate %d Hz, %d ch (%s)\n", 
        audio->name, audio->bits, audio->sample_rate,
        audio->channels, audio->channels >= 2 ? "Stereo" : "Mono");

    unsigned int frame_count = pcm_bytes_to_frames(pcm, audio->bytes);

    int err = pcm_writei(pcm, *frames, frame_count);
    if (err < 0) {
        printf("error: %d (%s)\n", err, pcm_get_error(pcm));
    }
    printf("Finished!\n");

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

int play_streaming(struct audio *audio, void **frames)
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

    do {
        int err = pcm_writei(pcm, *frames, audio->frames);
        if (err < 0) {
            printf("error: %d (%s)\n", err, pcm_get_error(pcm));
            break;
        }
    } while (audio->capturing);

    printf("Finished!\n");

    pcm_close(pcm);
    return 0;
}
