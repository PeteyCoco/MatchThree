// Copyright Peter Carsten Collins (2024)


#include "GemBase.h"

#include "Gem/GemDataAsset.h"

AGemBase::AGemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(GetRootComponent());
}

void AGemBase::SetData(UGemDataAsset* GemData)
{
	if (!GemData) 
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid GemData pointer!"));
	}

	StaticMesh->SetStaticMesh(GemData->Mesh);
	StaticMesh->SetMaterial(0, GemData->Material);
}

void AGemBase::BeginPlay()
{
	Super::BeginPlay();
}

