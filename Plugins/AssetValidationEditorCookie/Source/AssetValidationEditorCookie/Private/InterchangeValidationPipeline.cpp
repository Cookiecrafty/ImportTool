#include "InterchangeValidationPipeline.h"

void UInterchangeValidationPipeline::ExecutePipeline(
    UInterchangeBaseNodeContainer* BaseNodeContainer,
    const TArray<UInterchangeSourceData*>& SourceDatas,
    const FString& ContentBasePath
)
{
    UE_LOG(LogTemp, Warning, TEXT("Pipeline de validation Interchange appelé !"));
    
    // Version simplifiée - on valide juste les noms des fichiers sources
    for (UInterchangeSourceData* SourceData : SourceDatas)
    {
        if (!SourceData)
            continue;
            
        FString Filename = SourceData->GetFilename();
        FString BaseName = FPaths::GetBaseFilename(Filename);
        
        UE_LOG(LogTemp, Display, TEXT("Validation de l'import : %s"), *BaseName);
        
        // Validation pour Static Meshes (FBX, OBJ, etc.)
        FString Extension = FPaths::GetExtension(Filename).ToLower();
        if (Extension == TEXT("fbx") || Extension == TEXT("obj"))
        {
            if (!BaseName.StartsWith(TEXT("SM_")))
            {
                UE_LOG(LogTemp, Error, TEXT("Import refusé : Mesh '%s' doit commencer par 'SM_'."), *BaseName);
                // Note: Dans cette approche, on ne peut pas empêcher l'import, seulement logger
            }
            else
            {
                UE_LOG(LogTemp, Display, TEXT("Mesh '%s' : Nomenclature OK"), *BaseName);
            }
        }
        
        // Validation pour Textures
        if (Extension == TEXT("png") || Extension == TEXT("jpg") || Extension == TEXT("tga"))
        {
            if (!BaseName.StartsWith(TEXT("T_")))
            {
                UE_LOG(LogTemp, Error, TEXT("Import refusé : Texture '%s' doit commencer par 'T_'."), *BaseName);
            }
            else
            {
                UE_LOG(LogTemp, Display, TEXT("Texture '%s' : Nomenclature OK"), *BaseName);
            }
        }
    }
}
