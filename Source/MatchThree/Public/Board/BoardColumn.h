// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GemBase.h"
#include "BoardColumn.generated.h"

class AGemBase;

/**
 * 
 */
struct FBoardColumn
{
public:
	FBoardColumn();
	FBoardColumn(int32 Height);

	int32 GetHeight() const;

	AGemBase* GetGem(int32 Index) const;
	AGemBase* GetGem(int32 Index);

	void SetGem(AGemBase* InGem, int32 Index);

	bool IsEmpty(int32 Index) const;

	void QueueGemToSpawn(EGemType GemType);
	EGemType DequeueGemToSpawn();

	int32 GetEmptySpaceUnder(int32 Index) const;

	int32 GetTopEmptyIndex() const;

private:
	TArray<AGemBase*> Gems;

	TArray<EGemType> GemsToSpawn;

	bool IsValidIndex(int32 Index) const;
};
