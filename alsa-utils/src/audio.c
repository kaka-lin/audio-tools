#include "audio.h"

void audio_show_info(snd_pcm_t *pcm, snd_pcm_hw_params_t *params,
    snd_pcm_uframes_t frames, unsigned int val, unsigned int den, int dir) {
  show_pcm_name(pcm);
  show_pcm_state(pcm);
  show_pcm_access_type(params, val);
  show_pcm_format(params, val);
  show_pcm_subformat(params, val);
  show_pcm_channels(params, val);
  show_pcm_rate(params, val, dir);
  show_pcm_period_time(params, val, dir);
  show_pcm_period_size(params, frames, dir);
  show_pcm_buffer_time(params, val, dir);
  show_pcm_buffer_size(params, val);
  show_pcm_period(params, val, dir);
  show_pcm_rate_numden(params, val, den);
  show_pcm_sbits(params);
  show_pcm_tick_time(params, val, dir);

  show_pcm_is_batch(params);
  show_pcm_is_block_transfer(params);
  show_pcm_is_double(params);
  show_pcm_is_half_duplex(params);
  show_pcm_is_joint_duplex(params);

  show_pcm_can_overrange(params);
  show_pcm_can_mmap_sample_resolution(params);
  show_pcm_can_pause(params);
  show_pcm_can_resume(params);
  show_pcm_can_sync_start(params);
}

int audio_set_hw_params(snd_pcm_t *pcm, snd_pcm_hw_params_t *params) {
    int rc, dir;
    unsigned int val;
    snd_pcm_uframes_t frames;

    /* Allocate a hardware parameters object. */
    //snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(pcm, params);

    /* Set the desired hardware parameters. */

    // 1. Interleaved mode.
    snd_pcm_hw_params_set_access(pcm, params,
                                 SND_PCM_ACCESS_RW_INTERLEAVED);

    // 2. Singed 16-bit little-endian format.
    snd_pcm_hw_params_set_format(pcm, params,
                                 SND_PCM_FORMAT_S16_LE);

    // 3. Two channels (stereo).
    snd_pcm_hw_params_set_channels(pcm, params, 2);

    // 4. 44100 bits/second sampling rate (CD quality).
    val = 44100;
    snd_pcm_hw_params_set_rate_near(pcm, params, &val, &dir);

    // 5. Set period size to 32 frames.
    frames = 32;
    snd_pcm_hw_params_set_period_size_near(pcm, params, &frames, &dir);

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(pcm, params);
    if (rc < 0) {
        fprintf(stderr, "Unable to set hw parameters: %s\n",
                snd_strerror(rc));
        return -1;
    }

    return 0;
}
