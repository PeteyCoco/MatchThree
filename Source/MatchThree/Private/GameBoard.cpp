// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "DrawDebugHelpers.h"

AGameBoard::AGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameBoard::BeginPlay()
{
	Super::BeginPlay();
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

