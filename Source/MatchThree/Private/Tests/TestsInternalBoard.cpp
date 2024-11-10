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
	AGemBase* Gem03 = NewObject<AGemBase>();
	AGemBase* Gem04 = NewObject<AGemBase>();

	// Collapsing an already collapsed board does nothing
	{
		UInternalBoard* InternalBoard = NewObject<UInternalBoard>();
		InternalBoard->Initialize(8, 8);
		InternalBoard->SetGem(Gem00, { 0,0 });

		InternalBoard->Collapse();

		TestTrue(TEXT("Gem00 is still at location (0,0)"), InternalBoard->GetGem({0,0}) == Gem00);
	}

	// Collapsing a board with a single large gap
	{
		UInternalBoard* InternalBoard = NewObject<UInternalBoard>();
		InternalBoard->Initialize(8, 8);
		InternalBoard->SetGem(Gem00, { 0,0 });
		InternalBoard->SetGem(Gem03, { 0,3 });
		InternalBoard->SetGem(Gem04, { 0,4 });

		InternalBoard->Collapse();

		TestTrue(TEXT("Gem03 has moved to location (0,1)"), InternalBoard->GetBoardLocation(Gem03) == FBoardLocation(0, 1));
		TestTrue(TEXT("Gem04 has moved to location (0,2)"), InternalBoard->GetBoardLocation(Gem04) == FBoardLocation(0, 2));
		TestTrue(TEXT("Location (0,3) is empty"), InternalBoard->IsEmpty(FBoardLocation(0, 3)));
	}
	return true;
}