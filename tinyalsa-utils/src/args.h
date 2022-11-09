#ifndef ARGS_H
#define ARGS_H

#include "common.h"

void print_usage(char *name, struct audio *audio);

int parse_args(struct audio *audio, int argc, char **argv);

#endif /* ARGS_H */
