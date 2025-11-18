
#pragma once

#include "CoreMinimal.h"
#include "InterchangePipelineBase.h"
#include "InterchangeSourceData.h"
#include "InterchangeValidationPipeline.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class ASSETVALIDATIONEDITORCOOKIE_API UInterchangeValidationPipeline : public UInterchangePipelineBase
{
	GENERATED_BODY()

public:
	virtual void ExecutePipeline(
	   UInterchangeBaseNodeContainer* BaseNodeContainer,
	   const TArray<UInterchangeSourceData*>& SourceDatas,
	   const FString& ContentBasePath
	) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PolygonLimit = 10000;

};
