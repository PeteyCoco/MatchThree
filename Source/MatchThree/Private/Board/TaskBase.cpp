// Copyright Peter Carsten Collins (2024)


#include "Board/TaskBase.h"

void UTaskBase::Complete()
{
	bIsComplete = true;
	OnTaskComplete.Broadcast();
}
