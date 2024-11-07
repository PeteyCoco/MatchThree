// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreeGameMode.h"

#include "GameBoard.h"
#include "Kismet/GameplayStatics.h"

void AMatchThreeGameMode::BeginPlay()
{
}

void AMatchThreeGameMode::StartPlay()
{
	Super::StartPlay();

	// Collect dependencies
	GameBoard = Cast<AGameBoard>(UGameplayStatics::GetActorOfClass(this, AGameBoard::StaticClass()));
}