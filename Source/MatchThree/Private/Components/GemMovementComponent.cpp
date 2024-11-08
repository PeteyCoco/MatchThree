// Copyright Peter Carsten Collins (2024)


#include "Components/GemMovementComponent.h"

void UGemMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsMoving)
	{
        const FVector CurrentLocation = GetOwner()->GetActorLocation();
        const float CurrentDistanceToTarget = FVector::DistSquared(CurrentLocation, TargetLocation);

        if (CurrentDistanceToTarget < 1.f)
        {
            bIsMoving = false;
            GetOwner()->SetActorLocation(TargetLocation);
            Velocity = FVector::ZeroVector;
            return;
        }

        const FVector DirectionToTarget = (TargetLocation - CurrentLocation).GetSafeNormal();
        const FVector DeltaVelocity = Acceleration * DirectionToTarget * DeltaTime;
        Velocity += DeltaVelocity;
        Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);

        const FVector NewLocation = CurrentLocation + Velocity * DeltaTime;
        const float NewDistanceToTarget = FVector::DistSquared(NewLocation, TargetLocation);
        if (NewDistanceToTarget > CurrentDistanceToTarget)
        {
            bIsMoving = false;
            GetOwner()->SetActorLocation(TargetLocation);
            Velocity = FVector::ZeroVector;
        }
        else
        {
            GetOwner()->SetActorLocation(NewLocation);
        }
	}
}

void UGemMovementComponent::MoveTo(const FVector& NewLocation)
{
	bIsMoving = true;
	TargetLocation = NewLocation;
}
