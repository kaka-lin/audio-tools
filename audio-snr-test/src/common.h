#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include <tinyalsa/asoundlib.h>

#define ID_RIFF 0x46464952 // "RIFF"
#define ID_WAVE 0x45564157 // "WAVE"
#define ID_FMT  0x20746d66 // "fmt "
#define ID_DATA 0x61746164 // "data"

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_size;
    uint32_t riff_format;

    uint32_t fmt_id;
    uint32_t fmt_size;

    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    
    uint32_t data_id;
    uint32_t data_size;
};

struct audio {
    FILE *cap_fd;
    FILE *play_fd;
    char *name;

    unsigned int card;
    unsigned int device;
    unsigned int channels;
    unsigned int sample_rate;
    enum pcm_format pcm_format;
    unsigned int bits;
    unsigned int period_size; 
    unsigned int period_count;
    unsigned int record_time; //seconds

    size_t bytes;
    size_t frames;

    int flags;
    bool capturing;
    bool playing;
    bool verbose_mode;

    /* pthread */
    pthread_mutex_t lock;
	pthread_condattr_t attr;
	pthread_cond_t cond;
};

struct wav_info {
    FILE *fd;
    struct wav_header *header;
    
    unsigned int channels;
    unsigned int rate;
    unsigned int bits; // resolution
    size_t bytes;
    size_t frames;

    void *buffer;
    int *data;
    double *norm_data;

    int max;
};

#endif /* COMMON_H */
