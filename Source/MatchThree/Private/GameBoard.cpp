// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "GemBase.h"
#include "DrawDebugHelpers.h"

AGameBoard::AGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameBoard::PlaceGem(const FBoardLocation& InCoordinate)
{
	FVector CellPosition = GetActorLocation();
	CellPosition += GetActorRightVector() * InCoordinate.X * CellSpacing;
	CellPosition += GetActorForwardVector() * InCoordinate.Y * CellSpacing;
	FTransform PlaceTransform;
	PlaceTransform.SetLocation(CellPosition);

	AGemBase* GemToPlace = GetWorld()->SpawnActorDeferred<AGemBase>(AGemBase::StaticClass(), PlaceTransform);
	GemToPlace->SetData(GemData[EGemType::Square]);
	GemToPlace->FinishSpawning(PlaceTransform);
}

void AGameBoard::BeginPlay()
{
	Super::BeginPlay();

	PlaceGem(FBoardLocation(0, 0));
	PlaceGem(FBoardLocation(1, 0));
}

void AGameBoard::Tick(float DeltaTime)
{
	UWorld* World = GetWorld();
	for (int i = 0; i < BoardWidth; i++)
	{
		for (int j = 0; j < BoardHeight; j++)
		{
			FVector CellPosition = GetActorLocation();
			CellPosition += GetActorForwardVector() * j * CellSpacing;
			CellPosition += GetActorRightVector() * i * CellSpacing;
			DrawDebugPoint(World, CellPosition, 10.f, FColor::Green);
		}
	}
}

