// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TaskAddGemToColumn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskCompleteSignature);

class AGameBoard;
/**
 * 
 */
UCLASS()
class MATCHTHREE_API UTaskAddGemsToColumn : public UObject
{
	GENERATED_BODY()

public:
	void Init(AGameBoard* InGameBoard, int32 InColumn, int32 InNumberToAdd, float InTimerRate);

	void Execute();

	bool IsComplete() const;

	FOnTaskCompleteSignature OnTaskComplete;

private:
	UPROPERTY()
	AGameBoard* GameBoard;

	int32 Column;

	int32 NumberToAdd;
	int32 NumberAdded;

	FTimerHandle TimerHandle;
	float TimerRate;

	UFUNCTION()
	void TimerCallback();

	bool bIsComplete = false;

};
