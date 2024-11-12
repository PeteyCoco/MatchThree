// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ColumnCascader.generated.h"


class AGameBoard;

/**
 * 
 */
UCLASS()
class MATCHTHREE_API UColumnCascader : public UObject
{
	GENERATED_BODY()

public:
	void Execute(AGameBoard* InGameBoard, int32 InColumn, float CascadeRate);
	
protected:
	AGameBoard* GameBoard;
	int32 Column;
	int32 CurrentRow;

	UFUNCTION()
	void CascadeTimerCallback();

	FTimerHandle CascadeTimer;
};
