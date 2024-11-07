// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GemDataAsset.generated.h"

/**
 * Data for the each gem
 */
UCLASS()
class MATCHTHREE_API UGemDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Gem Data")
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, Category = "Gem Data")
	TObjectPtr<UMaterialInstance> Material;
};
