// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TaskBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskCompleteSignature);

/**
 * 
 */
UCLASS()
class MATCHTHREE_API UTaskBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Execute() {};

	bool IsComplete() const { return bIsComplete; }

protected:
	bool bIsComplete = false;
};
