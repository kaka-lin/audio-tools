#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tinyalsa/asoundlib.h>

#include "src/audio.h"
#include "src/args.h"

int main(int argc, char **argv)
{
    struct audio audio;
    void *frames;
    int ret;

    memset(&audio, 0, sizeof(audio));

    audio.card = 0;
    audio.device = 0;
    audio.channels = 2;
    audio.sample_rate = 44100;
    audio.pcm_format = PCM_FORMAT_S16_LE;
    audio.bits = pcm_format_to_bits(audio.pcm_format);
    audio.period_size = 1024;
    audio.period_count = 2;
    audio.flags = PCM_OUT;

    ret = parse_args(&audio, argc, argv);
    if (ret) {
        return EXIT_FAILURE;
    }

    if (audio.fd == 0) {
        audio.name = "test.wav";
        audio.fd = fopen(audio.name, "rb");
    }
    
    if (audio.fd == NULL) {
        perror("failed to open file");
        return EXIT_FAILURE;
    }

    /* Parse WAV file */
    ret = read_file(&audio, &frames);
    if (ret != 0) {
        return EXIT_FAILURE;
    }

    /* Start Playback */
    ret = start_play(&audio, (const void **)&frames);
    if (ret != 0) {
        return EXIT_FAILURE;
    }

    free(frames);
    return EXIT_SUCCESS;
}
