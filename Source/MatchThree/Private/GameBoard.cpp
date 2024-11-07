// Copyright Peter Carsten Collins (2024)


#include "GameBoard.h"

#include "GemBase.h"
#include "DrawDebugHelpers.h"

AGameBoard::AGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameBoard::ResetBoard()
{
	TArray<EGemType> GemTypes;
	int NumGemTypes = GemData.GetKeys(GemTypes);

	for (int i = 0; i < BoardWidth; i++)
	{
		for (int j = 0; j < BoardHeight; j++)
		{
			EGemType GemType = GemTypes[FMath::RandRange(0, NumGemTypes - 1)];
			SpawnGem(i, GemType);
		}
	}
}

void AGameBoard::SpawnGem(int32 Column, EGemType GemType)
{
	// Spawn at above board
	const int Row = GetColumnHeight(Column);
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation += GetActorRightVector() * Column * CellSpacing;
	SpawnLocation += GetActorForwardVector() * (Row + BoardHeight + DistanceSpawnAboveBoard) * CellSpacing;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);

	AGemBase* GemToPlace = GetWorld()->SpawnActorDeferred<AGemBase>(AGemBase::StaticClass(), SpawnTransform);
	GemToPlace->SetData(GemData[GemType]);
	GemToPlace->SetActorScale3D(FVector(GemScale, GemScale, GemScale));
	GemToPlace->FinishSpawning(SpawnTransform);

	// Set the gem on the board
	Gems[Column][Row] = GemToPlace;
	GemToPlace->MoveTo(GetWorldLocation({ Column, Row }));

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

int32 AGameBoard::GetColumnHeight(int32 Column) const
{
	int32 Height = 0;
	while (Height < BoardHeight && GetGem({ Column, Height }))
	{
		++Height;
	}
	return Height;
}

void AGameBoard::BeginPlay()
{
	Super::BeginPlay();

	InitializeInternalBoard();

	ResetBoard();
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
	//UWorld* World = GetWorld();
	//for (int i = 0; i < BoardWidth; i++)
	//{
	//	for (int j = 0; j < BoardHeight; j++)
	//	{
	//		FBoardLocation BoardLocation(i, j);
	//		FVector CellPosition = GetWorldLocation(BoardLocation);
	//		if (GetGem(BoardLocation))
	//		{
	//			DrawDebugPoint(World, CellPosition, 10.f, FColor::Green);
	//		}
	//		else
	//		{
	//			DrawDebugPoint(World, CellPosition, 10.f, FColor::Black);
	//		}
	//	}
	//}
}

