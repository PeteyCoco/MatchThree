// Copyright Peter Carsten Collins (2024)


#include "Board/InternalBoard.h"
#include "GemBase.h"

void UInternalBoard::Initialize(int32 InBoardWidth, int32 InBoardHeight)
{
	BoardData.Empty();
	BoardData.SetNum(BoardWidth);
	for (int32 ColIndex = 0; ColIndex < BoardWidth; ++ColIndex)
	{
		BoardData[ColIndex].SetNum(BoardHeight);

		for (int32 RowIndex = 0; RowIndex < BoardHeight; ++RowIndex)
		{
			BoardData[ColIndex][RowIndex] = nullptr;
		}
	}
}

void UInternalBoard::SetGem(AGemBase* Gem, const FBoardLocation& BoardLocation)
{
	BoardData[BoardLocation.X][BoardLocation.Y] = Gem;
}

AGemBase* UInternalBoard::GetGem(const FBoardLocation& BoardLocation) const
{
	return BoardData[BoardLocation.X][BoardLocation.Y];
}

AGemBase* UInternalBoard::GetGem(const FBoardLocation& BoardLocation)
{
	return BoardData[BoardLocation.X][BoardLocation.Y];
}

FBoardLocation UInternalBoard::GetBoardLocation(const AGemBase* Gem) const
{
	if (!Gem)
	{
		UE_LOG(LogTemp, Error, TEXT("Gem is nullptr. Returning default location."));
		return FBoardLocation();
	}
	else
	{
		for (int32 ColIndex = 0; ColIndex < BoardWidth; ++ColIndex)
		{
			for (int32 RowIndex = 0; RowIndex < BoardHeight; ++RowIndex)
			{
				if (GetGem({ ColIndex, RowIndex }) == Gem)
				{
					return FBoardLocation(ColIndex, RowIndex);
				}
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Gem [&s] does not exist on the board. Returning default location."), Gem->GetFName());
	return FBoardLocation();
}

bool UInternalBoard::AreNeighbours(AGemBase* GemA, AGemBase* GemB) const
{
	if (GemA && GemB)
	{
		const FBoardLocation GemABoardLocation = GetBoardLocation(GemA);
		const FBoardLocation GemBBoardLocation = GetBoardLocation(GemB);

		const int XDiff = FMath::Abs(GemABoardLocation.X - GemBBoardLocation.X);
		const int YDiff = FMath::Abs(GemABoardLocation.Y - GemBBoardLocation.Y);

		const bool bXNeighbours = XDiff == 1 && YDiff == 0;
		const bool bYNeighbours = XDiff == 0 && YDiff == 1;

		return bXNeighbours || bYNeighbours;
	}
	return false;
}

bool UInternalBoard::IsEmpty(const FBoardLocation& BoardLocation) const
{
	return GetGem(BoardLocation) == nullptr;
}

void UInternalBoard::GetAllGems(TArray<AGemBase*>& OutGems) const
{
	OutGems.Empty();
	OutGems.SetNum(BoardHeight * BoardWidth);
	for (int32 ColIndex = 0; ColIndex < BoardWidth; ++ColIndex)
	{
		for (int32 RowIndex = 0; RowIndex < BoardHeight; ++RowIndex)
		{
			OutGems.Add(GetGem({ ColIndex, RowIndex }));
		}
	}
}

void UInternalBoard::GetGemsInRow(int Row, TArray<AGemBase*>& OutGems) const
{
	OutGems.Empty();
	for (int32 ColIndex = 0; ColIndex < BoardWidth; ++ColIndex)
	{
		OutGems.Add(GetGem({ ColIndex, Row }));
	}
}

int UInternalBoard::SpacesAtTop(int Column) const
{
	int Spaces = 0;
	for (int Row = BoardHeight - 1; Row >= 0; Row--)
	{
		if (IsEmpty({ Column, Row }))
		{
			Spaces++;
		}
		else
		{
			break;
		}
	}
	return Spaces;
}

FBoardLocation UInternalBoard::GetNextEmptyLocationBelow(const FBoardLocation& InLocation) const
{
	FBoardLocation CandidateBoardLocation = InLocation;
	while (CandidateBoardLocation.Y > 0)
	{
		CandidateBoardLocation.Y--;
		if (!IsEmpty(CandidateBoardLocation))
		{
			CandidateBoardLocation.Y++;
			return CandidateBoardLocation;
		}
	}
	return CandidateBoardLocation;
}

FBoardLocation UInternalBoard::GetTopEmptyLocation(int32 Column) const
{
	return GetNextEmptyLocationBelow({Column, BoardHeight - 1});
}

bool UInternalBoard::AddGemToTopOfColumn(int Column, AGemBase* Gem)
{
	if (SpacesAtTop(Column) == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Tried adding a gem to a full column [column:%d]"), Column)
		return false;
	}
	SetGem(Gem, { Column, BoardHeight - SpacesAtTop(Column)});
	return true;
}

int32 UInternalBoard::NumberOfGems(int32 Column) const
{
	int Number = 0;
	for (int Row = 0; Row < BoardHeight; Row++)
	{
		if (!IsEmpty({ Column, Row }))
		{
			Number++;
		}
	}
	return Number;
}

void UInternalBoard::Remove(AGemBase* InGem)
{
	const FBoardLocation BoardLocation = GetBoardLocation(InGem);
	SetGem(nullptr, BoardLocation);
}

bool UInternalBoard::ContainsGem(AGemBase* InGem) const
{
	for (int32 ColIndex = 0; ColIndex < BoardWidth; ++ColIndex)
	{
		for (int32 RowIndex = 0; RowIndex < BoardHeight; ++RowIndex)
		{
			if (BoardData[ColIndex][RowIndex] == InGem)
			{
				return true;
			}
		}
	}
	return false;
}

bool operator==(const FBoardLocation& A, const FBoardLocation& B)
{
	return A.X == B.X && A.Y == B.Y;
}
