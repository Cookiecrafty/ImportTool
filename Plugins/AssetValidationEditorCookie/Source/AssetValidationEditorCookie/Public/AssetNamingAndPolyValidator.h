#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "AssetNamingAndPolyValidator.generated.h"

UCLASS()
class UAssetNamingAndPolyValidator : public UEditorValidatorBase
{
	GENERATED_BODY()

public:
	// Fonction qui dit si un asset peut être validé par ce validateur
	virtual bool CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const override;

	// Fonction de validation principale
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context) override;
};
