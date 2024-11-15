// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Board/TaskBase.h"
#include "Board/Match.h"
#include "TaskSwapGems.generated.h"

class AGameBoard;

/**
 * 
 */
UCLASS()
class MATCHTHREE_API UTaskSwapGems : public UTaskBase
{
	GENERATED_BODY()
	//~ Begin UTaskBase interface
public:
	virtual void Execute() override;
	//~ End UTaskBase interface

public:
	void Init(AGameBoard* InGameBoard, const FBoardLocation& InLocationA, const FBoardLocation& InLocationB);

protected:
	UFUNCTION()
	void MoveToCompleteCallback(AGemBase* MovedGem);

private:
	UPROPERTY()
	AGameBoard* GameBoard;
	FBoardLocation LocationA;
	FBoardLocation LocationB;

	bool bCallbackCalled = false;
};
