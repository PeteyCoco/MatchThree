// Copyright Peter Carsten Collins (2024)


#include "GemBase.h"

AGemBase::AGemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(GetRootComponent());
}

void AGemBase::BeginPlay()
{
	Super::BeginPlay();
}

