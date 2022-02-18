#ifndef __TASK_KERNEL_H
#define __TASK_KERNEL_H
#include "registers.h"
#include <stdint.h>

typedef struct Task {
	Registers registers;
	struct Task* nextTask;
} Task;

extern uint32_t* PageDirectory;
extern void SwitchTask(Registers* previousTask, Registers* task);

void InitialiseMultitasking();
void CreateTask(Task* task, void(*entry)());
void SetTask(Task* task);
void MakeTask(Task primary, Task secondary);
void YieldTask();

#endif
