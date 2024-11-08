// Copyright Peter Carsten Collins (2024)


#include "GemBase.h"

#include "Components/SpinnerComponent.h"
#include "Gem/GemDataAsset.h"
#include "MatchThree/MatchThree.h"

AGemBase::AGemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(StaticMesh);

	StaticMesh->SetCollisionResponseToChannel(ECC_Gem, ECR_Block);

	SpinnerComponent = CreateDefaultSubobject<USpinnerComponent>("SpinnerComponent");

	bIsMoving = false;
	bIsSelected = false;
}

void AGemBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGemBase::Tick(float DeltaTime)
{
	if (bIsMoving)
	{
		const FVector CurrentLocation = GetActorLocation();
		const float CurrentDistanceToTarget = (CurrentLocation - TargetLocation).Size();
		if (CurrentDistanceToTarget < 1.f)
		{
			bIsMoving = false;
		}

		const FVector NewLocation = FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaTime, MovementSpeed);
		const float NewDistanceToTarget = (NewLocation - TargetLocation).Size();

		// Correct for overshooting target
		NewDistanceToTarget < CurrentDistanceToTarget ? SetActorLocation(NewLocation) : SetActorLocation(TargetLocation);
	}
}

void AGemBase::SetData(UGemDataAsset* GemData)
{
	if (!GemData) 
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid GemDataAsset pointer!"));
	}

	StaticMesh->SetStaticMesh(GemData->Mesh);
	StaticMesh->SetMaterial(0, GemData->Material);
	Type = GemData->Type; 
}

void AGemBase::MoveTo(const FVector& InTargetLocation)
{
	bIsMoving = true;
	TargetLocation = InTargetLocation;
}

void AGemBase::SetSelected(bool bInSelected)
{
	bIsSelected = bInSelected;
	bIsSelected ? SpinnerComponent->Start() : SpinnerComponent->Stop();
}



