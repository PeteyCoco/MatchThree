// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameBoard.generated.h"

class AGemBase;
class UGemDataAsset;


UENUM(BlueprintType)
enum class EGemType : uint8
{
	Square
};

/*
* Base class for the game board in a match three game
*/
UCLASS()
class MATCHTHREE_API AGameBoard : public AActor
{
	GENERATED_BODY()
	
	//~ Begin AActor interface
public:
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	//~ End AActor interface

public:
	AGameBoard();

	UFUNCTION(BlueprintCallable, Category = "Game Board")
	void PlaceGem();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	int32 BoardWidth = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	int32 BoardHeight = 8;

	// Spacing between cell centers on the game board
	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	float CellSpacing = 200.f;

	// Mapping from gem types to their blueprint actors
	UPROPERTY(EditAnywhere, Category = "Board Properties")
	TMap<EGemType, TSubclassOf<AGemBase>> GemActorClasses;

	// Mapping from gem types to their data assets
	UPROPERTY(EditAnywhere, Category = "Board Properties")
	TMap<EGemType, UGemDataAsset*> GemData;
};
