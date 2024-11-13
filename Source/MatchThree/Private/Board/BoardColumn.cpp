// Copyright Peter Carsten Collins (2024)


#include "Board/BoardColumn.h"

FBoardColumn::FBoardColumn()
{
	Gems = {};
}

FBoardColumn::FBoardColumn(int32 Height)
{
	Gems.SetNum(Height);
	Gems.Empty();
}

int32 FBoardColumn::GetHeight() const
{
	return Gems.Num();
}

AGemBase* FBoardColumn::GetGem(int32 Index) const
{
	if (!IsValidIndex(Index)) return nullptr;

	return Gems[Index];
}

AGemBase* FBoardColumn::GetGem(int32 Index)
{
	if (!IsValidIndex(Index)) return nullptr;

	return Gems[Index];
}

void FBoardColumn::SetGem(AGemBase* InGem, int32 Index)
{
	if (!IsValidIndex(Index)) return;

	Gems[Index] = InGem;
}

bool FBoardColumn::IsEmpty(int32 Index) const
{
	if (!IsValidIndex(Index)) return false;

	return Gems[Index] == nullptr;
}

bool FBoardColumn::IsValidIndex(int32 Index) const
{
	if (Index < Gems.Num() && Index >= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Index [%d] out of range!"));
	}
	return true;
}

void FBoardColumn::QueueGemToSpawn(EGemType GemType)
{
	GemsToSpawn.Add(GemType);
}

EGemType FBoardColumn::DequeueGemToSpawn()
{
	return GemsToSpawn.Pop();
}

int32 FBoardColumn::GetEmptySpaceUnder(int32 Index) const
{
	int32 CandidateIndex = Index;
	while (CandidateIndex > 0)
	{
		CandidateIndex--;
		if (!IsEmpty(CandidateIndex))
		{
			CandidateIndex++;
			return CandidateIndex;
		}
	}
	return CandidateIndex;
}

int32 FBoardColumn::GetTopEmptyIndex() const
{
	const int32 TopIndex = GetHeight() - 1;

	if (!IsEmpty(TopIndex))
	{
		return -1;
	}

	return GetEmptySpaceUnder(TopIndex);
}

