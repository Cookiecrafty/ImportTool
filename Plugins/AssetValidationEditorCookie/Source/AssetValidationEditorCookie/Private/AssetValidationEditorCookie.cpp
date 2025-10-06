#include "AssetValidationEditorCookie.h"
#include "ImportValidationPipeline.h"
#include "Editor.h"
#include "EditorSubsystem.h"

IMPLEMENT_MODULE(FAssetValidationEditorCookieModule, AssetValidationEditorCookie)

void FAssetValidationEditorCookieModule::StartupModule()
{
	if (GEditor)
	{
		UImportSubsystem* ImportSubsystem = GEditor->GetEditorSubsystem<UImportSubsystem>();
		if (ImportSubsystem)
		{
			ImportSubsystem->OnAssetPostImport.AddRaw(this, &FAssetValidationEditorCookieModule::OnAssetPostImport);
		}
	}
}

void FAssetValidationEditorCookieModule::ShutdownModule()
{
	if (GEditor)
	{
		UImportSubsystem* ImportSubsystem = GEditor->GetEditorSubsystem<UImportSubsystem>();
		if (ImportSubsystem)
		{
			ImportSubsystem->OnAssetPostImport.RemoveAll(this);
		}
	}
}

void FAssetValidationEditorCookieModule::OnAssetPostImport(UFactory* InFactory, UObject* InCreatedObject)
{
	if (!InCreatedObject)
		return;

	// Créer et appeler ton pipeline de validation
	UImportValidationPipeline* Validator = NewObject<UImportValidationPipeline>();

	TArray<UObject*> AssetsToValidate;
	AssetsToValidate.Add(InCreatedObject);

	Validator->ExecuteValidationPipeline(AssetsToValidate);
}
