#ifndef WAV_H
#define WAV_H

#include "common.h"

int wav_init(struct wav_info *wav_info);

/* Parsing WAVE */
int wav_read(struct wav_info *wav_info, char *name);
void parse_wav(struct wav_info *wav_info);
void data_array(struct wav_info *wav_info);

int max(struct wav_info *wav_info);
void normalization(struct wav_info *wav_info);

#endif /* WAV_H */
