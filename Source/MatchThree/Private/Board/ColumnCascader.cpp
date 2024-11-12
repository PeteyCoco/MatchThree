// Copyright Peter Carsten Collins (2024)


#include "Board/ColumnCascader.h"
#include "TimerManager.h"
#include "GameBoard.h"
#include "Board/InternalBoard.h"

void UColumnCascader::Execute(AGameBoard* InGameBoard, int32 InColumn, float CascadeRate)
{
	GameBoard = InGameBoard;
	Column = InColumn;

	if (!InGameBoard) return;

	GetWorld()->GetTimerManager().SetTimer(CascadeTimer, this, &UColumnCascader::CascadeTimerCallback, CascadeRate, true);
}

void UColumnCascader::CascadeTimerCallback()
{
	if (CurrentRow < GameBoard->GetBoardHeight())
	{
		GameBoard->MoveIntoPosition({ Column, CurrentRow });
		CurrentRow++;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(CascadeTimer);
	}
}
