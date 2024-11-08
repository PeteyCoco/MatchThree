// Copyright Peter Carsten Collins (2024)


#include "Core/MatchThreePawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameBoard.h"
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

		AGemBase* HitGem = Cast<AGemBase>(HitResult.GetActor());


		if (!HitGem)
		{
			// Nothing hit
			ClearSelection();
		}
		else if (!SelectedGem)
		{
			// Gem hit, nothing previously selected
			SelectedGem = HitGem;
			SelectedGem->SetSelected(true);

			SelectionIndicator->SetActorLocation(SelectedGem->GetActorLocation());
			SelectionIndicator->Show(true);
		}
		else
		{
			// Gem hit, something previously selected
			if (HitGem != SelectedGem)
			{
				GameBoard = !GameBoard ? GetGameBoard() : GameBoard;
				if (GameBoard)
				{
					if (GameBoard->CanSwapGems(SelectedGem, HitGem))
					{
						GameBoard->SwapGems(SelectedGem, HitGem);
						ClearSelection();
					}
					else
					{
						SelectedGem->SetSelected(false);
						SelectedGem = HitGem;
						SelectionIndicator->SetActorLocation(SelectedGem->GetActorLocation());
					}
				}

			}
		}
	}
}

AGameBoard* AMatchThreePawn::GetGameBoard()
{
	return Cast<AGameBoard>(UGameplayStatics::GetActorOfClass(this, AGameBoard::StaticClass()));
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

}
