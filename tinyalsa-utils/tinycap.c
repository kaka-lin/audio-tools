#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <tinyalsa/asoundlib.h>

#include "src/args.h"
#include "src/audio.h"

bool *capturing;

void sigint_handler(int sig)
{
    if (sig == SIGINT){
        *capturing = false;
    }
}

struct fun_para
{
    struct audio *audio;
    void *frames;
};

static void *audio_capture_thread_func(void *args)
{
    struct fun_para *para = (struct fun_para *)args;

    /* Write WAV header */
    write_wav_header(para->audio);

    /* Start Capture */
    start_capture(para->audio, para->frames);
 
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    struct audio audio;
    void *frames;
    int ret;

    // pthread
    pthread_t audio_cap_thread;
    // pthread function parameters
    struct fun_para para;

    memset(&audio, 0, sizeof(audio));

    audio.capturing = true;
    audio.card = 0;
    audio.device = 0;
    audio.channels = 2;
    audio.sample_rate = 44100;
    audio.pcm_format = PCM_FORMAT_S16_LE;
    audio.bits = pcm_format_to_bits(audio.pcm_format);
    audio.period_size = 1024,
    audio.period_count = 2,
    audio.flags = PCM_IN;

    ret = parse_args(&audio, argc, argv);
    if (ret) {
        return EXIT_FAILURE;
    }

    if (audio.fd == 0) {
        audio.name = "test.wav";
        audio.fd = fopen(audio.name, "wb");
    }
    
    if (audio.fd == NULL)
        return EXIT_FAILURE;
    
    /* Signal */
    capturing = &audio.capturing;
    signal(SIGINT, sigint_handler);

    /* pthread init */
    pthread_mutex_init(&audio.lock, 0);
	pthread_condattr_init(&audio.attr);
	pthread_cond_init(&audio.cond, &audio.attr);

    para.audio = &audio;
    para.frames = &frames;
    
    /* Thread: Create*/
    if (pthread_create(&audio_cap_thread, NULL, 
            audio_capture_thread_func, (void *)&para))
        goto err;

    /* Thread: Wait to thread end */
    pthread_join(audio_cap_thread, 0);

    if (audio.verbose_mode)
        printf("Captured %lu frames, %lu bytes\n", 
            audio.frames, audio.bytes);

    pthread_mutex_destroy(&audio.lock);
    free(frames);
    return EXIT_SUCCESS;

err:
    pthread_mutex_destroy(&audio.lock);
	pthread_cond_destroy(&audio.cond);
	pthread_condattr_destroy(&audio.attr);
    return EXIT_FAILURE;
}
