// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GemBase.generated.h"

class UGemDataAsset;

/*
*	Base class for gems on the game board
*/
UCLASS()
class MATCHTHREE_API AGemBase : public AActor
{
	GENERATED_BODY()
	
	//~ Begin AActor interface
public:
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	//~ End AActor interface

public:	
	AGemBase();

	// Set the data of this gem actor
	void SetData(UGemDataAsset* GemData);

	// Move the gem to the given world location
	void MoveTo(const FVector& Destination);

	// Set this gem as selected
	void SetSelected(bool bInSelected);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gem Properties")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	// True if this actor is moving
	bool bIsMoving;

	// Location to move to
	FVector TargetLocation;

	// Movement speed
	UPROPERTY(EditDefaultsOnly, Category = "Gem Properties")
	float MovementSpeed = 2000.f;

	// Selected rotation speed
	UPROPERTY(EditDefaultsOnly, Category = "Gem Properties")
	float RotationSpeed = 180.f;

	bool bIsSelected;
};
