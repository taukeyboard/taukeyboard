#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include "CH58xBLE_LIB.h"

#define   MAIN_KEY_EVENT                               0x0001

#define MODE_USB                         0x6b

extern __attribute__((aligned(4))) uint8_t device_mode;
extern tmosTaskID main_task_id;

void main_task_init(void);

#endif