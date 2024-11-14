// Copyright Peter Carsten Collins (2024)


#include "Board/TaskCollapseAndFill.h"

#include "Board/TaskAddGemToColumn.h"
#include "Board/TaskCollapseColumn.h"

void UTaskCollapseAndFill::Execute()
{
	TaskCollapseColumn->Execute();
}

void UTaskCollapseAndFill::Init(AGameBoard* InGameBoard, int32 InColumn, int32 InNumberToAdd, float InTimerRate)
{
	GameBoard = InGameBoard;
	Column = InColumn;
	NumberToAdd = InNumberToAdd;
	TimerRate = InTimerRate;

	TaskCollapseColumn = NewObject<UTaskCollapseColumn>(this);
	TaskCollapseColumn->Init(GameBoard, Column, TimerRate);

	TaskAddGemsToColumn = NewObject<UTaskAddGemsToColumn>(this);
	TaskAddGemsToColumn->Init(GameBoard, Column, NumberToAdd, TimerRate);

	// Chain the tasks into a macro task
	TaskCollapseColumn->OnTaskComplete.AddUniqueDynamic(TaskAddGemsToColumn, &UTaskBase::Execute);
	TaskAddGemsToColumn->OnTaskComplete.AddUniqueDynamic(this, &UTaskBase::Complete);
}