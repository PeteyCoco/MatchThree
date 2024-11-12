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

	GetWorld()->GetTimerManager().SetTimer(CascadeTimer, this, &UColumnCascader::CascadeTimerCallback, CascadeRate, true, 0.f);
}

void UColumnCascader::CascadeTimerCallback()
{
	if (CurrentRow < GameBoard->GetBoardHeight())
	{
		// Move the gem to the next empty spot below it in the column
		AGemBase* Gem = GameBoard->GetGem({ Column, CurrentRow });
		const FBoardLocation NewBoardLocation = GameBoard->GetNextEmptyLocationBelow(Gem);
		GameBoard->MoveGemToBoardLocation(Gem, NewBoardLocation);
		CurrentRow++;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(CascadeTimer);
	}
}
