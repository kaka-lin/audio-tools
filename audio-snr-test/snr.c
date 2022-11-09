#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "src/wav.h"
#include "src/array.h"

double rms(double *a, int len) 
{
    double sum = 0;
    for (int i = 0; i < len ; i++)
        sum += pow(a[i], 2);
    sum /= len;
    return sqrt(sum);
}

double snr(double sig, double noise) 
{
    return 20 * log10(sig / noise);
}

int main(int argc, char **argv)
{
    int sig_amplitude, signoise_amplitude;
    struct wav_info wav_sig, wav_signoise;
    double *left_sig, *left_signoise, *left_noise;

    /* Init */
    wav_init(&wav_sig);
    wav_init(&wav_signoise);
    
    wav_read(&wav_sig, "audio/signal.wav");
    wav_read(&wav_signoise, "audio/signal_with_noise.wav");

    sig_amplitude = max(&wav_sig);
    signoise_amplitude = max(&wav_signoise);

    /* normalization */
    normalization(&wav_sig);
    normalization(&wav_signoise);

    /* left channel */
    left_sig = calloc(wav_sig.frames, sizeof(double));
    if (left_sig == NULL) {
        fprintf(stderr, "Memory allocation faile.\n");
    }

    for (int i = 0; i < wav_sig.frames; i++) {
        left_sig[i] = wav_sig.norm_data[i*2];
    }

    left_signoise = calloc(wav_sig.frames, sizeof(double));
    if (left_signoise == NULL) {
        fprintf(stderr, "Memory allocation faile.\n");
    }

    for (int i = 0; i < wav_sig.frames; i++) {
        left_signoise[i] = wav_signoise.norm_data[i*2];
    }

    /* noise */
    left_noise = array_sub(left_signoise, left_sig, wav_sig.frames);

    /* snr */
    double rms_sig = rms(left_sig, wav_sig.frames);
    double rms_noise = rms(left_noise, wav_sig.frames);
    double snr_amplitude = snr(rms_sig, rms_noise);
    printf("SNR: %.2fdB\n", snr_amplitude);
}

