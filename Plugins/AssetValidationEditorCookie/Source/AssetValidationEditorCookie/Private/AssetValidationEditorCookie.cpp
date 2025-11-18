#include "AssetValidationEditorCookie.h"
#include "Editor.h"
#include "EditorSubsystem.h"

IMPLEMENT_MODULE(FAssetValidationEditorCookieModule, AssetValidationEditorCookie)

void FAssetValidationEditorCookieModule::StartupModule()
{
	if (GEditor)
	{
		UImportSubsystem* ImportSubsystem = GEditor->GetEditorSubsystem<UImportSubsystem>();

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


