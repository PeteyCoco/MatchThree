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
		SettleRow(CascadeCurrentRow);
		CascadeCurrentRow++;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ending board cascade"), CascadeCurrentRow);
		GetWorld()->GetTimerManager().ClearTimer(CascadeTimer);
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

	// Swap the gems' world position
	CurrentSwap.FirstGem->OnMoveToCompleteDelegate.AddUniqueDynamic(this, &AGameBoard::HandleSwapComplete);
	CurrentSwap.SecondGem->OnMoveToCompleteDelegate.AddUniqueDynamic(this, &AGameBoard::HandleSwapComplete);

	CurrentSwap.FirstGem->MoveTo(GetWorldLocation(SecondGemBoardLocation));
	CurrentSwap.SecondGem->MoveTo(GetWorldLocation(FirstGemBoardLocation));

	// Swap internal board positions
	InternalBoard->SetGem(CurrentSwap.SecondGem, FirstGemBoardLocation);
	InternalBoard->SetGem(CurrentSwap.FirstGem, SecondGemBoardLocation);
}

void AGameBoard::SettleBoard()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting cascade"));
	CascadeCurrentRow = 0;
	GetWorld()->GetTimerManager().SetTimer(CascadeTimer, this, &AGameBoard::CascadeTimerCallback, CascadeRate, true);
}

void AGameBoard::SettleRow(int Row)
{
	TArray<AGemBase*> Gems;
	InternalBoard->GetGemsInRow(Row, Gems);
	for (AGemBase* Gem : Gems)
	{
		if (Gem) Gem->MoveTo(GetWorldLocation(InternalBoard->GetBoardLocation(Gem)));
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
	SettleBoard();
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
	GemToPlace->FinishSpawning(SpawnTransform);
	return GemToPlace;
}

void AGameBoard::HandleSwapComplete()
{
	// Wait for both gems to stop moving
	if (CurrentSwap.FirstGem->IsMoving() || CurrentSwap.SecondGem->IsMoving()) return;

	TArray<AGemBase*> Matches;
	InternalBoard->GetMatches(CurrentSwap.FirstGem, Matches);
	InternalBoard->GetMatches(CurrentSwap.SecondGem, Matches);

	if (Matches.Num() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Swap complete and no matches"));

		// Undo the current swap
		const FBoardLocation FirstGemBoardLocation = InternalBoard->GetBoardLocation(CurrentSwap.FirstGem);
		const FBoardLocation SecondGemBoardLocation = InternalBoard->GetBoardLocation(CurrentSwap.SecondGem);

		// Swap the gems' world position
		CurrentSwap.FirstGem->MoveTo(GetWorldLocation(SecondGemBoardLocation));
		CurrentSwap.SecondGem->MoveTo(GetWorldLocation(FirstGemBoardLocation));

		// Swap internal board positions
		InternalBoard->SetGem(CurrentSwap.SecondGem, FirstGemBoardLocation);
		InternalBoard->SetGem(CurrentSwap.FirstGem, SecondGemBoardLocation);

		// Clear the current swap
		CurrentSwap.FirstGem = nullptr;
		CurrentSwap.SecondGem = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Swap complete and matches"));

		// Destroy matches
		for (AGemBase* Gem : Matches)
		{
			DestroyGem(Gem);
		}

		InternalBoard->Collapse();
		FillBoard();
	}
}

