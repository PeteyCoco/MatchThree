// Copyright Peter Carsten Collins (2024)


#include "Board/BoardColumn.h"

FBoardColumn::FBoardColumn()
{
	Gems = {};
}

FBoardColumn::FBoardColumn(int32 Height)
{
	Gems.SetNum(Height);
}

int32 FBoardColumn::GetHeight() const
{
	return Gems.Num();
}

AGemBase* FBoardColumn::GetGem(int32 Index) const
{
	return Gems[Index];
}

AGemBase* FBoardColumn::GetGem(int32 Index)
{
	return Gems[Index];
}

void FBoardColumn::SetGem(AGemBase* InGem, int32 Index)
{
	Gems[Index] = InGem;
}

bool FBoardColumn::Contains(const AGemBase* Gem) const
{
	return GetIndex(Gem) != -1;
}

bool FBoardColumn::IsEmpty(int32 Index) const
{
	return Gems[Index] == nullptr;
}

int32 FBoardColumn::GetIndex(const AGemBase* Gem) const
{
	for (int i = 0; i < Gems.Num(); i++)
	{
		if (Gems[i] == Gem) return i;
	}
	return -1;
}

void FBoardColumn::QueueGemToSpawn(EGemType GemType)
{
	GemsToSpawn.Add(GemType);
}

EGemType FBoardColumn::DequeueGemToSpawn()
{
	if (GemsToSpawn.IsEmpty())
	{
		UE_LOG(LogTemp, Fatal, TEXT("Tried to dequeue gem from empty queue"));
	}
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

int32 FBoardColumn::NumberOfGems() const
{
	int32 Number = 0;
	for (AGemBase* Gem : Gems)
	{
		if (Gem) Number++;
	}
	return Number;
}

int32 FBoardColumn::NumberOfGemsToSpawn() const
{
	return GemsToSpawn.Num();
}

