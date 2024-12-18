// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Board/TaskBase.h"
#include "TaskAddGemToColumn.generated.h"


class AGameBoard;
/**
 * 
 */
UCLASS()
class MATCHTHREE_API UTaskAddGemsToColumn : public UTaskBase
{
	GENERATED_BODY()

	//~ Begin UTaskBase interface
public:
	virtual void Execute() override;
	//~ End UTaskBase interface

public:
	void Init(AGameBoard* InGameBoard, int32 InColumn, int32 InNumberToAdd, float InTimerRate);

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
};
