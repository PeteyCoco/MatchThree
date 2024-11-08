// Copyright Peter Carsten Collins (2024)


#include "Components/SpinnerComponent.h"

USpinnerComponent::USpinnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	State = ESpinnerState::Stopped;
}

void USpinnerComponent::BeginPlay()
{
	Super::BeginPlay();	
}


void USpinnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	switch (State)
	{
	case ESpinnerState::Spinning:
		//CurrentRotation += RotationSpeed * DeltaTime;
		//GetOwner()->SetActorRotation(CurrentRotation);
		GetOwner()->AddActorLocalRotation(FQuat(RotationSpeed * DeltaTime));
		break;
	case ESpinnerState::Stopping:
		GetOwner()->SetActorRotation(StartRotator);
		State = ESpinnerState::Stopped;
		break;
	case ESpinnerState::Stopped:
		break;
	default:
		break;
	}
	
}

void USpinnerComponent::Start()
{
	State = ESpinnerState::Spinning;
	StartRotator = GetOwner()->GetActorRotation();
}

void USpinnerComponent::Stop()
{
	State = ESpinnerState::Stopping;
}

