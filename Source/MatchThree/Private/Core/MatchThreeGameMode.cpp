// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreeGameMode.h"

#include "GameBoard.h"
#include "Board/TaskAddGemToColumn.h"
#include "Kismet/GameplayStatics.h"

void AMatchThreeGameMode::BeginPlay()
{
}

void AMatchThreeGameMode::StartPlay()
{
	Super::StartPlay();

	// Collect dependencies
	GameBoard = Cast<AGameBoard>(UGameplayStatics::GetActorOfClass(this, AGameBoard::StaticClass()));

	// Fill the columns
	for (int Column = 0; Column < GameBoard->GetBoardWidth(); Column++)
	{
		UTaskAddGemsToColumn* Task = NewObject<UTaskAddGemsToColumn>(this);
		Task->Init(GameBoard, Column, GameBoard->GetBoardHeight(), 1.f);
		Task->Execute();
	}
}