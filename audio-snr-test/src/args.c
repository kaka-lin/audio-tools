#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>

#include "args.h"
#include "audio.h"

static struct option opts[] = {
    {"card", 1, 0, 'D'},       /* Set card name */
    {"device", 1, 0, 'd'},     /* Set device name */
    {"channels", 1, 0, 'c'},   /* Set number of channel */
    {"rate", 1, 0, 'r'},       /* Set sample rate */
    {"format", 1, 0, 'f'},     /* Set sample format */
    {"time", 1, 0, 't'},       /* Set record time */
    {"filename", 1, 0, 's'},   /* Set filename of the audio */
    {"verbose", 0, 0, 'v'},    /* Set verbose mode */
    {"help", 0, 0, 'h'},       /* Show help */
    {0, 0, 0, 0}
};

void print_usage(char *name, struct audio *audio)
{
    printf("Usage: %s [options]\n", name);
	printf("Supported options:\n");
    printf("\t-D card\n");
    printf("\t-d device\n");
    printf("\t-c channels\n");
    printf("\t-r sample rate\n");
    printf("\t-f sample format\n");
    printf("\t-t time of record\n");
    printf("\t-s the filename of the audio file\n");
    printf("\t-v verbose mode\n");
    printf("\t-h show help\n");
    printf("\n");
}

int parse_args(struct audio *audio, int argc, char **argv)
{
    int c;

    while ((c = getopt_long(argc, argv, "hD:d:c:r:f:t:s:v", opts, NULL)) != -1) {
        switch (c) {
        case 'h':
            print_usage(argv[0], audio);
            return 1;
            break;
        case 'D':
            if (optarg)
                audio->card = atoi(optarg);
            break;
        case 'd':
            if (optarg)
                audio->device = atoi(optarg);
            break;
        case 'c':
            if (optarg)
                audio->channels = atoi(optarg);
            break;
        case 'r':
            audio->sample_rate = atoi(optarg);
            break;
        case 'f':
            if (optarg)
                audio->pcm_format = atoi(optarg);
            break;
        case 't':
            if (optarg)
                audio->record_time = atoi(optarg);
            break;
        case 's':
            if (optarg)
                audio->name = optarg;         
            break;
        case 'v': 
            audio->verbose_mode = 1;
            break;
        default:
            printf("Invalid option -%c\n", c);
            printf("Run %s -h for help\n", argv[0]);
            return 1;
        }
    }

    return 0;
}
