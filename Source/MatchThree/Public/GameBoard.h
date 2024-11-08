// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameBoard.generated.h"

class AGemBase;
class UGemDataAsset;


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
};

/*
* Base class for the game board in a match three game
*/
UCLASS(BlueprintType)
class MATCHTHREE_API AGameBoard : public AActor
{
	GENERATED_BODY()
	
	//~ Begin AActor interface
public:
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	void InitializeInternalBoard();
	//~ End AActor interface

public:
	AGameBoard();
	
	UFUNCTION(BlueprintCallable, Category = "Game Board")
	void ResetBoard();

	UFUNCTION(BlueprintCallable, Category = "Game Board")
	void SpawnGem(int32 Column, EGemType GemType);

	// Get the gem at the given board location
	UFUNCTION(BlueprintCallable, Category = "Game Board")
	AGemBase* GetGem(const FBoardLocation& InLocation) const;

	// Get the board location of the given gem
	UFUNCTION(BlueprintCallable, Category = "Game Board")
	FBoardLocation GetBoardLocation(AGemBase* Gem) const;

	// Get the world location of the given board location
	UFUNCTION(BlueprintCallable, Category = "Game Board")
	FVector GetWorldLocation(const FBoardLocation& InLocation) const;

	// Get the number of gems at the bottom of the column
	int32 GetColumnHeight(int32 Column) const;

	// Return true if the given gems can be swapped
	bool CanSwapGems(AGemBase* GemA, AGemBase* GemB) const;

	// Swap the gems' position on the board
	void SwapGems(AGemBase* GemA, AGemBase* GemB);

	// Append reference to all neighbouring gems matching the given gem's type to the given array
	void GetMatches(AGemBase* Gem, TArray<AGemBase*>& OutArray);

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

	/* Internal gem locations on the board*/
	TArray<TArray<AGemBase*>> Gems;

	UFUNCTION()
	void HandleSwapComplete();
};
