// Copyright Peter Carsten Collins (2024)


#include "Tests/TestsInternalBoard.h"

#include "Board/InternalBoard.h"
#include "GemBase.h"
#include "CoreTypes.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatchThreeTest, "MatchThree.InternalBoard.Collapse", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMatchThreeTest::RunTest(const FString& Parameters)
{
	AGemBase* GemA = NewObject<AGemBase>();
	AGemBase* GemB = NewObject<AGemBase>();
	AGemBase* GemC = NewObject<AGemBase>();

	// Collapsing an already collapsed board does nothing
	{
		UInternalBoard* InternalBoard = NewObject<UInternalBoard>();
		InternalBoard->Initialize(8, 8);
		InternalBoard->SetGem(GemA, { 0,0 });

		InternalBoard->Collapse();

		TestTrue(TEXT("GemA is still at location (0,0)"), InternalBoard->GetGem({0,0}) == GemA);
	}

	// Collapsing a board with a single large gap
	{
		UInternalBoard* InternalBoard = NewObject<UInternalBoard>();
		InternalBoard->Initialize(8, 8);
		InternalBoard->SetGem(GemA, { 0,0 });
		InternalBoard->SetGem(GemB, { 0,3 });
		InternalBoard->SetGem(GemC, { 0,4 });

		InternalBoard->Collapse();

		TestTrue(TEXT("GemB has moved to location (0,1)"), InternalBoard->GetBoardLocation(GemB) == FBoardLocation(0, 1));
		TestTrue(TEXT("GemC has moved to location (0,2)"), InternalBoard->GetBoardLocation(GemC) == FBoardLocation(0, 2));
		TestTrue(TEXT("Location (0,3) is empty"), InternalBoard->IsEmpty(FBoardLocation(0, 3)));
	}

	{
		UInternalBoard* InternalBoard = NewObject<UInternalBoard>();
		InternalBoard->Initialize(8, 8);
		InternalBoard->SetGem(GemA, { 0,0 });
		InternalBoard->SetGem(GemB, { 2, 7 });

		TestEqual(TEXT("Column 0 has 7 spaces at the top"), InternalBoard->SpacesAtTop(0), 7);
		TestEqual(TEXT("Column 1 has 8 spaces at the top"), InternalBoard->SpacesAtTop(1), 8);
		TestEqual(TEXT("Column 2 has 0 spaces at the top"), InternalBoard->SpacesAtTop(2), 0);

		AGemBase* GemD = NewObject<AGemBase>();
		InternalBoard->AddGemToTopOfColumn(0, GemD);

		TestEqual(TEXT("Column 0 has 6 spaces at the top"), InternalBoard->SpacesAtTop(0), 6);
	}


	return true;
}