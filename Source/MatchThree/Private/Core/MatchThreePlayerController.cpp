// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreePlayerController.h"

#include "DrawDebugHelpers.h"
#include "MatchThree/MatchThree.h"


AMatchThreePlayerController::AMatchThreePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
}

void AMatchThreePlayerController::BeginPlay()
{
	
}

void AMatchThreePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
