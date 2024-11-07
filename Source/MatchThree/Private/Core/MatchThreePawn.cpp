// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreePawn.h"

AMatchThreePawn::AMatchThreePawn()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMatchThreePawn::BeginPlay()
{
	Super::BeginPlay();
}

void AMatchThreePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

