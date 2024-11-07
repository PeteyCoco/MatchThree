// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "GemBase.h"
#include "DrawDebugHelpers.h"

AGameBoard::AGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameBoard::PlaceGem()
{
	FTransform PlaceLocation;
	PlaceLocation.SetLocation(GetActorLocation());

	AGemBase* GemToPlace = GetWorld()->SpawnActorDeferred<AGemBase>(AGemBase::StaticClass(), PlaceLocation);
	GemToPlace->SetData(GemData[EGemType::Square]);
	GemToPlace->FinishSpawning(PlaceLocation);
}

void AGameBoard::BeginPlay()
{
	Super::BeginPlay();

	PlaceGem();
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

