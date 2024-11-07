// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GemBase.generated.h"

/*
*	Base class for gems on the game board
*/
UCLASS()
class MATCHTHREE_API AGemBase : public AActor
{
	GENERATED_BODY()
	
	//~ Begin AActor interface
protected:
	virtual void BeginPlay() override;
	//~ End AActor interface

public:	
	AGemBase();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gem Properties")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
};
