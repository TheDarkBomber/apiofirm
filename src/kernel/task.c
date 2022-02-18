#include "task.h"
#include "paging.h"
#include "x86.h"
#include "stdio.h"
#include <stdint.h>

static Task *currentTask;
static Task primaryTask;
static Task secondaryTask;

static void initialNewTask() {
	setDefaultColour(VGA_LIGHT_GREEN);
	print("[KERNEL] Multitasking initiated.\r\n");
	setDefaultColour(VGA_YELLOW);
	YieldTask();
}

void InitialiseMultitasking() {
	primaryTask.registers.CR3 = x86ReadCR3();
	primaryTask.registers.EFLAGS = x86ReadEFLAGS();
  CreateTask(&secondaryTask, initialNewTask);
  primaryTask.nextTask = &secondaryTask;
  secondaryTask.nextTask = &primaryTask;
	SetTask(&primaryTask);
  YieldTask();
}

void CreateTask(Task *task, void (*entry)()) {
  task->registers.EAX = 0;
  task->registers.EBX = 0;
  task->registers.ECX = 0;
  task->registers.EDX = 0;
  task->registers.ESI = 0;
  task->registers.EDI = 0;
  task->registers.EIP = (uint32_t)entry;
  task->registers.CR3 = x86ReadCR3();
	task->registers.EFLAGS = x86ReadEFLAGS();
  task->registers.ESP = AllocateNextFreePage();
  task->nextTask = 0;
}

void SetTask(Task *task) { currentTask = task; }

void YieldTask() {
	Task* previousTask = currentTask;
	currentTask = currentTask->nextTask;
	SwitchTask(&previousTask->registers, &currentTask->registers);
}
