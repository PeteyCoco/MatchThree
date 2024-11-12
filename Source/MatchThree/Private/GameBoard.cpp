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

	const FBoardLocation FirstGemBoardLocation = InternalBoard->GetBoardLocation(CurrentSwap.FirstGem);
	const FBoardLocation SecondGemBoardLocation = InternalBoard->GetBoardLocation(CurrentSwap.SecondGem);

	CurrentSwap.FirstGem->MoveTo(GetWorldLocation(SecondGemBoardLocation));
	CurrentSwap.SecondGem->MoveTo(GetWorldLocation(FirstGemBoardLocation));

	// Swap internal board positions
	InternalBoard->SetGem(CurrentSwap.SecondGem, FirstGemBoardLocation);
	InternalBoard->SetGem(CurrentSwap.FirstGem, SecondGemBoardLocation);
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

bool AGameBoard::IsRowInPosition(int Row) const
{
	TArray<AGemBase*> GemsInRow;
	InternalBoard->GetGemsInRow(Row, GemsInRow);
	for (AGemBase* Gem : GemsInRow)
	{
		if (!Gem) continue;

		const FBoardLocation GemBoardLocation = InternalBoard->GetBoardLocation(Gem);
		const float DistSquaredToBoardLocation = FVector::DistSquared(GetWorldLocation(GemBoardLocation), Gem->GetActorLocation());
		if (DistSquaredToBoardLocation > 10.f || Gem->IsMoving())
		{
			return false;
		}
	}
	return true;
}

void AGameBoard::GetMatches(const FBoardLocation& InLocation, TArray<AGemBase*> OutMatch) const
{
	AGemBase* InGem = InternalBoard->GetGem(InLocation);
	return InternalBoard->GetMatches(InGem, OutMatch);
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
	InternalBoard->GetMatches(InGem, Matches);

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
		if (CurrentSwap.FirstGem && !CurrentSwap.FirstGem->IsMoving() && CurrentSwap.SecondGem && !CurrentSwap.SecondGem->IsMoving())
		{ 
			SwapGems(CurrentSwap.SecondGem, CurrentSwap.FirstGem);
			CurrentSwap.FirstGem = nullptr;
			CurrentSwap.SecondGem = nullptr;
		}
	}
}

