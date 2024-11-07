// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionIndicator.generated.h"

UCLASS()
class MATCHTHREE_API ASelectionIndicator : public AActor
{
	GENERATED_BODY()

	//~ Begin AActor interface
protected:
	virtual void BeginPlay() override;
	//~ End AActor interface

public:
	ASelectionIndicator();

	void Show(bool bShouldShow);
};
