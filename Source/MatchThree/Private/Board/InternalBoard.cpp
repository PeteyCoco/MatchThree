// Copyright Peter Carsten Collins (2024)


#include "Board/InternalBoard.h"
#include "GemBase.h"

void UInternalBoard::Initialize(int32 InBoardWidth, int32 InBoardHeight)
{
	BoardData.Empty();
	BoardData.SetNum(BoardWidth);
	for (int32 ColIndex = 0; ColIndex < BoardWidth; ++ColIndex)
	{
		BoardData[ColIndex].SetNum(BoardHeight);

		for (int32 RowIndex = 0; RowIndex < BoardHeight; ++RowIndex)
		{
			BoardData[ColIndex][RowIndex] = nullptr;
		}
	}
}

void UInternalBoard::SetGem(AGemBase* Gem, const FBoardLocation& BoardLocation)
{
	BoardData[BoardLocation.X][BoardLocation.Y] = Gem;
}

AGemBase* UInternalBoard::GetGem(const FBoardLocation& BoardLocation)
{
	return BoardData[BoardLocation.X][BoardLocation.Y];
}
