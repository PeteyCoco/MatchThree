// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MatchThreePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MATCHTHREE_API AMatchThreePlayerController : public APlayerController
{
	GENERATED_BODY()
	
	//~ Begin APlayerController interface
public:
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	//~ End APlayerController interface

public:
	AMatchThreePlayerController();
};
