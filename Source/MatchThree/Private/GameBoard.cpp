// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "GemBase.h"
#include "DrawDebugHelpers.h"

AGameBoard::AGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameBoard::ResetBoard()
{
	TArray<EGemType> GemTypes;
	int NumGemTypes = GemData.GetKeys(GemTypes);

	for (int i = 0; i < BoardWidth; i++)
	{
		for (int j = 0; j < BoardHeight; j++)
		{
			EGemType GemType = GemTypes[FMath::RandRange(0, NumGemTypes - 1)];
			SpawnGem(i, GemType);
		}
	}
}

void AGameBoard::SpawnGem(int32 Column, EGemType GemType)
{
	// Spawn at above board
	const int Row = GetColumnHeight(Column);
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation += GetActorRightVector() * Column * CellSpacing;
	SpawnLocation += GetActorForwardVector() * (Row + BoardHeight + DistanceSpawnAboveBoard) * CellSpacing;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(GetActorRotation().Quaternion());

	AGemBase* GemToPlace = GetWorld()->SpawnActorDeferred<AGemBase>(GemActorClass, SpawnTransform);
	GemToPlace->SetData(GemData[GemType]);
	GemToPlace->SetActorScale3D(FVector(GemScale, GemScale, GemScale));
	GemToPlace->FinishSpawning(SpawnTransform);

	// Set the gem on the board
	Gems[Column][Row] = GemToPlace;
	GemToPlace->MoveTo(GetWorldLocation({ Column, Row }));

}

AGemBase* AGameBoard::GetGem(const FBoardLocation& InLocation) const
{
	return Gems[InLocation.X][InLocation.Y];
}

FBoardLocation AGameBoard::GetBoardLocation(AGemBase* Gem) const
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
				if (Gems[ColIndex][RowIndex] == Gem)
				{
					return FBoardLocation(ColIndex, RowIndex);
				}
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Gem [&s] does not exist on the board. Returning default location."), Gem->GetFName());
	return FBoardLocation();
}

FVector AGameBoard::GetWorldLocation(const FBoardLocation& InLocation) const
{
	FVector WorldPosition = GetActorLocation();
	WorldPosition += GetActorRightVector() * InLocation.X * CellSpacing;
	WorldPosition += GetActorForwardVector() * InLocation.Y * CellSpacing;
	return WorldPosition;
}

int32 AGameBoard::GetColumnHeight(int32 Column) const
{
	int32 Height = 0;
	while (Height < BoardHeight && GetGem({ Column, Height }))
	{
		++Height;
	}
	return Height;
}

bool AGameBoard::CanSwapGems(AGemBase* GemA, AGemBase* GemB) const
{
	if (GemA && GemB)
	{
		const FBoardLocation GemABoardLocation = GetBoardLocation(GemA);
		const FBoardLocation GemBBoardLocation = GetBoardLocation(GemB);

		const int XDiff =  FMath::Abs(GemABoardLocation.X - GemBBoardLocation.X);
		const int YDiff = FMath::Abs(GemABoardLocation.Y - GemBBoardLocation.Y);

		const bool bXNeighbours = XDiff == 1 && YDiff == 0;
		const bool bYNeighbours = XDiff == 0 && YDiff == 1;

		return bXNeighbours || bYNeighbours;
	}
	return false;
}

void AGameBoard::SwapGems(AGemBase* GemA, AGemBase* GemB)
{
	if (!GemA || !GemB) return;

	FBoardLocation GemABoardLocation = GetBoardLocation(GemA);
	FBoardLocation GemBBoardLocation = GetBoardLocation(GemB);

	Gems[GemABoardLocation.X][GemABoardLocation.Y] = GemB;
	Gems[GemBBoardLocation.X][GemBBoardLocation.Y] = GemA;

	GemA->OnMoveToCompleteDelegate.AddUniqueDynamic(this, &AGameBoard::HandleSwapComplete);
	GemA->MoveTo(GetWorldLocation(GemBBoardLocation));
	GemB->MoveTo(GetWorldLocation(GemABoardLocation));
}

void AGameBoard::GetMatches(AGemBase* Gem, TArray<AGemBase*>& OutArray)
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
		AGemBase* CandidateGem = Gems[i][GemBoardLocation.Y];
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
		AGemBase* CandidateGem = Gems[i][GemBoardLocation.Y];
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
		AGemBase* CandidateGem = Gems[GemBoardLocation.X][i];
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
		AGemBase* CandidateGem = Gems[GemBoardLocation.X][i];
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

void AGameBoard::HandleSwapComplete()
{
	UE_LOG(LogTemp, Warning, TEXT("Swap complete!"));
}

void AGameBoard::BeginPlay()
{
	Super::BeginPlay();

	InitializeInternalBoard();

	ResetBoard();
}

void AGameBoard::InitializeInternalBoard()
{
	Gems.Empty();
	Gems.SetNum(BoardWidth);
	for (int32 ColIndex = 0; ColIndex < BoardWidth; ++ColIndex)
	{
		Gems[ColIndex].SetNum(BoardHeight);

		for (int32 RowIndex = 0; RowIndex < BoardHeight; ++RowIndex)
		{
			Gems[ColIndex][RowIndex] = nullptr;
		}
	}
}

void AGameBoard::Tick(float DeltaTime)
{

}

