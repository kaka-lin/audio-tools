#ifndef UTILS_H
#define UTILS_H

#include <alsa/asoundlib.h>

void query_alsa_version();
void query_pcm_stream_types();
void query_pcm_access_types();
void query_pcm_formats();
void query_pcm_subformats();
void query_pcm_states();

void show_pcm_name(snd_pcm_t *pcm);
void show_pcm_state(snd_pcm_t *pcm);
void show_pcm_access_type(snd_pcm_hw_params_t *params, 
    unsigned int val);
void show_pcm_format(snd_pcm_hw_params_t *params, 
    unsigned int val);
void show_pcm_subformat(snd_pcm_hw_params_t *params, 
    unsigned int val);
void show_pcm_channels(snd_pcm_hw_params_t *params, 
    unsigned int val);
void show_pcm_rate(snd_pcm_hw_params_t *params, 
    unsigned int val, int dir);
void show_pcm_period_time(snd_pcm_hw_params_t *params, 
    unsigned int val, int dir);
void show_pcm_period_size(snd_pcm_hw_params_t *params, 
    snd_pcm_uframes_t frames, int dir);
void show_pcm_buffer_time(snd_pcm_hw_params_t *params, 
    unsigned int val, int dir);
void show_pcm_buffer_size(snd_pcm_hw_params_t *params, 
    unsigned int val);
void show_pcm_period(snd_pcm_hw_params_t *params, 
    unsigned int val, int dir);
void show_pcm_rate_numden(snd_pcm_hw_params_t *params, 
    unsigned int num, unsigned int den);
void show_pcm_sbits(snd_pcm_hw_params_t *params);
void show_pcm_tick_time(snd_pcm_hw_params_t *params, 
    unsigned int val, int dir);

void show_pcm_is_batch(snd_pcm_hw_params_t *params);
void show_pcm_is_block_transfer(snd_pcm_hw_params_t *params);
void show_pcm_is_double(snd_pcm_hw_params_t *params);
void show_pcm_is_half_duplex(snd_pcm_hw_params_t *params);
void show_pcm_is_joint_duplex(snd_pcm_hw_params_t *params);

void show_pcm_can_overrange(snd_pcm_hw_params_t *params);
void show_pcm_can_mmap_sample_resolution(snd_pcm_hw_params_t *params);
void show_pcm_can_pause(snd_pcm_hw_params_t *params);
void show_pcm_can_resume(snd_pcm_hw_params_t *params);
void show_pcm_can_sync_start(snd_pcm_hw_params_t *params);

#endif // UTILS_H
