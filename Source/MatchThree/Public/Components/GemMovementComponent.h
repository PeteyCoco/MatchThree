// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "GemMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class MATCHTHREE_API UGemMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

	//~ Begin UMovementComponent interface
protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UMovementComponent interface

public:
	// Move towards the new location with constant acceleration
	void MoveTo(const FVector& NewLocation);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gem Movement")
	float MaxSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gem Movement")
	float Acceleration = 1000.f;

	bool bIsMoving = false;

	FVector TargetLocation;
};
