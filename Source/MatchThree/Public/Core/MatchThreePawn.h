// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MatchThreePawn.generated.h"

class ASelectionIndicator;
class UInputAction;
class UInputMappingContext;

UCLASS()
class MATCHTHREE_API AMatchThreePawn : public APawn
{
	GENERATED_BODY()

	//~ Begin APawn interface
protected:
	virtual void BeginPlay() override;
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//~ End APawn interface

public:
	AMatchThreePawn();

protected:
	/** MappingContext for player input. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	UInputAction* ClickAction;

	UFUNCTION()
	void Click(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Selection")
	TSubclassOf<ASelectionIndicator> SelectionIndicatorClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Selection")
	ASelectionIndicator* SelectionIndicator;
};
