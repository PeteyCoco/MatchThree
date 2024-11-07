// Copyright Peter Carsten Collins (2024)


#include "SelectionIndicator.h"

ASelectionIndicator::ASelectionIndicator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASelectionIndicator::BeginPlay()
{
	Super::BeginPlay();
}

void ASelectionIndicator::Show(bool bShouldShow)
{
	SetActorHiddenInGame(!bShouldShow);
}


