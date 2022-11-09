#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <tinyalsa/asoundlib.h>

#include "src/args.h"
#include "src/audio.h"

struct fun_para
{
    struct audio *audio;
    void *frames;
};

static void *audio_capture_thread_func(void *args)
{
    struct fun_para *para = (struct fun_para *)args;

    /* Start Capture */
    start_capture(para->audio);

    pthread_exit(NULL);
}

static void *audio_play_thread_func(void *args)
{
    struct fun_para *para = (struct fun_para *)args;

    /* Start Playback */
    start_play(para->audio, para->frames);

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    struct audio audio;
    void *frames;
    int ret;

    // pthread
    pthread_t cap_thread, play_thread;
    // pthread function parameters
    struct fun_para para;

    memset(&audio, 0, sizeof(audio));

    audio.capturing = true;
    audio.card = 0;
    audio.device = 0;
    audio.channels = 2;
    audio.sample_rate = 48000;
    audio.pcm_format = PCM_FORMAT_S16_LE;
    audio.bits = pcm_format_to_bits(audio.pcm_format);
    audio.period_size = 1024,
    audio.period_count = 2;

    ret = parse_args(&audio, argc, argv);
    if (ret) {
        return EXIT_FAILURE;
    }

    if (audio.cap_fd == 0) {
        audio.cap_fd = fopen("audio/signal_with_noise.wav", "wb");
    }

    if (audio.play_fd == 0) {
        audio.play_fd = fopen("audio/signal.wav", "rb");
    }
    
    if (audio.cap_fd == NULL || audio.play_fd == NULL)
        return EXIT_FAILURE;
    
    /* pthread init */
    pthread_mutex_init(&audio.lock, 0);
	pthread_condattr_init(&audio.attr);
	pthread_cond_init(&audio.cond, &audio.attr);

    /* Playback: Parse WAV file */
    ret = read_file(&audio, &frames);
    if (ret != 0) {
        return EXIT_FAILURE;
    }

    /* Capture: Write WAV header */
    write_wav_header(&audio);

    para.audio = &audio;
    para.frames = frames;

    /* Thread: Create*/
    if (pthread_create(&cap_thread, NULL, 
            audio_capture_thread_func, (void *)&para))
        goto err;
    if (pthread_create(&play_thread, NULL, 
            audio_play_thread_func, (void *)&para))
        goto err;
    
    pthread_join(play_thread, 0);
    pthread_join(cap_thread, 0);
    free(frames);
    pthread_mutex_destroy(&audio.lock);
    return EXIT_SUCCESS;

err:
    free(frames);
    pthread_mutex_destroy(&audio.lock);
	pthread_cond_destroy(&audio.cond);
	pthread_condattr_destroy(&audio.attr);
    return EXIT_FAILURE;
}
