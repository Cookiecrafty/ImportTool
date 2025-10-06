#include "AssetNamingAndPolyValidator.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Logging/LogMacros.h"

// Log personnalisé pour le détail
DEFINE_LOG_CATEGORY_STATIC(LogAssetValidationDetail, Log, All);

bool UAssetNamingAndPolyValidator::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const
{
    // Valide Static Meshes et Textures 2D
    UClass* AssetClass = InAssetData.GetClass();
    return AssetClass == UStaticMesh::StaticClass() || AssetClass == UTexture2D::StaticClass();
}

EDataValidationResult UAssetNamingAndPolyValidator::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
    // Valeurs limites personnalisables
    const int32 MaxTriangles = 10000;
    const int32 MaxTextureSize = 2048;

    // ----- Pour Static Mesh -------
    if (UStaticMesh* Mesh = Cast<UStaticMesh>(InAsset))
    {
        FString AssetName = InAssetData.AssetName.ToString();
        int32 TotalTriangles = 0;
        if (Mesh->GetRenderData())
        {
            for (const FStaticMeshLODResources& LOD : Mesh->GetRenderData()->LODResources)
            {
                TotalTriangles += LOD.GetNumTriangles();
            }
        }

        // Log détaillé
        UE_LOG(LogAssetValidationDetail, Display, TEXT("StaticMesh '%s' : Triangles=%d (Limite=%d), Nomenclature attendue=SM_"),
            *AssetName, TotalTriangles, MaxTriangles);

        // Vérification nomenclature
        if (!AssetName.StartsWith(TEXT("SM_")))
        {
            AssetFails(InAsset, FText::Format(
                FText::FromString("ERREUR - StaticMesh : nom '{0}' n'est pas conforme ('SM_' attendu) - Triangles: {1} (max {2})"),
                FText::FromString(AssetName),
                FText::AsNumber(TotalTriangles),
                FText::AsNumber(MaxTriangles)
            ));
            return EDataValidationResult::Invalid;
        }

        // Vérification polygones
        if (TotalTriangles > MaxTriangles)
        {
            AssetFails(InAsset, FText::Format(
                FText::FromString("ERREUR - StaticMesh : '{0}' a trop de triangles : {1} (max {2})"),
                FText::FromString(AssetName),
                FText::AsNumber(TotalTriangles),
                FText::AsNumber(MaxTriangles)
            ));
            return EDataValidationResult::Invalid;
        }
        UE_LOG(LogAssetValidationDetail, Display, TEXT("OK - StaticMesh '%s' valide : Triangles=%d (max %d)"), *AssetName, TotalTriangles, MaxTriangles);
        AssetPasses(InAsset);
        return EDataValidationResult::Valid;
    }

    // ----- Pour Texture 2D -------
    if (UTexture2D* Texture = Cast<UTexture2D>(InAsset))
    {
        FString AssetName = InAssetData.AssetName.ToString();
        int32 Width = Texture->GetSizeX();
        int32 Height = Texture->GetSizeY();

        // Log détaillé
        UE_LOG(LogAssetValidationDetail, Display, TEXT("Texture2D '%s' : Taille=%dx%d (Limite=%dx%d), Nomenclature attendue=T_"),
            *AssetName, Width, Height, MaxTextureSize, MaxTextureSize);

        // Vérification nomenclature
        if (!AssetName.StartsWith(TEXT("T_")))
        {
            AssetFails(InAsset, FText::Format(
                FText::FromString("ERREUR - Texture2D : nom '{0}' n'est pas conforme ('T_' attendu) - Taille: {1}x{2} (max {3})"),
                FText::FromString(AssetName),
                FText::AsNumber(Width),
                FText::AsNumber(Height),
                FText::AsNumber(MaxTextureSize)
            ));
            return EDataValidationResult::Invalid;
        }

        // Vérification dimensions
        if (Width > MaxTextureSize || Height > MaxTextureSize)
        {
            AssetFails(InAsset, FText::Format(
                FText::FromString("ERREUR - Texture2D : '{0}' trop grande : {1}x{2} (max {3})"),
                FText::FromString(AssetName),
                FText::AsNumber(Width),
                FText::AsNumber(Height),
                FText::AsNumber(MaxTextureSize)
            ));
            return EDataValidationResult::Invalid;
        }

        UE_LOG(LogAssetValidationDetail, Display, TEXT("OK - Texture2D '%s' valide : %dx%d (max %d)"), *AssetName, Width, Height, MaxTextureSize);
        AssetPasses(InAsset);
        return EDataValidationResult::Valid;
    }

    return EDataValidationResult::NotValidated;
}
