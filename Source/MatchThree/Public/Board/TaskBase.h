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
	UFUNCTION()
	virtual void Execute() {};

	FOnTaskCompleteSignature OnTaskComplete;

	bool IsComplete() const { return bIsComplete; }

	UFUNCTION()
	void Complete();

protected:
	bool bIsComplete = false;
};
