// Copyright Peter Carsten Collins (2024)


#include "Tasks/TaskSwapGems.h"
#include "GameBoard.h"

void UTaskSwapGems::Init(AGameBoard* InGameBoard, const FBoardLocation& InLocationA, const FBoardLocation& InLocationB)
{
	GameBoard = InGameBoard;
	LocationA = InLocationA;
	LocationB = InLocationB;
}

void UTaskSwapGems::Execute()
{
	bCallbackCalled = false;
	AGemBase* GemA = GameBoard->GetGem(LocationA);
	AGemBase* GemB = GameBoard->GetGem(LocationB);

	if (GemA && GemB)
	{
		GemA->bCannotMatch = true;
		GemB->bCannotMatch = true;

		GemA->OnGemMoveToCompleteDelegate.AddUniqueDynamic(this, &UTaskSwapGems::MoveToCompleteCallback);
		GemB->OnGemMoveToCompleteDelegate.AddUniqueDynamic(this, &UTaskSwapGems::MoveToCompleteCallback);

		GemA->MoveTo(GameBoard->GetWorldLocation(LocationB));
		GemB->MoveTo(GameBoard->GetWorldLocation(LocationA));

		GameBoard->SetGem(GemA, LocationB);
		GameBoard->SetGem(GemB, LocationA);
	}
}

void UTaskSwapGems::MoveToCompleteCallback(AGemBase* MovedGem)
{
	if (!bCallbackCalled && GameBoard->IsInPosition(LocationA) && GameBoard->IsInPosition(LocationB))
	{
		bCallbackCalled = true;

		AGemBase* GemA = GameBoard->GetGem(LocationA);
		AGemBase* GemB = GameBoard->GetGem(LocationB);

		GemA->bCannotMatch = false;
		GemB->bCannotMatch = false;

		Complete();
	}
}

