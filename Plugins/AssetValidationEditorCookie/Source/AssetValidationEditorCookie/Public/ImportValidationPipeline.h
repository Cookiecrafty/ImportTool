#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ImportValidationPipeline.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class UImportValidationPipeline : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void ExecuteValidationPipeline(const TArray<UObject*>& CreatedAssets);
};
