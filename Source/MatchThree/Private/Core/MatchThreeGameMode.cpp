// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreeGameMode.h"

#include "GameBoard.h"
#include "Board/Match.h"
#include "Board/TaskPool.h"
#include "Board/TaskAddGemToColumn.h"
#include "Board/TaskCollapseAndFill.h"
#include "Score/ScoreActor.h"
#include "Tasks/TaskSwapGems.h"
#include "Tasks/TaskSequential.h"
#include "Kismet/GameplayStatics.h"

void AMatchThreeGameMode::BeginPlay()
{
}

void AMatchThreeGameMode::StartPlay()
{
	Super::StartPlay();

	// Collect dependencies
	TaskPool = NewObject<UTaskPool>(this);
	GameBoard = Cast<AGameBoard>(UGameplayStatics::GetActorOfClass(this, AGameBoard::StaticClass()));

	GameBoard->OnMatchFoundDelegate.AddUniqueDynamic(this, &AMatchThreeGameMode::HandleMatchesFound);

	// Fill the columns
	for (int Column = 0; Column < GameBoard->GetBoardWidth(); Column++)
	{
		UTaskAddGemsToColumn* Task = NewObject<UTaskAddGemsToColumn>(this);
		TaskPool->AddTask(Task);
		Task->Init(GameBoard, Column, GameBoard->GetBoardHeight(), .2f);
		Task->Execute();
	}
}

void AMatchThreeGameMode::SwapGems(AGemBase* GemA, AGemBase* GemB)
{
	// Only one swap action at once
	if (CurrentSwapAction.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Swap already in progress"));
		return;
	}

	// Set the current swap action
	CurrentSwapAction = MakeShared<FSwapPair>();
	CurrentSwapAction->LocationA = GameBoard->GetBoardLocation(GemA);
	CurrentSwapAction->LocationB = GameBoard->GetBoardLocation(GemB);

	// Swap the gems
	UTaskSwapGems* TaskSwapGems = NewObject<UTaskSwapGems>(this);
	TaskSwapGems->Init(GameBoard, CurrentSwapAction->LocationA, CurrentSwapAction->LocationB);
	TaskSwapGems->OnTaskComplete.AddUniqueDynamic(this, &AMatchThreeGameMode::HandleCompletedSwapAction);
	TaskPool->AddTask(TaskSwapGems);
	TaskSwapGems->Execute();
}

bool AMatchThreeGameMode::CanSwapGems(AGemBase* GemA, AGemBase* GemB)
{
	return GameBoard->CanSwapGems(GemA, GemB);
}

void AMatchThreeGameMode::HandleMatchesFound(TArray<FMatch>& Matches)
{
	UE_LOG(LogTemp, Warning, TEXT("Matches found!"));

	for (const FMatch& Match : Matches)
	{
		if (!Match.IsEmpty())
		{
			AScoreActor* ScoreActor = GetWorld()->SpawnActor<AScoreActor>(ScoreActorClass);
			ScoreActor->SetActorLocation(GameBoard->GetWorldLocation(Match.GetLocations()[0]));
		}
	}

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
		TaskPool->AddTask(TaskCollapseAndFill);
		TaskCollapseAndFill->Init(GameBoard, Column, NumberToAdd, .2f);
		TaskCollapseAndFill->Execute();
	}
}

void AMatchThreeGameMode::HandleCompletedSwapAction()
{
	TArray<FMatch> Matches{ {}, {} };
	const bool bMatchFoundAtLocationA = GameBoard->MatchFound(CurrentSwapAction->LocationA, Matches[0]);
	const bool bMatchFoundAtLocationB = GameBoard->MatchFound(CurrentSwapAction->LocationB, Matches[1]);

	if (bMatchFoundAtLocationA || bMatchFoundAtLocationB)
	{
		HandleMatchesFound(Matches);
		ClearCurrentSwapAction();
	}
	else
	{
		// Swap the gems back
		UTaskSwapGems* TaskSwapGems = NewObject<UTaskSwapGems>(this);
		TaskSwapGems->Init(GameBoard, CurrentSwapAction->LocationA, CurrentSwapAction->LocationB);
		TaskSwapGems->OnTaskComplete.AddUniqueDynamic(this, &AMatchThreeGameMode::HandleUndoneSwapAction);
		TaskPool->AddTask(TaskSwapGems);
		TaskSwapGems->Execute();
	}	
}

void AMatchThreeGameMode::HandleUndoneSwapAction()
{
	//TArray<FMatch> Matches{ {}, {} };
	//const bool bMatchFoundAtLocationA = GameBoard->MatchFound(CurrentSwapAction->LocationA, Matches[0]);
	//const bool bMatchFoundAtLocationB = GameBoard->MatchFound(CurrentSwapAction->LocationB, Matches[1]);

	//if (bMatchFoundAtLocationA || bMatchFoundAtLocationB)
	//{
	//	HandleMatchesFound(Matches);
	//}
	ClearCurrentSwapAction();
}

void AMatchThreeGameMode::ClearCurrentSwapAction()
{
	CurrentSwapAction.Reset();
}
