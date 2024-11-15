// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Board/TaskBase.h"
#include "TaskSequential.generated.h"

/**
 * A task that takes an array of tasks and executes them sequentially
 */
UCLASS()
class MATCHTHREE_API UTaskSequential : public UTaskBase
{
	GENERATED_BODY()

	//~ Begin UTaskBase interface
public:
	virtual void Execute() override;
	//~ End UTaskBase interface

	// Add a task to the sequence
	void AddTask(UTaskBase* Task);

protected:
	UPROPERTY()
	TArray<UTaskBase*> Tasks;
};
