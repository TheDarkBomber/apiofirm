#include "task.h"
#include "paging.h"
#include "x86.h"
#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>

static bool taskLocked = false;

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
	primaryTask.index = 0;
	SetTask(&primaryTask);
  YieldTask();
}

void CreateTask(Task *task, void (*entry)()) {
	task->page = AllocateNextFreePage();
  task->registers.EAX = 0;
  task->registers.EBX = 0;
  task->registers.ECX = 0;
  task->registers.EDX = 0;
  task->registers.ESI = 0;
  task->registers.EDI = 0;
  task->registers.EIP = (uint32_t)entry;
  task->registers.CR3 = x86ReadCR3();
	task->registers.EFLAGS = x86ReadEFLAGS();
  task->registers.ESP = task->page;
  task->nextTask = 0;
}

void SetTask(Task *task) { currentTask = task; }

void LockTask() { taskLocked = true; }
void UnlockTask() { taskLocked = false; }

void YieldTask() {
	if (taskLocked) {
		cprint("Task locked.\r\n");
		return;
	}
	Task* previousTask = currentTask;
	currentTask = currentTask->nextTask;
	SwitchTask(&previousTask->registers, &currentTask->registers);
}

void AppendTask(Task* task) {
	Task* next = currentTask->nextTask;
	while (next->nextTask->index != 0) next = next->nextTask;
	next->nextTask = task;
	task->index = (next->index) + 1;
	task->nextTask = currentTask;
}

void DestroyTask(Task* task) {
	AntiappendTask(task->index);
	FreePage(task->page / PAGE_BLOCK, task->page % PAGE_SIZE);
}

void AntiappendTask(uint32_t task) {
	Task* next = currentTask;
	while (next->nextTask->index != task) next = next->nextTask;
	next->nextTask->nextTask->index = next->nextTask->index;
	next->nextTask = next->nextTask->nextTask;
}
