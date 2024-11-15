// Copyright Peter Carsten Collins (2024)


#include "GemBase.h"

#include "Components/SpinnerComponent.h"
#include "Components/GemMovementComponent.h"
#include "Gem/GemDataAsset.h"
#include "MatchThree/MatchThree.h"

AGemBase::AGemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(StaticMesh);

	StaticMesh->SetCollisionResponseToChannel(ECC_Gem, ECR_Block);

	SpinnerComponent = CreateDefaultSubobject<USpinnerComponent>("SpinnerComponent");

	MovementComponent = CreateDefaultSubobject<UGemMovementComponent>("MovementComponent");

	bIsSelected = false;
}

void AGemBase::BeginPlay()
{
	Super::BeginPlay();
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

void AGemBase::MoveTo(const FVector& NewLocation)
{
	MovementComponent->OnMoveToCompleteDelegate.AddUniqueDynamic(this, &AGemBase::HandleMoveToComplete);
	MovementComponent->MoveTo(NewLocation);
}

void AGemBase::SetSelected(bool bInSelected)
{
	bIsSelected = bInSelected;
	bIsSelected ? SpinnerComponent->Start() : SpinnerComponent->Stop();
}

void AGemBase::HandleMoveToComplete()
{
	OnGemMoveToCompleteDelegate.Broadcast(this);
}



