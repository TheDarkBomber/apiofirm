#ifndef __TASK_KERNEL_H
#define __TASK_KERNEL_H
#include "registers.h"
#include <stdint.h>

#define END_TASK for(;;) YieldTask();
#define RESET_TASK(previous) for(;;) { SetTask(previous); YieldTask(); }

typedef struct Task {
	Registers registers;
	struct Task* nextTask;
	uint32_t index;
	uint32_t page;
} Task;

extern uint32_t* PageDirectory;
extern void SwitchTask(Registers* previousTask, Registers* task);

void InitialiseMultitasking();
void CreateTask(Task* task, void(*entry)());
void SetTask(Task* task);
void LockTask();
void UnlockTask();
void YieldTask();
void AppendTask(Task* task);
void DestroyTask(Task* task);
void AntiappendTask(uint32_t task);

#endif
