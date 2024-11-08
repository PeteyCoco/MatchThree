// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpinnerComponent.generated.h"

UENUM()
enum class ESpinnerState : uint8
{
	Spinning,
	Stopping,
	Stopped,
};

/*
*	Component that spins the actor about an axis 
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MATCHTHREE_API USpinnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpinnerComponent();

	//~ Begin UActorComponent interface
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent interface

public:
	// Start the spinner
	void Start();

	// Stop the spinner
	void Stop();

protected:
	// Selected rotation speed
	UPROPERTY(EditDefaultsOnly, Category = "Spinner Properties")
	FRotator RotationSpeed = FRotator(11.f, 29.f, 31.f);

	// The state of the spinner
	UPROPERTY(VisibleInstanceOnly, Category = "Spinner Properties")
	ESpinnerState State;

	// The rotator of the actor when it started spinning
	FRotator StartRotator;
};
