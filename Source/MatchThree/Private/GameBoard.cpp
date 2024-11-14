// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "GemBase.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Board/BoardColumn.h"

AGameBoard::AGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGameBoard::BeginPlay()
{
	Super::BeginPlay();

	for (int Column = 0; Column < BoardWidth; Column++)
	{
		Columns.Add(FBoardColumn(BoardHeight));
		for (int Row = 0; Row < BoardHeight; Row++)
		{
			QueueGemToSpawn(Column);
		}
	}
}

void AGameBoard::DestroyGem(AGemBase* Gem)
{
	if (!Gem) return;
	Gem->Destroy();
}

AGemBase* AGameBoard::GetGem(const FBoardLocation& InLocation) const
{
	return Columns[InLocation.X].GetGem(InLocation.Y);
}

void AGameBoard::SetGem(AGemBase* Gem, const FBoardLocation& BoardLocation)
{
	Columns[BoardLocation.X].SetGem(Gem, BoardLocation.Y);
}

bool AGameBoard::ContainsGem(AGemBase* InGem) const
{
	for (const FBoardColumn& Column : Columns)
	{
		if (Column.Contains(InGem)) return true;
	}
	return false;
}

FVector AGameBoard::GetWorldLocation(const FBoardLocation& InLocation) const
{
	FVector WorldPosition = GetActorLocation();
	WorldPosition += GetActorRightVector() * InLocation.X * CellSpacing;
	WorldPosition += GetActorForwardVector() * InLocation.Y * CellSpacing;
	return WorldPosition;
}

int32 AGameBoard::NumberOfGems(int32 Column) const
{
	return Columns[Column].NumberOfGems() + Columns[Column].NumberOfGemsToSpawn();
}

int32 AGameBoard::GetBoardWidth() const
{
	return BoardWidth;
}

int32 AGameBoard::GetBoardHeight() const
{
	return BoardHeight;
}

bool AGameBoard::CanSwapGems(AGemBase* GemA, AGemBase* GemB) const
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

void AGameBoard::SwapGems(AGemBase* GemA, AGemBase* GemB)
{
	if (!GemA || !GemB) return;

	CurrentSwap.FirstGem = GemA;
	CurrentSwap.SecondGem = GemB;

	const FBoardLocation FirstInLocation = GetBoardLocation(CurrentSwap.FirstGem);
	const FBoardLocation SecondInLocation = GetBoardLocation(CurrentSwap.SecondGem);

	CurrentSwap.FirstGem->MoveTo(GetWorldLocation(SecondInLocation));
	CurrentSwap.SecondGem->MoveTo(GetWorldLocation(FirstInLocation));

	// Swap internal board positions
	SetGem(CurrentSwap.SecondGem, FirstInLocation);
	SetGem(CurrentSwap.FirstGem, SecondInLocation);
}

void AGameBoard::MoveIntoPosition(const FBoardLocation& BoardLocation)
{
	AGemBase* Gem = GetGem(BoardLocation);
	if (Gem)
	{
		Gem->MoveTo(GetWorldLocation(BoardLocation));
	}
}

void AGameBoard::MoveGemToBoardLocation(AGemBase* Gem, const FBoardLocation& NewBoardLocation)
{
	if (Gem)
	{
		if (ContainsGem(Gem))
		{
			const FBoardLocation OldBoardLocation = GetBoardLocation(Gem);
			SetGem(nullptr, OldBoardLocation);
		}
		SetGem(Gem, NewBoardLocation);
		Gem->MoveTo(GetWorldLocation(NewBoardLocation));
	}
}

bool AGameBoard::IsInPosition(AGemBase* InGem) const
{
	if (!InGem) { return false; }

	const FBoardLocation InLocation = GetBoardLocation(InGem);
	const float DistSquaredToBoardLocation = FVector::DistSquared(GetWorldLocation(InLocation), InGem->GetActorLocation());
	if (DistSquaredToBoardLocation > 10.f || InGem->IsMoving())
	{
		return false;
	}
	return true;
}

bool AGameBoard::IsInPosition(const FBoardLocation& InLocation) const
{
	const AGemBase* Gem = GetGem(InLocation);
	if (!Gem) return true; // An empty location is considered as in position

	const float DistSquaredToBoardLocation = FVector::DistSquared(GetWorldLocation(InLocation), Gem->GetActorLocation());
	if (DistSquaredToBoardLocation > 10.f || Gem->IsMoving())
	{
		return false;
	}
	return true;
}

bool AGameBoard::IsEmpty(const FBoardLocation& InLocation) const
{
	return Columns[InLocation.X].IsEmpty(InLocation.Y);
}

void AGameBoard::GetMatch(AGemBase* InGem, FMatch& OutMatch) const
{
	if (!InGem)
	{
		UE_LOG(LogTemp, Error, TEXT("Tried matching to a nullptr gem!"));
		return;
	}
	const FBoardLocation InLocation = GetBoardLocation(InGem);

	const int XMin = FMath::Max(0, InLocation.X - 2);
	const int XMax = FMath::Min(InLocation.X + 2, BoardWidth - 1);

	// Check horizontal matches
	TArray<FBoardLocation> Matches;
	Matches.Add(InLocation);

	// Grow the left
	for (int i = InLocation.X - 1; XMin <= i; i--)
	{
		AGemBase* CandidateGem = GetGem({ i,InLocation.Y });
		if (IsInPosition(CandidateGem) && CandidateGem && CandidateGem->GetType() == InGem->GetType())
		{
			Matches.Add({i,InLocation.Y });
		}
		else
		{
			break;
		}
	}
	// Grow the right
	for (int i = InLocation.X + 1; i <= XMax; i++)
	{
		AGemBase* CandidateGem = GetGem({ i,InLocation.Y });
		if (IsInPosition(CandidateGem) && CandidateGem && CandidateGem->GetType() == InGem->GetType())
		{
			Matches.Add({ i,InLocation.Y });
		}
		else
		{
			break;
		}
	}

	// Check that at least 3 matches occured
	if (Matches.Num() >= 3)
	{
		OutMatch.AddLocations(Matches);
		return;
	}

	// Check vertical matches
	const int YMin = FMath::Max(0, InLocation.Y - 2);
	const int YMax = FMath::Min(InLocation.Y + 2, BoardHeight - 1);

	Matches.Empty();
	Matches.Add(InLocation);

	// Grow the bottom
	for (int i = InLocation.Y - 1; YMin <= i; i--)
	{
		AGemBase* CandidateGem = GetGem({ InLocation.X,i });
		if (IsInPosition(CandidateGem) && CandidateGem && CandidateGem->GetType() == InGem->GetType())
		{
			Matches.Add({ InLocation.X,i });
		}
		else
		{
			break;
		}
	}
	// Grow the top
	for (int i = InLocation.Y + 1; i <= YMax; i++)
	{
		AGemBase* CandidateGem = GetGem({ InLocation.X,i });
		if (IsInPosition(CandidateGem) && CandidateGem && CandidateGem->GetType() == InGem->GetType())
		{
			Matches.Add({ InLocation.X,i });
		}
		else
		{
			break;
		}
	}

	// Check that at least 3 matches occured
	if (Matches.Num() >= 3)
	{
		OutMatch.AddLocations(Matches);
	}
}

AGemBase* AGameBoard::SpawnGem(int32 Column, EGemType GemType)
{
	const int Row = Columns[Column].GetHeight();
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation += GetActorRightVector() * Column * CellSpacing;
	SpawnLocation += GetActorForwardVector() * (BoardHeight + DistanceSpawnAboveBoard) * CellSpacing;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(GetActorRotation().Quaternion());

	AGemBase* GemToPlace = GetWorld()->SpawnActorDeferred<AGemBase>(GemActorClass, SpawnTransform);
	GemToPlace->SetData(GemData[GemType]);
	GemToPlace->SetActorScale3D(FVector(GemScale, GemScale, GemScale));
	GemToPlace->OnGemMoveToCompleteDelegate.AddUniqueDynamic(this, &AGameBoard::HandleGemMoveToComplete);
	GemToPlace->FinishSpawning(SpawnTransform);
	return GemToPlace;
}

void AGameBoard::HandleGemMoveToComplete(AGemBase* InGem)
{
	// Look for matches
	FMatch Match;
	GetMatch(InGem, Match);

	if (!Match.GemLocations.IsEmpty())
	{
		TArray<FMatch> Matches{ Match };
		OnMatchFoundDelegate.Broadcast(Matches);
		CurrentSwap.FirstGem = nullptr;
		CurrentSwap.SecondGem = nullptr;
	}
	else
	{
		if (IsInPosition(CurrentSwap.FirstGem) && IsInPosition(CurrentSwap.SecondGem))
		{ 
			SwapGems(CurrentSwap.SecondGem, CurrentSwap.FirstGem);
			CurrentSwap.FirstGem = nullptr;
			CurrentSwap.SecondGem = nullptr;
		}
	}
}

FBoardLocation AGameBoard::GetNextEmptyLocationBelow(const FBoardLocation& InLocation) const
{
	FBoardLocation OutLocation;
	OutLocation.X = InLocation.X;
	OutLocation.Y = Columns[InLocation.X].GetEmptySpaceUnder(InLocation.Y);
	return OutLocation;
}

FBoardLocation AGameBoard::GetTopEmptyLocation(int32 Column) const
{
	FBoardLocation OutLocation;
	OutLocation.X = Column;
	OutLocation.Y = Columns[Column].GetTopEmptyIndex();
	return OutLocation;
}

EGemType AGameBoard::GetRandomGemType() const
{
	TArray<EGemType> GemTypes;
	int NumGemTypes = GemData.GetKeys(GemTypes);
	return GemTypes[FMath::RandRange(0, NumGemTypes - 1)];
}

void AGameBoard::QueueGemToSpawn(int32 Column)
{
	Columns[Column].QueueGemToSpawn(GetRandomGemType());
}

EGemType AGameBoard::DequeueGemToSpawn(int32 Column)
{
	QueueGemToSpawn(Column);
	return Columns[Column].DequeueGemToSpawn();
}

void AGameBoard::MoveGemDown(const FBoardLocation& InLocation)
{
	AGemBase* Gem = GetGem(InLocation);
	const FBoardLocation NewBoardLocation = GetNextEmptyLocationBelow(InLocation);
	MoveGemToBoardLocation(Gem, NewBoardLocation);
}

bool AGameBoard::CanMoveDown(const FBoardLocation& InLocation) const
{
	FBoardLocation LocationBelow = InLocation;
	LocationBelow.Y--;
	return !IsEmpty(InLocation) && LocationBelow.Y >= 0 && IsEmpty(LocationBelow);
}

void AGameBoard::SpawnGemInColumn(int32 Column)
{
	EGemType GemType = DequeueGemToSpawn(Column);
	AGemBase* Gem = SpawnGem(Column, GemType);
	const FBoardLocation NewBoardLocation = GetTopEmptyLocation(Column);
	MoveGemToBoardLocation(Gem, NewBoardLocation);
}

FBoardLocation AGameBoard::GetBoardLocation(const AGemBase* Gem) const
{
	if (!Gem)
	{
		UE_LOG(LogTemp, Error, TEXT("Gem is nullptr. Returning default location."));
		return FBoardLocation();
	}
	else
	{
		FBoardLocation BoardLocation;
		for (int i = 0; i < Columns.Num(); i++)
		{
			int Index = Columns[i].GetIndex(Gem);
			if (Index == -1)
			{
				continue;
			}
			else
			{
				BoardLocation.X = i;
				BoardLocation.Y = Index;
				return BoardLocation;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Gem [&s] does not exist on the board. Returning default location."), Gem->GetFName());
	return FBoardLocation();
}

void AGameBoard::Remove(AGemBase* InGem)
{
	const FBoardLocation BoardLocation = GetBoardLocation(InGem);
	SetGem(nullptr, BoardLocation);
}

bool operator==(const FBoardLocation& A, const FBoardLocation& B)
{
	return A.X == B.X && A.Y == B.Y;
}
