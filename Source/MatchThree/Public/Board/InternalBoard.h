// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InternalBoard.generated.h"

class AGemBase;

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

/**
 * 
 */
UCLASS()
class MATCHTHREE_API UInternalBoard : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(int32 InBoardWidth, int32 InBoardHeight);

	AGemBase* GetGem(const FBoardLocation& BoardLocation) const;
	AGemBase* GetGem(const FBoardLocation& BoardLocation);

	void SetGem(AGemBase* Gem, const FBoardLocation& BoardLocation);

	FBoardLocation GetBoardLocation(const AGemBase* Gem) const;

	// Return true if the gems are next to eachother
	bool AreNeighbours(AGemBase* GemA, AGemBase* GemB) const;

	// Move gems to their lowest positions in each column
	void Collapse();

	// Return true if the given location is empty
	bool IsEmpty(const FBoardLocation& BoardLocation) const;

	// Get references to all board gems
	void GetAllGems(TArray<AGemBase*>& OutGems) const;

	// Get references to all gems in a row
	void GetGemsInRow(int Row, TArray<AGemBase*>& OutGems) const;

	// Get the number of empty spaces at the top of the column
	int SpacesAtTop(int Column) const;

	// Get the board location above the next lowest gem
	FBoardLocation GetNextEmptyLocationBelow(AGemBase* Gem) const;

	// Add a gem to the top of the column. Returns false if failed
	bool AddGemToTopOfColumn(int Column, AGemBase* Gem);

	// Remove the given gem from the board
	void Remove(AGemBase* InGem);
	 
	int32 GetBoardWidth() { return BoardWidth; }
	int32 GetBoardHeight() { return BoardHeight; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	int32 BoardWidth = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	int32 BoardHeight = 8;
	
	TArray<TArray<AGemBase*>> BoardData;
};
