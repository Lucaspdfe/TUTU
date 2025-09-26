#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <hal/hal.h>

#ifndef MAX_TASKS
#define MAX_TASKS 16
#endif

#ifndef TASK_STACK_SIZE
#define TASK_STACK_SIZE 4096
#endif

// Public API
void Scheduler_Init(uint32_t tick_hz, uint32_t default_timeslice_ticks);
Task* Scheduler_AddTask(void (*entry)(void*), void *arg, const char *name);
__attribute__((noreturn)) void Scheduler_Start(void);
void Scheduler_Yield(void);
void Scheduler_Sleep(uint32_t ms);
void Scheduler_Exit(void);

// Must be called by the PIT tick ISR
void Scheduler_OnTickISR(void);

// Optional: query
uint64_t Scheduler_Ticks(void);
Task* Scheduler_Current(void);
