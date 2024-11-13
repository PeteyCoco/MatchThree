// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TimerHandle.h"
#include "GemBase.h"
#include "Board/BoardColumn.h"
#include "GameBoard.generated.h"

class AGemBase;
class UGemDataAsset;
class UInternalBoard;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBoardCascadeCompleteSignature);

/* The gems involved in a swap action */
USTRUCT()
struct FSwapPair
{
	GENERATED_BODY()

	AGemBase* FirstGem;
	AGemBase* SecondGem;
};

/*
* Struct representing a coordinate on the board
*/
USTRUCT(Blueprintable)
struct FBoardLocation
{
	GENERATED_BODY()

	FBoardLocation()
	{
		X = 0;
		Y = 0;
	}
	FBoardLocation(int InX, int InY)
	{
		X = InX;
		Y = InY;
	}

	UPROPERTY()
	int32 X = 0;

	UPROPERTY()
	int32 Y = 0;

	friend bool operator==(const FBoardLocation& A, const FBoardLocation& B);
};



/*
* Base class for the game board in a match three game
*/
UCLASS(BlueprintType)
class MATCHTHREE_API AGameBoard : public AActor
{
	GENERATED_BODY()
	
	//~ Begin AActor interface
protected:
	virtual void BeginPlay() override;
	//~ End AActor interface

public:
	AGameBoard();
	
	UFUNCTION(BlueprintCallable, Category = "Game Board")
	void ResetBoard();

	UFUNCTION(BlueprintCallable, Category = "Game Board")
	AGemBase* SpawnGem(int32 Column, EGemType GemType);

	// Get the gem at the given board location
	UFUNCTION(BlueprintCallable, Category = "Game Board")
	AGemBase* GetGem(const FBoardLocation& InLocation) const;

	// Set the gem at to the given location
	void SetGem(AGemBase* Gem, const FBoardLocation& BoardLocation);

	// Returns true if the gem is on the board
	bool ContainsGem(AGemBase* InGem) const;

	// Get the world location of the given board location
	UFUNCTION(BlueprintCallable, Category = "Game Board")
	FVector GetWorldLocation(const FBoardLocation& InLocation) const;

	// Get the number of gems int the column
	int32 NumberOfGems(int32 Column) const;

	int32 GetBoardWidth() const;
	int32 GetBoardHeight() const;

	// Return true if the given gems can be swapped
	bool CanSwapGems(AGemBase* GemA, AGemBase* GemB) const;

	// Swap the gems' position on the board
	void SwapGems(AGemBase* GemA, AGemBase* GemB);

	// Move the board gems to their board positions
	void CascadeBoard();

	// Move column of gems into position
	void CascadeColumn(int Column);

	void MoveIntoPosition(const FBoardLocation& BoardLocation);

	void MoveGemToBoardLocation(AGemBase* Gem, const FBoardLocation& NewBoardLocation);

	// Delegate that broadcasts when the board has finished cascading
	UPROPERTY(BlueprintAssignable)
	FBoardCascadeCompleteSignature BoardCascadeCompleteDelegate;

	// Spawn gems at the top of the board to fill in empty space
	void FillBoard();

	// Return true if the gems is near its board positions and not moving
	bool IsInPosition(AGemBase* InGem) const;

	bool IsEmpty(const FBoardLocation& InLocation) const;

	// Return an array of gems that form a match with the gem at the given location
	void GetMatches(AGemBase* InGem, TArray<AGemBase*>& OutMatch) const;

	// Logic to execute when a gem has finished a MoveTo
	UFUNCTION()
	void HandleGemMoveToComplete(AGemBase* InGem);

	// Get the board location above the next lowest gem
	FBoardLocation GetNextEmptyLocationBelow(const FBoardLocation& InLocation) const;

	// Get the empty location at the top of the column
	FBoardLocation GetTopEmptyLocation(int32 Column) const;

	EGemType GetRandomGemType() const;

	void QueueGemToSpawn(int32 Column);
	EGemType DequeueGemToSpawn(int32 Column);

	// Attempt to move the gem at the given location downward
	void MoveGemDown(const FBoardLocation& InLocation);

	// Spawn a gem into a column and move it down
	void SpawnGemInColumn(int32 Column);

	FBoardLocation GetBoardLocation(const AGemBase* Gem) const;

	// Remove the given gem from the board
	void Remove(AGemBase* InGem);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	int32 BoardWidth = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	int32 BoardHeight = 8;

	// Spacing between cell centers on the game board
	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	float CellSpacing = 200.f;

	// Distance above the board to spawn gems (in units of CellSpacing)
	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	float DistanceSpawnAboveBoard = 5.f;

	// Mapping from gem types to their blueprint actors
	UPROPERTY(EditAnywhere, Category = "Gem Properties")
	TSubclassOf<AGemBase> GemActorClass;

	// Mapping from gem types to their data assets
	UPROPERTY(EditAnywhere, Category = "Gem Properties")
	TMap<EGemType, UGemDataAsset*> GemData;

	// Scale of the gems
	UPROPERTY(EditAnywhere, Category = "Gem Properties")
	float GemScale = 0.9f;

	FSwapPair CurrentSwap;

	void DestroyGem(AGemBase* Gem);

	UPROPERTY(EditAnywhere, Category = "Board Properties")
	float CascadeRate = 1.f;

	TArray<struct FBoardColumn> Columns;
};
