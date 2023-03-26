#ifndef CALCULATE_PI_H
#define CALCULATE_PI_H

#include "gui.h"
#include "scheduler.h"
#include <stdint.h>

typedef struct args_st {
  int64_t i;
  int64_t n;
  double result;
  double divisor;
  int8_t sign;
  GtkWidget *row;
} args_t;

void calculate_pi(args_t *args, scheduler_config_t config);

void on_start(size_t id, args_t *args);

void on_continue(size_t id, args_t *args);

void on_pause(size_t id, args_t *args);

void on_end(size_t id, args_t *args);

#endif // !CALCULATE_PI_H
