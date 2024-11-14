// Copyright Peter Carsten Collins (2024)


#include "Board/Match.h"

void FMatch::AddLocation(const FBoardLocation& BoardLocation)
{
	TArray<int32>& ColumnValues = GemLocations.FindOrAdd(BoardLocation.X);
	ColumnValues.Add(BoardLocation.Y);
}

void FMatch::AddLocations(const TArray<FBoardLocation>& BoardLocations)
{
	for (const FBoardLocation& BoardLocation : BoardLocations)
	{
		AddLocation(BoardLocation);
	}
}

TArray<FBoardLocation> FMatch::GetLocationsInColumn(int32 Column) const
{
	const TArray<int32>* RowsPtr = GemLocations.Find(Column);
	TArray<FBoardLocation> BoardLocations;
	if (RowsPtr)
	{
		TArray<int32> Rows = *RowsPtr;
		for (int32 Row : Rows)
		{
			BoardLocations.Add({ Column, Row });
		}
	}
	return BoardLocations;
}
