// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "Board/InternalBoard.h"
#include "GemBase.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

AGameBoard::AGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGameBoard::BeginPlay()
{
	Super::BeginPlay();

	InternalBoard = NewObject<UInternalBoard>(this);
	InitializeInternalBoard();

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

	InternalBoard->SetGem(nullptr, InternalBoard->GetBoardLocation(Gem));
	Gem->Destroy();
}

void AGameBoard::CascadeTimerCallback()
{
	if (CascadeCurrentRow < BoardHeight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cascading row %d"), CascadeCurrentRow);
		CascadeRow(CascadeCurrentRow);
		CascadeCurrentRow++;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ending board cascade"), CascadeCurrentRow);
		GetWorld()->GetTimerManager().ClearTimer(CascadeTimer);
	}
}

void AGameBoard::CheckCascadeRowComplete(AGemBase* InGem)
{
	const int Row = InternalBoard->GetBoardLocation(InGem).Y;
	if (IsRowInPosition(Row))
	{
		RowCascadeCompleteDelegate.Broadcast(Row);
		if (Row == BoardHeight - 1)
		{
			BoardCascadeCompleteDelegate.Broadcast();
		}
	}
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
	UE_LOG(LogTemp, Warning, TEXT("Starting cascade"));
	CascadeCurrentRow = 0;
	while (IsRowInPosition(CascadeCurrentRow))
	{
		CascadeCurrentRow++;
	}
	GetWorld()->GetTimerManager().SetTimer(CascadeTimer, this, &AGameBoard::CascadeTimerCallback, CascadeRate, true);
}

void AGameBoard::CascadeRow(int Row)
{
	TArray<AGemBase*> Gems;
	InternalBoard->GetGemsInRow(Row, Gems);
	for (AGemBase* Gem : Gems)
	{
		if (Gem)
		{
			Gem->OnGemMoveToCompleteDelegate.AddUniqueDynamic(this, &AGameBoard::CheckCascadeRowComplete);
			Gem->MoveTo(GetWorldLocation(InternalBoard->GetBoardLocation(Gem)));
		}
	}
}

void AGameBoard::FillBoard()
{
	TArray<EGemType> GemTypes;
	int NumGemTypes = GemData.GetKeys(GemTypes);

	for (int i = 0; i < BoardWidth; i++)
	{
		const int NumberOfGemsToSpawn = InternalBoard->SpacesAtTop(i);

		for (int j = 0; j < NumberOfGemsToSpawn; j++)
		{
			EGemType GemType = GemTypes[FMath::RandRange(0, NumGemTypes - 1)];
			AGemBase* GemToPlace = SpawnGem(i, GemType);

			InternalBoard->AddGemToTopOfColumn(i, GemToPlace);
		}
	}
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

