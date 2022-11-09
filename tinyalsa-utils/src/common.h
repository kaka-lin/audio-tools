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
    // The 'RIFF' chunk descriptor
    uint32_t riff_id;
    uint32_t riff_size;
    uint32_t riff_format;

    // The 'fmt' sub-chunk
    uint32_t fmt_id;
    uint32_t fmt_size;

    // The 'audio' chunk
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;

    //The 'data' sub-chunk
    uint32_t data_id;
    uint32_t data_size;
};

struct audio {
    FILE *fd;
    char *name;

    unsigned int card;
    unsigned int device;
    unsigned int channels;
    unsigned int sample_rate;
    enum pcm_format pcm_format;
    unsigned int bits;
    /* buffer = period_count * period_size */
    unsigned int period_size;  // 每次中斷, DMA傳輸的大小
    unsigned int period_count; // 有幾個preiod_size -> 會組成一個環形buffer
    unsigned int record_time; //second

    size_t bytes;
    size_t frames;

    int flags;
    bool capturing;
    bool verbose_mode;

    /* pthread */
    pthread_mutex_t lock;
	pthread_condattr_t attr;
	pthread_cond_t cond;
};

#endif /* COMMON_H */
