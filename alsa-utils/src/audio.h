#ifndef AUDIO_H
#define AUDIO_H

#include "utils.h"

void audio_show_info(snd_pcm_t *pcm, snd_pcm_hw_params_t *params,
    snd_pcm_uframes_t frames, unsigned int val, unsigned int den, int dir);

int audio_set_hw_params(snd_pcm_t *pcm, snd_pcm_hw_params_t *params);

#endif // AUDIO_H
