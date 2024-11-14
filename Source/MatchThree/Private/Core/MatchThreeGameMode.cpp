// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreeGameMode.h"

#include "GameBoard.h"
#include "Board/Match.h"
#include "Board/TaskAddGemToColumn.h"
#include "Board/TaskCollapseAndFill.h"
#include "Kismet/GameplayStatics.h"

void AMatchThreeGameMode::BeginPlay()
{
}

void AMatchThreeGameMode::StartPlay()
{
	Super::StartPlay();

	// Collect dependencies
	GameBoard = Cast<AGameBoard>(UGameplayStatics::GetActorOfClass(this, AGameBoard::StaticClass()));

	GameBoard->OnMatchFoundDelegate.AddUniqueDynamic(this, &AMatchThreeGameMode::HandleMatchesFound);

	// Fill the columns
	for (int Column = 0; Column < GameBoard->GetBoardWidth(); Column++)
	{
		UTaskAddGemsToColumn* Task = NewObject<UTaskAddGemsToColumn>(this);
		Task->Init(GameBoard, Column, GameBoard->GetBoardHeight(), .2f);
		Task->Execute();
	}
}

void AMatchThreeGameMode::HandleMatchesFound(TArray<FMatch>& Matches)
{
	UE_LOG(LogTemp, Warning, TEXT("Matches found! Destroying..."));

	for (int32 Column = 0; Column < GameBoard->GetBoardWidth(); Column++)
	{
		// Track the number of matched gems in each column
		int32 NumberToAdd = 0;
		for (FMatch& Match : Matches)
		{
			TArray<FBoardLocation> GemLocationsInColumn = Match.GetLocationsInColumn(Column);
			for (const FBoardLocation& GemLocation : GemLocationsInColumn)
			{
				AGemBase* Gem = GameBoard->GetGem(GemLocation);
				GameBoard->Remove(Gem);
				Gem->Destroy();
				NumberToAdd++;
			}
		}

		// Collapse and fill the column
		UTaskCollapseAndFill* TaskCollapseAndFill = NewObject<UTaskCollapseAndFill>(this);
		TaskCollapseAndFill->Init(GameBoard, Column, NumberToAdd, .2f);
		TaskCollapseAndFill->Execute();
	}
}
