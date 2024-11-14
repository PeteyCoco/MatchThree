// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TaskPool.generated.h"

class UTaskBase;

/**
 * A class that keeps track of ongoing tasks
 */
UCLASS()
class MATCHTHREE_API UTaskPool : public UObject
{
	GENERATED_BODY()

public:
	void AddTask(UTaskBase* InTask);

private:
	UPROPERTY()
	TArray<UTaskBase*> Tasks;

	// The maximum number of tasks possible;
	int32 MaxTasks = 100.f;

	// Destroy tasks that are complete
	UFUNCTION()
	void Clean();
};
