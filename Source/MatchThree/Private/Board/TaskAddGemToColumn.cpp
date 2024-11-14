// Copyright Peter Carsten Collins (2024)


#include "Board/TaskAddGemToColumn.h"
#include "GameBoard.h"

void UTaskAddGemsToColumn::Init(AGameBoard* InGameBoard, int32 InColumn, int32 InNumberToAdd, float InTimerRate)
{
	GameBoard = InGameBoard;
	Column = InColumn;
	NumberToAdd = InNumberToAdd;
	TimerRate = InTimerRate;
}

void UTaskAddGemsToColumn::Execute()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTaskAddGemsToColumn::TimerCallback, TimerRate, true, 0.f);
}

void UTaskAddGemsToColumn::TimerCallback()
{
	if (NumberAdded < NumberToAdd)
	{
		GameBoard->SpawnGemInColumn(Column);
		NumberAdded++;
	}
	else
	{
		Complete();
	}
}
