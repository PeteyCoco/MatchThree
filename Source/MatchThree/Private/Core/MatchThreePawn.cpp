// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreePawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AMatchThreePawn::Click);
}

void AMatchThreePawn::Click(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Click!"));
}