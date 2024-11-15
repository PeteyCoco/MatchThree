// Copyright Peter Carsten Collins (2024)


#include "Board/TaskPool.h"

#include "Board/TaskBase.h"

void UTaskPool::AddTask(UTaskBase* InTask)
{
	// Clean the list if it's full
	if (Tasks.Num() >= MaxTasks)
	{
		Clean();
	}

	// Look for an empty space for the task
	for (int i = 0; i < Tasks.Num(); i++)
	{
		if (Tasks[i] == nullptr)
		{
			Tasks[i] = InTask;
			return;
		}
	}
	
	// Extend the array and add a task
	if (Tasks.Num() < MaxTasks)
	{
		Tasks.Add(InTask);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TaskPool is full with [%d] active tasks. Task not added"), Tasks.Num());
	}
}

void UTaskPool::Clean()
{
	for (int i = 0; i < Tasks.Num(); i++)
	{
		if (Tasks[i] && Tasks[i]->IsComplete())
		{
			Tasks[i] = nullptr;
		}
	}
}

