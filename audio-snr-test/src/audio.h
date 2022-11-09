#ifndef AUDIO_H
#define AUDIO_H

#include "common.h"

/* Capture */
void write_wav_header(struct audio *audio);
int start_capture(struct audio *audio);

/* Parsing WAVE */
int read_file(struct audio *audio, void **frames);
void parse_wav(struct audio *audio);

/* Playback */
int start_play(struct audio *audio, const void *frames);
int sample_is_playable(struct audio *audio);
int check_param(struct pcm_params *params, unsigned int param, unsigned int value,
    char *param_name, char *param_unit);

#endif /* AUDIO_H */
