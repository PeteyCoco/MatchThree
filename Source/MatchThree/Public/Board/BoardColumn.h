// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GemBase.h"
#include "BoardColumn.generated.h"


/**
 * 
 */
USTRUCT()
struct FBoardColumn
{
	GENERATED_BODY();

public:
	FBoardColumn();
	FBoardColumn(int32 Height);

	int32 GetHeight() const;

	AGemBase* GetGem(int32 Index) const;
	AGemBase* GetGem(int32 Index);

	void SetGem(AGemBase* InGem, int32 Index);

	bool Contains(const AGemBase* Gem) const;

	bool IsEmpty(int32 Index) const;

	int32 GetIndex(const AGemBase* Gem) const;

	void QueueGemToSpawn(EGemType GemType);
	EGemType DequeueGemToSpawn();

	int32 GetEmptySpaceUnder(int32 Index) const;

	int32 GetTopEmptyIndex() const;

	int32 NumberOfGems() const;

	int32 NumberOfGemsToSpawn() const;

private:
	TArray<AGemBase*> Gems;

	TArray<EGemType> GemsToSpawn;
};
