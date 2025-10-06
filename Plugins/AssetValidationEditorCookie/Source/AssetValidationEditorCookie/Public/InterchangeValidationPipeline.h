#pragma once

#include "CoreMinimal.h"
#include "InterchangePipelineBase.h"
#include "InterchangeValidationPipeline.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class ASSETVALIDATIONEDITORCOOKIE_API UInterchangeValidationPipeline : public UInterchangePipelineBase
{
	GENERATED_BODY()

public:
	// Override de la méthode ExecutePipeline avec la bonne signature UE 5.6
	virtual void ExecutePipeline(
		UInterchangeBaseNodeContainer* BaseNodeContainer,
		const TArray<UInterchangeSourceData*>& SourceDatas,
		const FString& ContentBasePath
	) override;
};
