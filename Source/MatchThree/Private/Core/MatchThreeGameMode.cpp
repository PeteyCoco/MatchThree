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

	GameBoard->OnMatchFoundDelegate.AddUniqueDynamic(this, &AMatchThreeGameMode::HandleMatchFound);

	// Fill the columns
	for (int Column = 0; Column < GameBoard->GetBoardWidth(); Column++)
	{
		UTaskAddGemsToColumn* Task = NewObject<UTaskAddGemsToColumn>(this);
		Task->Init(GameBoard, Column, GameBoard->GetBoardHeight(), .2f);
		Task->Execute();
	}
}

void AMatchThreeGameMode::HandleMatchFound(TArray<AGemBase*>& Gems)
{
	UE_LOG(LogTemp, Warning, TEXT("Match found! Destroying..."));

	for (AGemBase* Gem : Gems)
	{
		GameBoard->Remove(Gem);
		Gem->Destroy();
	}

}
