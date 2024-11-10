// Copyright Peter Carsten Collins (2024)


#include "Tests/TestsInternalBoard.h"

#include "Board/InternalBoard.h"
#include "GemBase.h"
#include "CoreTypes.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatchThreeTest, "MatchThree.InternalBoard.Collapse", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FMatchThreeTest::RunTest(const FString& Parameters)
{
	AGemBase* Gem00 = NewObject<AGemBase>();
	AGemBase* Gem02 = NewObject<AGemBase>();

	// Collapsing an already collapsed board does nothing
	{
		UInternalBoard* InternalBoard = NewObject<UInternalBoard>();
		InternalBoard->Initialize(8, 8);
		InternalBoard->SetGem(Gem00, { 0,0 });

		InternalBoard->Collapse();

		TestTrue(TEXT("Gem00 is still at location (0,0)"), InternalBoard->GetGem({0,0}) == Gem00);
	}

	{
		UInternalBoard* InternalBoard = NewObject<UInternalBoard>();
		InternalBoard->Initialize(8, 8);
		InternalBoard->SetGem(Gem00, { 0,0 });
		InternalBoard->SetGem(Gem02, { 0,2 });

		InternalBoard->Collapse();

		TestTrue(TEXT("Gem02 has moved to location (0,1)"), InternalBoard->GetBoardLocation(Gem02) == FBoardLocation(0, 1));
	}
	return true;
}