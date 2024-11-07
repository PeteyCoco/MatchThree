// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MatchThreePawn.generated.h"

UCLASS()
class MATCHTHREE_API AMatchThreePawn : public APawn
{
	GENERATED_BODY()

	//~ Begin APawn interface
protected:
	virtual void BeginPlay() override;
public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//~ End APawn interface

public:
	AMatchThreePawn();

};
