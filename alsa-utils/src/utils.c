#include <stdio.h>

#include "utils.h"

void query_alsa_version() {
  printf("ALSA library version: %s\n", SND_LIB_VERSION_STR);
}

void query_pcm_stream_types() {
  int val;

  printf("\nPCM stream types:\n");
  for (val = 0; val <= SND_PCM_STREAM_LAST; val++) {
    printf("\t%s\n", snd_pcm_stream_name((snd_pcm_stream_t)val));
  }
}

void query_pcm_access_types() {
  int val;

  printf("\nPCM access types:\n");
  for (val = 0; val <= SND_PCM_ACCESS_LAST; val++) {
    printf("\t%s\n", snd_pcm_access_name((snd_pcm_access_t)val));
  }
}

void query_pcm_formats() {
  int val;

  printf("\nPCM formats:\n");
  for (val = 0; val <= SND_PCM_FORMAT_LAST; val++) {
    if (snd_pcm_format_name((snd_pcm_format_t)val) != NULL) {
      printf("\t%s(%s)\n", snd_pcm_format_name((snd_pcm_format_t)val),
             snd_pcm_format_description((snd_pcm_format_t)val));
    }
  }
}

void query_pcm_subformats() {
  int val;

  printf("\nPCM subformats:\n");
  for (val = 0; val <= SND_PCM_SUBFORMAT_LAST; val++) {
    if (snd_pcm_subformat_name((snd_pcm_subformat_t)val) != NULL) {
      printf("\t%s(%s)\n", snd_pcm_subformat_name((snd_pcm_subformat_t)val),
             snd_pcm_subformat_description((snd_pcm_subformat_t)val));
    }
  }
}

void query_pcm_states() {
  int val;

  printf("\nPCM states:\n");
  for (val = 0; val <= SND_PCM_STATE_LAST; val++) {
    printf("\t%s\n", snd_pcm_state_name((snd_pcm_state_t)val));
  }
}

void show_pcm_name(snd_pcm_t *pcm) {
  printf("PCM name: %s\n", snd_pcm_name(pcm));
}

void show_pcm_state(snd_pcm_t *pcm) {
  printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm)));
}

void show_pcm_access_type(snd_pcm_hw_params_t *params,
    unsigned int val) {
  snd_pcm_hw_params_get_access(params, (snd_pcm_access_t *)&val);

  printf("PCM access type: %s\n", snd_pcm_access_name(val));
}

void show_pcm_format(snd_pcm_hw_params_t *params,
    unsigned int val) {
    snd_pcm_hw_params_get_format(params,
        (snd_pcm_format_t *)&val);

    printf("PCM format: '%s'(%s)\n",
        snd_pcm_format_name(val),
        snd_pcm_format_description(val));
}

void show_pcm_subformat(snd_pcm_hw_params_t *params,
    unsigned int val)
{
    snd_pcm_hw_params_get_subformat(params,
        (snd_pcm_subformat_t *)&val);

    printf("PCM subformat: '%s'(%s)\n",
        snd_pcm_subformat_name(val),
        snd_pcm_subformat_description(val));
}

void show_pcm_channels(snd_pcm_hw_params_t *params,
    unsigned int val)
{
    snd_pcm_hw_params_get_channels(params, &val);

    printf("PCM channels: %d\n", val);
}

void show_pcm_rate(snd_pcm_hw_params_t *params,
    unsigned int val, int dir)
{
    snd_pcm_hw_params_get_rate(params, &val, &dir);

    printf("PCM rate: %d bps\n", val);
}

void show_pcm_period_time(snd_pcm_hw_params_t *params,
    unsigned int val, int dir)
{
    snd_pcm_hw_params_get_period_time(params, &val, &dir);

    printf("PCM period time: %d us\n", val);
}

void show_pcm_period_size(snd_pcm_hw_params_t *params,
    snd_pcm_uframes_t frames, int dir)
{
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);

    printf("PCM period size: %d frames\n", (int)frames);
}

void show_pcm_buffer_time(snd_pcm_hw_params_t *params,
    unsigned int val, int dir)
{
    snd_pcm_hw_params_get_buffer_time(params, &val, &dir);

    printf("PCM buffer time: %d us\n", val);
}

void show_pcm_buffer_size(snd_pcm_hw_params_t *params,
    unsigned int val)
{
    snd_pcm_hw_params_get_buffer_size(params,
        (snd_pcm_uframes_t *)&val);

    printf("PCM buffer size: %d frames\n", val);
}

void show_pcm_period(snd_pcm_hw_params_t *params,
    unsigned int val, int dir)
{
    snd_pcm_hw_params_get_periods(params, &val, &dir);

    printf("PCM period per buffer: %d frames\n", val);
}

void show_pcm_rate_numden(snd_pcm_hw_params_t *params,
    unsigned int num, unsigned int den)
{
    // numerator and denominator
    snd_pcm_hw_params_get_rate_numden(params, &num, &den);

    printf("PCM exact rate: %d/%d bps\n", num, den);
}

void show_pcm_sbits(snd_pcm_hw_params_t *params)
{
    int val;
    val = snd_pcm_hw_params_get_sbits(params);

    printf("PCM significant bits: %d\n", val);
}

void show_pcm_tick_time(snd_pcm_hw_params_t *params,
    unsigned int val, int dir)
{
    val = snd_pcm_hw_params_get_tick_time(params, &val, &dir);

    printf("PCM tick time: %d us\n", val);
}

void show_pcm_is_batch(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_is_batch(params);
    printf("PCM is batch: %s\n", val ? "Yes" : "No");
}

void show_pcm_is_block_transfer(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_is_block_transfer(params);
    printf("PCM is block_transfer: %s\n", val ? "Yes" : "No");
}

void show_pcm_is_double(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_is_double(params);
    printf("PCM is double: %s\n", val ? "Yes" : "No");
}

void show_pcm_is_half_duplex(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_is_half_duplex(params);
    printf("PCM is half_duplex: %s\n", val ? "Yes" : "No");
}

void show_pcm_is_joint_duplex(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_is_joint_duplex(params);
    printf("PCM is joint_duplex: %s\n", val ? "Yes" : "No");
}

void show_pcm_can_overrange(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_can_overrange(params);
    printf("PCM can overrange: %s\n", val ? "Yes" : "No");
}

void show_pcm_can_mmap_sample_resolution(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_can_mmap_sample_resolution(params);
    printf("PCM can mmap: %s\n", val ? "Yes" : "No");
}

void show_pcm_can_pause(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_can_pause(params);
    printf("PCM can pause: %s\n", val ? "Yes" : "No");
}

void show_pcm_can_resume(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_can_resume(params);
    printf("PCM can resume: %s\n", val ? "Yes" : "No");
}

void show_pcm_can_sync_start(snd_pcm_hw_params_t *params)
{
    int val;

    val = snd_pcm_hw_params_can_sync_start(params);
    printf("PCM can sync start: %s\n", val ? "Yes" : "No");
}
