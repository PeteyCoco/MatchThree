// Copyright Peter Carsten Collins (2024)


#include "Board/Match.h"

void FMatch::AddLocation(const FBoardLocation& BoardLocation)
{
	GemLocations.AddUnique(BoardLocation);
}

void FMatch::AddLocations(const TArray<FBoardLocation>& BoardLocations)
{
	for (const FBoardLocation& Location : BoardLocations)
	{
		GemLocations.AddUnique(Location);
	}
}

TArray<FBoardLocation> FMatch::GetLocationsInColumn(int32 Column) const
{
	TArray<FBoardLocation> OutLocations;
	for (const FBoardLocation& Location : GemLocations)
	{
		if (Location.X == Column)
		{
			OutLocations.Add(Location);
		}
	}
	return OutLocations;
}
