// Copyright Peter Carsten Collins (2024)


#include "Tasks/TaskSequential.h"


void UTaskSequential::AddTask(UTaskBase* Task)
{
	Tasks.Add(Task);
}

void UTaskSequential::Execute()
{
	// Check that the task array is valid
	for (UTaskBase* Task : Tasks)
	{
		if (!Task)
		{
			UE_LOG(LogTemp, Error, TEXT("A Task in the sequential task is nullptr. Aborting execute."));
			return;
		}
	}

	// Chain the tasks
	for (int i = 1; i < Tasks.Num(); i++)
	{
		Tasks[i - 1]->OnTaskComplete.AddUniqueDynamic(Tasks[i], &UTaskBase::Execute);
	}

	// Mark the task as complete when the final task completes
	Tasks[Tasks.Num() - 1]->OnTaskComplete.AddUniqueDynamic(this, &UTaskBase::Complete);

	// Start the sequence
	Tasks[0]->Execute();
}