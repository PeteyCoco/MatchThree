// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameBoard.h"
#include "MatchThreeGameMode.generated.h"

class AGameBoard;
class AGemBase;
class AScoreActor;
class UTaskPool;

/* The locations involved in a swap action */
USTRUCT()
struct FSwapPair
{
	GENERATED_BODY()

	FBoardLocation LocationA;
	FBoardLocation LocationB;
};

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

public:
	void SwapGems(AGemBase* GemA, AGemBase* GemB);

	bool CanSwapGems(AGemBase* GemA, AGemBase* GemB);

protected:
	UPROPERTY()
	TObjectPtr<AGameBoard> GameBoard;

	// Actor to spawn when a match is scored
	UPROPERTY(EditAnywhere)
	TSubclassOf<AScoreActor> ScoreActorClass;

	// Method to execute after matches are found
	UFUNCTION()
	void HandleMatchesFound(TArray<FMatch>& Matches);

	// Method to execute after a swap action is completed
	UFUNCTION()
	void HandleCompletedSwapAction();

	// Method to execute after a swap action is undone (we still check for matches)
	UFUNCTION()
	void HandleUndoneSwapAction();

	// Clear the current swap action
	UFUNCTION()
	void ClearCurrentSwapAction();

	// Task pool for overseeing ongoing tasks
	UPROPERTY()
	UTaskPool* TaskPool;

	// The currently swapping locations
	TSharedPtr<FSwapPair> CurrentSwapAction;
};
