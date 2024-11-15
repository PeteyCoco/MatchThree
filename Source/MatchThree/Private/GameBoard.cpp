// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "GemBase.h"
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
	// An empty location is considered as in position
	return !(Gem && Gem->IsMoving());
}

bool AGameBoard::IsEmpty(const FBoardLocation& InLocation) const
{
	return Columns[InLocation.X].IsEmpty(InLocation.Y);
}

void AGameBoard::GetMatch(AGemBase* InGem, FMatch& OutMatch) const
{
	OutMatch = FMatch();

	// Cannot match gems that have been matched already
	if (!InGem || InGem->bCannotMatch)
		return;

	const FBoardLocation InLocation = GetBoardLocation(InGem);

	// Lambda for growing matches in a specific direction
	auto GrowMatches = [&](TArray<FBoardLocation>& Matches, const FBoardLocation& Start, int StepX, int StepY, int Min, int Max)
		{
			for (int i = 1; i <= 2; ++i) // Check up to 2 steps in the specified direction
			{
				int X = Start.X + i * StepX;
				int Y = Start.Y + i * StepY;

				if (X < Min || X > Max || Y < Min || Y > Max)
					break;

				AGemBase* CandidateGem = GetGem({ X, Y });
				if (IsInPosition(CandidateGem) && CandidateGem && !CandidateGem->bCannotMatch && CandidateGem->GetType() == InGem->GetType())
				{
					Matches.Add({ X, Y });
				}
				else
				{
					break;
				}
			}
		};

	// Horizontal match check
	TArray<FBoardLocation> Matches = { InLocation };
	GrowMatches(Matches, InLocation, -1, 0, 0, BoardWidth - 1); // Grow left
	GrowMatches(Matches, InLocation, 1, 0, 0, BoardWidth - 1);  // Grow right

	if (Matches.Num() >= 3)
	{
		OutMatch.AddLocations(Matches);
		return;
	}

	// Vertical match check
	Matches.Empty();
	Matches.Add(InLocation);
	GrowMatches(Matches, InLocation, 0, -1, 0, BoardHeight - 1); // Grow down
	GrowMatches(Matches, InLocation, 0, 1, 0, BoardHeight - 1);  // Grow up

	if (Matches.Num() >= 3)
	{
		OutMatch.AddLocations(Matches);
	}
}

bool AGameBoard::MatchFound(const FBoardLocation& Location, FMatch& OutMatch) const
{
	OutMatch = FMatch();

	// Lambda for growing matches in a specific direction
	auto GrowMatches = [&](TArray<FBoardLocation>& Matches, const FBoardLocation& Start, int StepX, int StepY, int Min, int Max)
		{
			AGemBase* StartGem = GetGem(Start);
			if (!StartGem) return;

			for (int i = 1; i <= 2; ++i) // Check up to 2 steps in the specified direction
			{
				int X = Start.X + i * StepX;
				int Y = Start.Y + i * StepY;

				if (X < Min || X > Max || Y < Min || Y > Max)
					break;

				AGemBase* CandidateGem = GetGem({ X, Y });
				if (IsInPosition(CandidateGem) && CandidateGem && !CandidateGem->bCannotMatch && CandidateGem->GetType() == StartGem->GetType())
				{
					Matches.Add({ X, Y });
				}
				else
				{
					break;
				}
			}
		};

	// Horizontal match check
	TArray<FBoardLocation> Matches = { Location };
	GrowMatches(Matches, Location, -1, 0, 0, BoardWidth - 1); // Grow left
	GrowMatches(Matches, Location, 1, 0, 0, BoardWidth - 1);  // Grow right

	if (Matches.Num() >= 3)
	{
		OutMatch.AddLocations(Matches);
		return true;
	}

	// Vertical match check
	Matches.Empty();
	Matches.Add(Location);
	GrowMatches(Matches, Location, 0, -1, 0, BoardHeight - 1); // Grow down
	GrowMatches(Matches, Location, 0, 1, 0, BoardHeight - 1);  // Grow up

	if (Matches.Num() >= 3)
	{
		OutMatch.AddLocations(Matches);
		return true;
	}
	return false;
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

	if (!Match.IsEmpty())
	{
		MarkAsMatched(Match.GetLocations());
		TArray<FMatch> Matches{ Match };
		OnMatchFoundDelegate.Broadcast(Matches);
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

void AGameBoard::MarkAsMatched(const TArray<FBoardLocation>& Locations)
{
	for (FBoardLocation Location : Locations)
	{
		AGemBase* Gem = GetGem(Location);
		if (Gem) Gem->bCannotMatch = true;
	}
}

bool operator==(const FBoardLocation& A, const FBoardLocation& B)
{
	return A.X == B.X && A.Y == B.Y;
}
