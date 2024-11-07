// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "GemBase.h"
#include "DrawDebugHelpers.h"

AGameBoard::AGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameBoard::SpawnGem(int32 Column)
{
	// Spawn at above board
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation += GetActorRightVector() * Column * CellSpacing;
	SpawnLocation += GetActorForwardVector() * 20.f * CellSpacing;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);

	AGemBase* GemToPlace = GetWorld()->SpawnActorDeferred<AGemBase>(AGemBase::StaticClass(), SpawnTransform);
	GemToPlace->SetData(GemData[EGemType::Square]);
	GemToPlace->FinishSpawning(SpawnTransform);

	// Set the gem on the board
	Gems[Column][0] = GemToPlace;
	GemToPlace->MoveTo(GetWorldLocation({ Column, 0 }));

}

AGemBase* AGameBoard::GetGem(const FBoardLocation& InLocation) const
{
	return Gems[InLocation.X][InLocation.Y];
}

FVector AGameBoard::GetWorldLocation(const FBoardLocation& InLocation) const
{
	FVector WorldPosition = GetActorLocation();
	WorldPosition += GetActorRightVector() * InLocation.X * CellSpacing;
	WorldPosition += GetActorForwardVector() * InLocation.Y * CellSpacing;
	return WorldPosition;
}

void AGameBoard::BeginPlay()
{
	Super::BeginPlay();

	InitializeInternalBoard();

	SpawnGem(0);
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
	UWorld* World = GetWorld();
	for (int i = 0; i < BoardWidth; i++)
	{
		for (int j = 0; j < BoardHeight; j++)
		{
			FBoardLocation BoardLocation(i, j);
			FVector CellPosition = GetWorldLocation(BoardLocation);
			if (GetGem(BoardLocation))
			{
				DrawDebugPoint(World, CellPosition, 10.f, FColor::Green);
			}
			else
			{
				DrawDebugPoint(World, CellPosition, 10.f, FColor::Black);
			}
		}
	}
}

