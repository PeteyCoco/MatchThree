// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreePawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Core/MatchThreeGameMode.h"
#include "GemBase.h"
#include "Kismet/GameplayStatics.h"
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
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && SelectionIndicator)
	{
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursor(ECC_Gem, true, HitResult);

		if (AGemBase* HitGem = Cast<AGemBase>(HitResult.GetActor()))
		{
			HandleGemClicked(HitGem);
		}
	}
}

void AMatchThreePawn::HandleGemClicked(AGemBase* HitGem)
{
	if (!HitGem)
	{
		ClearSelection();
	}
	else if (!SelectedGem)
	{
		SelectGem(HitGem);
	}
	else if (HitGem != SelectedGem)
	{
		GameMode = !GameMode ? GetGameMode() : GameMode;
		if (GameMode && GameMode->CanSwapGems(HitGem, SelectedGem))
		{
			GameMode->SwapGems(SelectedGem, HitGem);
			ClearSelection();
		}
		else
		{
			ClearSelection();
			SelectGem(HitGem);
		}
	}
	else if (HitGem == SelectedGem)
	{
		ClearSelection();
	}
}

AMatchThreeGameMode* AMatchThreePawn::GetGameMode()
{
	return Cast<AMatchThreeGameMode>(UGameplayStatics::GetGameMode(this));
}

void AMatchThreePawn::ClearSelection()
{
	if (SelectedGem)
	{
		SelectedGem->SetSelected(false);
		SelectedGem = nullptr;
	}
	SelectionIndicator->Show(false);
}

void AMatchThreePawn::SelectGem(AGemBase* NewGem)
{
	if (!NewGem) return;

	SelectedGem = NewGem;
	NewGem->SetSelected(true);
	SelectionIndicator->SetActorLocation(NewGem->GetActorLocation());
	SelectionIndicator->Show(true);
}
