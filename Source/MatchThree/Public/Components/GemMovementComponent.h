// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "GemMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveToCompleteSignature);

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

	// Returns true if the gem is moving
	bool IsMoving() const { return bIsMoving; }

	// Delegate to broadcast on MoveTo complete
	FOnMoveToCompleteSignature OnMoveToCompleteDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gem Movement")
	float MaxSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gem Movement")
	float Acceleration = 1000.f;

	bool bIsMoving = false;

	FVector TargetLocation;

	void FinishMoveTo();
};
