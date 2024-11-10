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

	void SetGem(AGemBase* Gem, const FBoardLocation& BoardLocation);

	AGemBase* GetGem(const FBoardLocation& BoardLocation);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	int32 BoardWidth = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Board Properties")
	int32 BoardHeight = 8;
	
	TArray<TArray<AGemBase*>> BoardData;
};
