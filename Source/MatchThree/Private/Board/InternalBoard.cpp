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

FBoardLocation UInternalBoard::GetBoardLocation(AGemBase* Gem) const
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

void UInternalBoard::GetMatches(AGemBase* Gem, TArray<AGemBase*>& OutArray) const
{
	if (!Gem) return;

	FBoardLocation GemBoardLocation = GetBoardLocation(Gem);

	const int XMin = FMath::Max(0, GemBoardLocation.X - 2);
	const int XMax = FMath::Min(GemBoardLocation.X + 2, BoardWidth - 1);

	// Check horizontal matches
	TArray<AGemBase*> Matches;
	Matches.Add(Gem);

	// Grow the left
	for (int i = GemBoardLocation.X - 1; XMin <= i; i--)
	{
		AGemBase* CandidateGem = GetGem({ i,GemBoardLocation.Y });
		if (CandidateGem->GetType() == Gem->GetType())
		{
			Matches.Add(CandidateGem);
		}
		else
		{
			break;
		}
	}
	// Grow the right
	for (int i = GemBoardLocation.X + 1; i <= XMax; i++)
	{
		AGemBase* CandidateGem = GetGem({ i,GemBoardLocation.Y });
		if (CandidateGem->GetType() == Gem->GetType())
		{
			Matches.Add(CandidateGem);
		}
		else
		{
			break;
		}
	}

	// Check that at least 3 matches occured
	if (Matches.Num() >= 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Match of length %d along the horizontal"), Matches.Num());
		OutArray.Append(Matches);
		return;
	}

	// Check vertical matches
	const int YMin = FMath::Max(0, GemBoardLocation.Y - 2);
	const int YMax = FMath::Min(GemBoardLocation.Y + 2, BoardHeight - 1);

	Matches.Empty();
	Matches.Add(Gem);

	// Grow the bottom
	for (int i = GemBoardLocation.Y - 1; YMin <= i; i--)
	{
		AGemBase* CandidateGem = GetGem({ GemBoardLocation.X,i });
		if (CandidateGem->GetType() == Gem->GetType())
		{
			Matches.Add(CandidateGem);
		}
		else
		{
			break;
		}
	}
	// Grow the top
	for (int i = GemBoardLocation.Y + 1; i <= YMax; i++)
	{
		AGemBase* CandidateGem = GetGem({ GemBoardLocation.X,i });
		if (CandidateGem->GetType() == Gem->GetType())
		{
			Matches.Add(CandidateGem);
		}
		else
		{
			break;
		}
	}

	// Check that at least 3 matches occured
	if (Matches.Num() >= 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Match of length %d along the vertical"), Matches.Num());
		OutArray.Append(Matches);
	}
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
