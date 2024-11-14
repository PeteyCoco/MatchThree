// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Match.generated.h"

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
 * A struct representing a set of gems in a match
 */
USTRUCT()
struct FMatch
{
	GENERATED_BODY()

	// Add a board location to the match
	void AddLocation(const FBoardLocation& BoardLocation);
	void AddLocations(const TArray<FBoardLocation>& BoardLocations);

	// Get all of the board locations with this column number
	TArray<FBoardLocation> GetLocationsInColumn(int32 Column) const;

	TMap<int32, TArray<int32>> GemLocations;
	
};
