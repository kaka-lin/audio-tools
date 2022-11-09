#include <string.h>
#include <stdlib.h>

#include "wav.h"

int wav_init(struct wav_info *wav_info)
{
    memset(wav_info, 0, sizeof(*wav_info));

    wav_info->data = NULL;

    return 0;
}

int wav_read(struct wav_info *wav_info, char *name)
{
    int ret;

    wav_info->fd = fopen(name, "rb"); 
    if (wav_info->fd == NULL)
        return -1;

    printf("Parsing %s ...\n", name);
    parse_wav(wav_info);

    wav_info->buffer = malloc(wav_info->bytes);
    if (wav_info->buffer == NULL) {
        fprintf(stderr, "Memory allocation faile.\n");
        return -1;
    }

    if (fseek(wav_info->fd, 44, SEEK_SET) < 0) 
            return -1;
    ret = fread(wav_info->buffer, 1, wav_info->bytes, wav_info->fd);
    if (ret != wav_info->bytes) {
        fprintf(stderr, "Read error, %d\n", ret);
        return -1;
    }

    data_array(wav_info);

    fclose(wav_info->fd);
    return 0;
}

void parse_wav(struct wav_info *wav_info)
{
    struct wav_header header;
    memset(&header, 0, sizeof(header));

    fread(&header.riff_id, sizeof(header.riff_id), 1, wav_info->fd);
    fread(&header.riff_size, sizeof(header.riff_size), 1, wav_info->fd);
    fread(&header.riff_format, sizeof(header.riff_format), 1, wav_info->fd);
    fread(&header.fmt_id, sizeof(header.fmt_id), 1, wav_info->fd);
    fread(&header.fmt_size, sizeof(header.fmt_size), 1, wav_info->fd);
    fread(&header.audio_format, sizeof(header.audio_format), 1, wav_info->fd);
    fread(&header.num_channels, sizeof(header.num_channels), 1, wav_info->fd);
    fread(&header.sample_rate, sizeof(header.sample_rate), 1, wav_info->fd);
    fread(&header.byte_rate, sizeof(header.byte_rate), 1, wav_info->fd);
    fread(&header.block_align, sizeof(header.block_align), 1, wav_info->fd);
    fread(&header.bits_per_sample, sizeof(header.bits_per_sample), 1, wav_info->fd);
    fread(&header.data_id, sizeof(header.data_id), 1, wav_info->fd);
    fread(&header.data_size, sizeof(header.data_size), 1, wav_info->fd);

    wav_info->channels = header.num_channels;
    wav_info->rate = header.sample_rate;
    wav_info->bits = header.fmt_size;
    wav_info->bytes = header.data_size;
    wav_info->frames = wav_info->bytes / (wav_info->channels * (wav_info->bits / 8));
    wav_info->header = &header;
}

void data_array(struct wav_info *wav_info)
{
    int len = wav_info->bytes / (wav_info->bits / 8);
    int max = 0;
    
    wav_info->data = calloc(len, sizeof(int));
    if (wav_info->data == NULL)
        fprintf(stderr, "Memory allocation faile.\n");

    switch (wav_info->bits) {
        case 8:
            for (int i = 0; i < len; i++) {
                wav_info->data[i] = ((int8_t *)wav_info->buffer)[i];
                max = max > wav_info->data[i] ? max : wav_info->data[i];
            }
            break;
        case 16:
            for (int i = 0; i < len; i++) {
                wav_info->data[i] = ((int16_t *)wav_info->buffer)[i];
                max = max > wav_info->data[i] ? max : wav_info->data[i];
            }
            break;
        default:
            fprintf(stderr, "Resolution error!\n");
            break;
    }

    wav_info->max = max;
}

int max(struct wav_info *wav_info)
{   
    return wav_info->max;
}

void normalization(struct wav_info *wav_info)
{
    int len = wav_info->bytes / (wav_info->bits / 8);

    wav_info->norm_data = calloc(len, sizeof(double));
    if (wav_info->norm_data == NULL) {
        fprintf(stderr, "Memory allocation faile.\n");
    }

    for (int i = 0; i < len; i++) {
        wav_info->norm_data[i] = wav_info->data[i] / (float)wav_info->max;        
    }
}

