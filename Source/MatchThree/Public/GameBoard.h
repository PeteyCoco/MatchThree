// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TimerHandle.h"
#include "GameBoard.generated.h"

class AGemBase;
class UGemDataAsset;
class UInternalBoard;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBoardCascadeCompleteSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRowCascadeCompleteSignature, int, Row);



UENUM()
enum class EGemType : uint8
{
	Capsule,
	Cone,
	Icosphere,
	Sphere,
	Square,
	Triangle,
	Torus,

	MAX
};

/* The gems involved in a swap action */
USTRUCT()
struct FSwapPair
{
	GENERATED_BODY()

	AGemBase* FirstGem;
	AGemBase* SecondGem;
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

	// Get the world location of the given board location
	UFUNCTION(BlueprintCallable, Category = "Game Board")
	FVector GetWorldLocation(const FBoardLocation& InLocation) const;

	// Get the number of gems at the bottom of the column
	int32 GetColumnHeight(int32 Column) const;

	// Return true if the given gems can be swapped
	bool CanSwapGems(AGemBase* GemA, AGemBase* GemB) const;

	// Swap the gems' position on the board
	void SwapGems(AGemBase* GemA, AGemBase* GemB);

	// Move the board gems to their board positions
	void CascadeBoard();

	// Move the board gems in a given row to their positions
	void CascadeRow(int Row);

	// Delegate that broadcasts when the board has finished cascading
	UPROPERTY(BlueprintAssignable)
	FBoardCascadeCompleteSignature BoardCascadeCompleteDelegate;

	// Delegate that broadcasts whenever a row has finished cascading
	UPROPERTY(BlueprintAssignable)
	FRowCascadeCompleteSignature RowCascadeCompleteDelegate;

	// Spawn gems at the top of the board to fill in empty space
	void FillBoard();

	// Return true if the gems is near its board positions and not moving
	bool IsInPosition(AGemBase* InGem) const;

	// Return true if the gems in the given row are near their board positions
	bool IsRowInPosition(int Row) const;

	// Return an array of gems that form a match with the gem at the given location
	void GetMatches(AGemBase* InGem, TArray<AGemBase*>& OutMatch) const;

	// Logic to execute when a gem has finished a MoveTo
	UFUNCTION()
	void HandleGemMoveToComplete(AGemBase* InGem);

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

	UPROPERTY(VisibleInstanceOnly, Category = "Internal Board")
	UInternalBoard* InternalBoard;

	void InitializeInternalBoard();

	void DestroyGem(AGemBase* Gem);

	FTimerHandle CascadeTimer;

	UPROPERTY(EditAnywhere, Category = "Board Properties")
	float CascadeRate = 1.f;

	int CascadeCurrentRow = 0;
	UFUNCTION()
	void CascadeTimerCallback();

	UFUNCTION()
	void CheckCascadeRowComplete(AGemBase* InGem);

};
