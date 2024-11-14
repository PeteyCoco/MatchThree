// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameBoard.h"
#include "MatchThreeGameMode.generated.h"

class AGameBoard;
class AGemBase;

/**
 * 
 */
UCLASS()
class MATCHTHREE_API AMatchThreeGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	//~ Begin AGameModeBase interface
protected:
	virtual void BeginPlay() override;
	virtual void StartPlay() override;
	//~ End AGameModeBase interface

protected:
	UPROPERTY()
	TObjectPtr<AGameBoard> GameBoard;

	UFUNCTION()
	void HandleMatchesFound(TArray<FMatch>& Matches);
};
