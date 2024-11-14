// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TaskBase.h"
#include "TaskCollapseColumn.generated.h"


class AGameBoard;

/**
 * 
 */
UCLASS()
class MATCHTHREE_API UTaskCollapseColumn : public UTaskBase
{
	GENERATED_BODY()

	//~ Begin UTaskBase interface
public:
	virtual void Execute() override;
	//~ End UTaskBase interface

public:
	void Init(AGameBoard* InGameBoard, int32 InColumn, float InTimerRate);

	FOnTaskCompleteSignature OnTaskComplete;

private:
	UPROPERTY()
	AGameBoard* GameBoard;

	int32 Column;

	int32 CurrentRow;

	FTimerHandle TimerHandle;
	float TimerRate;

	UFUNCTION()
	void TimerCallback();
};
