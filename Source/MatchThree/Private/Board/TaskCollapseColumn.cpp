// Copyright Peter Carsten Collins (2024)


#include "Board/TaskCollapseColumn.h"
#include "GameBoard.h"

void UTaskCollapseColumn::Init(AGameBoard* InGameBoard, int32 InColumn, float InTimerRate)
{
	GameBoard = InGameBoard;
	Column = InColumn;
	TimerRate = InTimerRate;
}

void UTaskCollapseColumn::Execute()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTaskCollapseColumn::TimerCallback, TimerRate, true, 0.f);
}

void UTaskCollapseColumn::TimerCallback()
{
	// Find the next location that can move down
	while (CurrentRow < GameBoard->GetBoardHeight() && !GameBoard->CanMoveDown({ Column, CurrentRow }))
	{
		CurrentRow++;
	}

	if (CurrentRow < GameBoard->GetBoardHeight())
	{
		GameBoard->MoveGemDown({ Column, CurrentRow });
		CurrentRow++;
	}
	else
	{
		Complete();
	}
}
