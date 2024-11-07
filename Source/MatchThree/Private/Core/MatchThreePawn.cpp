// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreePawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MatchThree/MatchThree.h"
#include "SelectionIndicator.h"

AMatchThreePawn::AMatchThreePawn()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMatchThreePawn::BeginPlay()
{
	Super::BeginPlay();

	if (SelectionIndicatorClass)
	{
		SelectionIndicator = GetWorld()->SpawnActor<ASelectionIndicator>(SelectionIndicatorClass);
		SelectionIndicator->Show(false);
	}
}

void AMatchThreePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(ClickAction, ETriggerEvent::Completed, this, &AMatchThreePawn::Click);
}

void AMatchThreePawn::Click(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Click!"));

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && SelectionIndicator)
	{
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursor(ECC_Gem, true, HitResult);
		if (HitResult.bBlockingHit)
		{
			SelectionIndicator->Show(true);
			SelectionIndicator->SetActorLocation(HitResult.GetActor()->GetActorLocation());
		}
		else
		{
			SelectionIndicator->Show(false);
		}
	}
}