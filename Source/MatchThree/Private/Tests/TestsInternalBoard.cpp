// Copyright Peter Carsten Collins (2024)


#include "Tests/TestsInternalBoard.h"

#include "Board/InternalBoard.h"
#include "GemBase.h"
#include "CoreTypes.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatchThreeTest, "MatchThree.InternalBoard.Collapse", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FMatchThreeTest::RunTest(const FString& Parameters)
{
	AGemBase* Gem00 = NewObject<AGemBase>();

	// Collapsing an already collapsed board does nothing
	{
		UInternalBoard* InternalBoard = NewObject<UInternalBoard>();
		InternalBoard->Initialize(8, 8);
		InternalBoard->SetGem(Gem00, { 0,0 });

		InternalBoard->Collapse();

		TestTrue(TEXT("Gem00 is still at location (0,0)"), InternalBoard->GetGem({0,0}) == Gem00);
	}
	return true;
}