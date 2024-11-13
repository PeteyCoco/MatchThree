// Copyright Peter Carsten Collins (2024)


#include "Board/ColumnCascader.h"
#include "TimerManager.h"
#include "GameBoard.h"
#include "Board/InternalBoard.h"
#include "GemBase.h"

void UColumnCascader::Execute(AGameBoard* InGameBoard, int32 InColumn, float CascadeRate)
{
	GameBoard = InGameBoard;
	Column = InColumn;

	if (!InGameBoard) return;

	GapsFilled = 0;
	GapsToFill = GameBoard->GetBoardHeight() - GameBoard->NumberOfGems(Column);

	for (int i = 0; i < GapsToFill; i++)
	{
		GameBoard->QueueGemToSpawn(Column);
	}

	GetWorld()->GetTimerManager().SetTimer(CascadeTimer, this, &UColumnCascader::CascadeTimerCallback, CascadeRate, true, 0.f);
}

void UColumnCascader::CascadeTimerCallback()
{
	if (CurrentRow < GameBoard->GetBoardHeight())
	{
		// Move board gems down to the next empty spot below it in the column
		AGemBase* Gem = GameBoard->GetGem({ Column, CurrentRow });
		const FBoardLocation NewBoardLocation = GameBoard->GetNextEmptyLocationBelow({ Column, CurrentRow });
		GameBoard->MoveGemToBoardLocation(Gem, NewBoardLocation);
		CurrentRow++;
	}
	else if (GapsFilled < GapsToFill)
	{
		// Spawn gems to fill in the gaps
		EGemType GemType = GameBoard->DequeueGemToSpawn(Column);
		AGemBase* Gem = GameBoard->SpawnGem(Column, GemType);
		const FBoardLocation NewBoardLocation = GameBoard->GetTopEmptyLocation(Column);
		GameBoard->MoveGemToBoardLocation(Gem, NewBoardLocation);
		GapsFilled++;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(CascadeTimer);
	}
}
