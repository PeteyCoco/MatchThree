// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "Board/InternalBoard.h"
#include "GemBase.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Board/ColumnCascader.h"

AGameBoard::AGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGameBoard::BeginPlay()
{
	Super::BeginPlay();

	InternalBoard = NewObject<UInternalBoard>(this);
	InitializeInternalBoard();

	// Initialize GemsToSpawn map
	for (int Column = 0; Column < BoardWidth; Column++)
	{
		GemsToSpawn.Add(Column, {});
	}

	ResetBoard();
}

void AGameBoard::InitializeInternalBoard()
{
	if (InternalBoard)
	{
		InternalBoard->Initialize(BoardWidth, BoardHeight);
	}
}

void AGameBoard::DestroyGem(AGemBase* Gem)
{
	if (!Gem) return;
	Gem->Destroy();
}



void AGameBoard::ResetBoard()
{
	FillBoard();
}

AGemBase* AGameBoard::GetGem(const FBoardLocation& InLocation) const
{
	return InternalBoard->GetGem(InLocation);
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

int32 AGameBoard::NumberOfGems(int32 Column) const
{
	return InternalBoard->NumberOfGems(Column) + GemsToSpawn[Column].Num();
}

int32 AGameBoard::GetBoardWidth() const
{
	return InternalBoard->GetBoardHeight();
}

int32 AGameBoard::GetBoardHeight() const
{
	return InternalBoard->GetBoardHeight();
}

bool AGameBoard::CanSwapGems(AGemBase* GemA, AGemBase* GemB) const
{
	return InternalBoard->AreNeighbours(GemA, GemB);
}

void AGameBoard::SwapGems(AGemBase* GemA, AGemBase* GemB)
{
	if (!GemA || !GemB) return;

	CurrentSwap.FirstGem = GemA;
	CurrentSwap.SecondGem = GemB;

	const FBoardLocation FirstInLocation = InternalBoard->GetBoardLocation(CurrentSwap.FirstGem);
	const FBoardLocation SecondInLocation = InternalBoard->GetBoardLocation(CurrentSwap.SecondGem);

	CurrentSwap.FirstGem->MoveTo(GetWorldLocation(SecondInLocation));
	CurrentSwap.SecondGem->MoveTo(GetWorldLocation(FirstInLocation));

	// Swap internal board positions
	InternalBoard->SetGem(CurrentSwap.SecondGem, FirstInLocation);
	InternalBoard->SetGem(CurrentSwap.FirstGem, SecondInLocation);
}

void AGameBoard::CascadeBoard()
{
	for (int i = 0; i < BoardWidth; i++)
	{
		CascadeColumn(i);
	}
}

void AGameBoard::CascadeColumn(int Column)
{
	UColumnCascader* ColumnCascader = NewObject<UColumnCascader>(this);
	ColumnCascader->Execute(this, Column, CascadeRate);
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
		if (InternalBoard->ContainsGem(Gem))
		{
			const FBoardLocation OldBoardLocation = InternalBoard->GetBoardLocation(Gem);
			InternalBoard->SetGem(nullptr, OldBoardLocation);
		}
		InternalBoard->SetGem(Gem, NewBoardLocation);
		Gem->MoveTo(GetWorldLocation(NewBoardLocation));
	}
}

void AGameBoard::FillBoard()
{
	CascadeBoard();
}

bool AGameBoard::IsInPosition(AGemBase* InGem) const
{
	if (!InGem) { return false; }

	const FBoardLocation InLocation = InternalBoard->GetBoardLocation(InGem);
	const float DistSquaredToBoardLocation = FVector::DistSquared(GetWorldLocation(InLocation), InGem->GetActorLocation());
	if (DistSquaredToBoardLocation > 10.f || InGem->IsMoving())
	{
		return false;
	}
	return true;
}

bool AGameBoard::IsRowInPosition(int Row) const
{
	TArray<AGemBase*> GemsInRow;
	InternalBoard->GetGemsInRow(Row, GemsInRow);
	for (AGemBase* Gem : GemsInRow)
	{
		if (!Gem) continue;

		const FBoardLocation InLocation = InternalBoard->GetBoardLocation(Gem);
		const float DistSquaredToBoardLocation = FVector::DistSquared(GetWorldLocation(InLocation), Gem->GetActorLocation());
		if (DistSquaredToBoardLocation > 10.f || Gem->IsMoving())
		{
			return false;
		}
	}
	return true;
}

bool AGameBoard::IsEmpty(const FBoardLocation& InLocation) const
{
	return InternalBoard->IsEmpty(InLocation);
}

void AGameBoard::GetMatches(AGemBase* InGem, TArray<AGemBase*>& OutMatch) const
{
	if (!InGem)
	{
		UE_LOG(LogTemp, Error, TEXT("Tried matching to a nullptr gem!"));
		return;
	}
	const FBoardLocation InLocation = InternalBoard->GetBoardLocation(InGem);

	const int XMin = FMath::Max(0, InLocation.X - 2);
	const int XMax = FMath::Min(InLocation.X + 2, BoardWidth - 1);

	// Check horizontal matches
	TArray<AGemBase*> Matches;
	Matches.Add(InGem);

	// Grow the left
	for (int i = InLocation.X - 1; XMin <= i; i--)
	{
		AGemBase* CandidateGem = GetGem({ i,InLocation.Y });
		if (IsInPosition(CandidateGem) && CandidateGem && CandidateGem->GetType() == InGem->GetType())
		{
			Matches.Add(CandidateGem);
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
		OutMatch.Append(Matches);
		return;
	}

	// Check vertical matches
	const int YMin = FMath::Max(0, InLocation.Y - 2);
	const int YMax = FMath::Min(InLocation.Y + 2, BoardHeight - 1);

	Matches.Empty();
	Matches.Add(InGem);

	// Grow the bottom
	for (int i = InLocation.Y - 1; YMin <= i; i--)
	{
		AGemBase* CandidateGem = GetGem({ InLocation.X,i });
		if (IsInPosition(CandidateGem) && CandidateGem && CandidateGem->GetType() == InGem->GetType())
		{
			Matches.Add(CandidateGem);
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
		OutMatch.Append(Matches);
	}
}

AGemBase* AGameBoard::SpawnGem(int32 Column, EGemType GemType)
{
	const int Row = GetColumnHeight(Column);
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
	TArray<AGemBase*> Matches;
	GetMatches(InGem, Matches);

	if (!Matches.IsEmpty())
	{
		for (AGemBase* Gem : Matches)
		{
			InternalBoard->Remove(Gem);
			DestroyGem(Gem);
		}
		CurrentSwap.FirstGem = nullptr;
		CurrentSwap.SecondGem = nullptr;

		CascadeBoard();
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
	return InternalBoard->GetNextEmptyLocationBelow(InLocation);
}

FBoardLocation AGameBoard::GetTopEmptyLocation(int32 Column) const
{
	return InternalBoard->GetTopEmptyLocation(Column);
}

EGemType AGameBoard::GetRandomGemType() const
{
	TArray<EGemType> GemTypes;
	int NumGemTypes = GemData.GetKeys(GemTypes);
	return GemTypes[FMath::RandRange(0, NumGemTypes - 1)];
}

void AGameBoard::QueueGemToSpawn(int32 Column)
{
	GemsToSpawn[Column].Add(GetRandomGemType());
}

EGemType AGameBoard::DequeueGemToSpawn(int32 Column)
{
	if (GemsToSpawn[Column].IsEmpty())
	{
		UE_LOG(LogTemp, Fatal, TEXT("Tried to dequeue gem from empty queue"));
	}
	return GemsToSpawn[Column].Pop();
}

