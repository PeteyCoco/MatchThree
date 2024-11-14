// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Board/TaskBase.h"
#include "TaskCollapseAndFill.generated.h"

class AGameBoard;
class UTaskCollapseColumn;
class UTaskAddGemsToColumn;

/**
 *  Collapse a column and fill it with the specified number of gems
 */
UCLASS()
class MATCHTHREE_API UTaskCollapseAndFill : public UTaskBase
{
	GENERATED_BODY()

	//~ Begin UTaskBase interface
public:
	virtual void Execute() override;
	//~ End UTaskBase interface

public:
	void Init(AGameBoard* InGameBoard, int32 InColumn, int32 InNumberToAdd, float InTimerRate);

	FOnTaskCompleteSignature OnTaskComplete;

private:
	UPROPERTY()
	UTaskCollapseColumn* TaskCollapseColumn;

	UPROPERTY()
	UTaskAddGemsToColumn* TaskAddGemsToColumn;

	UPROPERTY()
	AGameBoard* GameBoard;

	int32 Column;

	int32 NumberToAdd;
	int32 NumberAdded;

	FTimerHandle TimerHandle;
	float TimerRate;
};
