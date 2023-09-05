#ifndef LIGHT_SCHEDULER_H
#define LIGHT_SCHEDULER_H

#include "LightControl.h"
#include "TimeService.h"
#include "LightControlSpy.h"
// TODO: Define the light scheduler interface here
typedef struct{
        int id;
        Time time;
        int event;
} LightEvent;

void initializer(void);
void LightScheduler_init(void);
void LightScheduler_destroy(void);
void LightScheduler_schedule(int id, Time time ,int event);
void LightScheduler_unschedule(int id, Time time, int event);
void LightScheduler_wakeUp(void);
int LightScheduler_getEventCounter(void);
const char* LightScheduler_getSizeError(void);
const char* LightScheduler_getIdExceededError(void);
#endif
