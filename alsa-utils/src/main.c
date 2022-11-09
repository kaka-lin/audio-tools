#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

#include "audio.h"

int main(int argc, char **argv) {
  //////////////////////////////////////////////////////////////////////////
  /* 找 Card and device name */

  char **hints;
  /* Enumerate sound devices */
  int err = snd_device_name_hint(-1, "pcm", (void***)&hints);
  if (err != 0)
  return; //Error! Just return

  char **n = hints;
  while (*n != NULL) {
    char *name = snd_device_name_get_hint(*n, "NAME");
    printf("%s\n", name);

    if (name != NULL && 0 != strcmp("null", name)) {
      // Copy name to another buffer and then free it
      free(name);
    }

    n++;
  }

  //Free hint buffer too
  snd_device_name_free_hint((void**)hints);
  printf("\n=========================\n");

  //////////////////////////////////////////////////////////////////////////
  /* 設置 mixer */
  int unmute, chn;
  int al, ar;
  snd_mixer_t *mixer;
  snd_mixer_elem_t *master_element;

  snd_mixer_open(&mixer, 0);
  snd_mixer_attach(mixer, "default"); // default 改為想要的card&device

  snd_mixer_selem_register(mixer, NULL, NULL);
  snd_mixer_load(mixer);

  for(master_element = snd_mixer_first_elem(mixer); master_element; master_element = snd_mixer_elem_next(master_element)) {
	  printf("Device name: %s\n", snd_mixer_selem_get_name(master_element));
  }

  /* 取得第一個 element，也就是 Master */
  master_element = snd_mixer_first_elem(mixer);
  /* 設定音量的範圍 0 ~ 100 */
  snd_mixer_selem_set_playback_volume_range(master_element, 0, 100);

  /* 取得 Master 是否靜音 */
  snd_mixer_selem_get_playback_switch(master_element, 0, &unmute);

  if (unmute) {
    printf("Now, Master is Unmute.\n");
  } else {
    printf("Now, Master is Mute.\n");
  }

  /* 取得左右聲道的音量 */
  snd_mixer_selem_get_playback_volume(master_element, SND_MIXER_SCHN_FRONT_LEFT, &al);
  snd_mixer_selem_get_playback_volume(master_element, SND_MIXER_SCHN_FRONT_RIGHT, &ar);
  /* 兩聲道相加除以二求平均音量 */
  printf("Before, Master volume is %d\n", (al + ar) >> 1);

  /* 設定 Master 音量 */
  snd_mixer_selem_set_playback_volume(master_element, SND_MIXER_SCHN_FRONT_LEFT, 100);
  snd_mixer_selem_set_playback_volume(master_element, SND_MIXER_SCHN_FRONT_RIGHT, 100);
  printf("Setting Master volume: 100\n");

  /* 取得左右聲道的音量 */
  snd_mixer_selem_get_playback_volume(master_element, SND_MIXER_SCHN_FRONT_LEFT, &al);
  snd_mixer_selem_get_playback_volume(master_element, SND_MIXER_SCHN_FRONT_RIGHT, &ar);
  /* 兩聲道相加除以二求平均音量 */
  printf("After, Master volume is %d\n", (al + ar) >> 1);

  /* 將 Master 切換為靜音 */
  for (chn=0;chn<=SND_MIXER_SCHN_LAST;chn++) {
    snd_mixer_selem_set_playback_switch(master_element, chn, 0);
  }
  printf("Setting Master to mute\n");

  /* 取得 Master 是否靜音 */
  snd_mixer_selem_get_playback_switch(master_element, 0, &unmute);
  if (unmute) {
    printf("Now, Master is Unmute.\n");
  } else {
    printf("Now, Master is Mute.\n");
  }

  /* 將 Master 切換為非靜音 */
  for (chn=0;chn<=SND_MIXER_SCHN_LAST;chn++) {
    snd_mixer_selem_set_playback_switch(master_element, chn, 1);
  }
  printf("Setting Master to unmute\n");

  /* 取得左右聲道的音量 */
  snd_mixer_selem_get_playback_volume(master_element, SND_MIXER_SCHN_FRONT_LEFT, &al);
  snd_mixer_selem_get_playback_volume(master_element, SND_MIXER_SCHN_FRONT_RIGHT, &ar);
  /* 兩聲道相加除以二求平均音量 */
  printf("After, Master volume is %d\n", (al + ar) >> 1);
  printf("\n=========================\n");

  //////////////////////////////////////////////////////////////////////////
  /* Open Card&device for capture(record)*/
  snd_pcm_t *pcm;
  snd_pcm_hw_params_t *params;
  snd_pcm_uframes_t frames;
  int rc, dir, size;
  const char *device_name = "default"; // default 改為想要的card&device
  unsigned int val, den;
  long loops;
  char *buffer;

  /* Open PCM device for recording(capture). */
  rc = snd_pcm_open(&pcm, device_name, SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0) {
    fprintf(stderr, "Unable to open pem device: %s\n", snd_strerror(rc));
    return -1;
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Set the desired hardware parameters. */
  audio_set_hw_params(pcm, params);

  /* Display information about the PCM interface */
  audio_show_info(pcm, params, frames, val, den, dir);

  /* Use a buffer large enough to hold one period. */
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  size = frames * 4; // 2 bytes/sample, 2 channels. (left, right)
  buffer = (char *)malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params, &val, &dir);
  // 5 secinds in microseconds divided by period time
  loops = 5000000 / val;

  printf("\n=========================\n");

  //////////////////////////////////////////////////////////////////////////
  /* Start recording */
  while (loops > 0) {
    loops--;

    /* read data from standard input.
     * can redirect to other file
     */
    rc = snd_pcm_readi(pcm, buffer, frames);
    if (rc == -EPIPE) {
      /* EPIPE means underrun
      *
      * underrun: 寫入資料到buffer的速度不夠快,
      *           造成buffer少資料
      * overrun:  從buffer讀取資料的速度不夠快,
      *           buffer被新的資料覆蓋,造成資料的丟失
      */
      fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(pcm);
    } else if (rc < 0) {
      fprintf(stderr, "error from read: %s\n", snd_strerror(rc));
    } else if (rc != (int)frames) {
      fprintf(stderr, "short read, read %d frames\n", rc);
    }

    rc = write(1, buffer, size);
    if (rc != size) fprintf(stderr, "short write: wrote %d bytes\n", rc);
  }
  /* End of record */
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  /* Open Card&device for Playback*/
  //snd_pcm_t *pcm;
  //snd_pcm_hw_params_t *params;
  //snd_pcm_uframes_t frames;
  //int rc, dir, size;
  //const char *device_name = "default"; // default 改為想要的card&device
  //unsigned int val, den;
  //long loops;
  //char *buffer;

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&pcm, device_name, SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr, "Unable to open pem device: %s\n", snd_strerror(rc));
    return -1;
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Set the desired hardware parameters. */
  audio_set_hw_params(pcm, params);

  /* Display information about the PCM interface */
  audio_show_info(pcm, params, frames, val, den, dir);

  /* Use a buffer large enough to hold one period. */
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  size = frames * 4; // 2 bytes/sample, 2 channels. (left, right)
  buffer = (char *)malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params, &val, &dir);
  // 5 secinds in microseconds divided by period time
  loops = 5000000 / val;

  printf("\n=========================\n");

  //////////////////////////////////////////////////////////////////////////
  /* Start playback */

  fd_set fds;
  struct timeval timeout;
  int r;

  while (loops > 0) {
    loops--;

    /* read data from standard input.
     * can redirect to other file
     */

    FD_ZERO(&fds);
    FD_SET(0, &fds);

    timeout.tv_sec = 0;
    timeout.tv_usec = 1000000;

    r = select(0 + 1, &fds, NULL, NULL, &timeout);
    if (-1 == r) {
      printf("%s error %d, %s\n", "select", errno, strerror(errno));
      continue;
    } else if (0 == r) {
      printf(">>>select timeout<<<\n");
      break;
    } else {
      rc = read(0, buffer, size);
      if (rc == 0) {
        fprintf(stderr, "end of file on input\n");
        break;
      } else if (rc != size) {
        fprintf(stderr, "short read: read %d bytes\n", rc);
      }
      rc = snd_pcm_writei(pcm, buffer, frames);
      if (rc == -EPIPE) {
        /* EPIPE means underrun
        *
        * underrun: 寫入資料到buffer的速度不夠快,
        *           造成buffer少資料
        * overrun:  從buffer讀取資料的速度不夠快,
        *           buffer被新的資料覆蓋,造成資料的丟失
        */
        fprintf(stderr, "underrun occurred\n");
        snd_pcm_prepare(pcm);
      } else if (rc < 0) {
        fprintf(stderr, "error from writei: %s\n", snd_strerror(rc));
      } else if (rc != (int)frames) {
        fprintf(stderr, "short write, write %d frames\n", rc);
      }
    }
  }
  /* End of playback */
  //////////////////////////////////////////////////////////////////////////

  snd_pcm_drain(pcm);
  snd_pcm_close(pcm);
  free(buffer);

  return 0;
}
