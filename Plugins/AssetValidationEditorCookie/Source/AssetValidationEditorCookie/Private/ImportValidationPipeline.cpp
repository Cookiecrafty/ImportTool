#include "ImportValidationPipeline.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"

void UImportValidationPipeline::ExecuteValidationPipeline(const TArray<UObject*>& CreatedAssets)
{
    const int32 MaxTriangles = 10000;
    const int32 MaxTextureSize = 2048;

    for (UObject* Asset : CreatedAssets)
    {
        if (!Asset) continue;

        // ---- Static Mesh ----
        if (UStaticMesh* Mesh = Cast<UStaticMesh>(Asset))
        {
            FString Name = Mesh->GetName();
            int32 TotalTriangles = 0;
            if (Mesh->GetRenderData())
            {
                for (const FStaticMeshLODResources& LOD : Mesh->GetRenderData()->LODResources)
                {
                    TotalTriangles += LOD.GetNumTriangles();
                }
            }
            if (!Name.StartsWith(TEXT("SM_")))
            {
                UE_LOG(LogTemp, Error, TEXT("Import refusé : StaticMesh '%s' doit commencer par 'SM_'."), *Name);
                Mesh->MarkAsGarbage();
                continue;
            }
            if (TotalTriangles > MaxTriangles)
            {
                UE_LOG(LogTemp, Error, TEXT("Import refusé : StaticMesh '%s' a trop de triangles (%d, max %d)."), *Name, TotalTriangles, MaxTriangles);
                Mesh->MarkAsGarbage();
                continue;
            }
            UE_LOG(LogTemp, Display, TEXT("StaticMesh '%s' importé OK: Triangles=%d (max %d)"), *Name, TotalTriangles, MaxTriangles);
        }

        // ---- Texture2D ----
        if (UTexture2D* Texture = Cast<UTexture2D>(Asset))
        {
            FString Name = Texture->GetName();
            int32 Width = Texture->GetSizeX();
            int32 Height = Texture->GetSizeY();
            if (!Name.StartsWith(TEXT("T_")))
            {
                UE_LOG(LogTemp, Error, TEXT("Import refusé : Texture2D '%s' doit commencer par 'T_'."), *Name);
                Texture->MarkAsGarbage();
                continue;
            }
            if (Width > MaxTextureSize || Height > MaxTextureSize)
            {
                UE_LOG(LogTemp, Error, TEXT("Import refusé : Texture2D '%s' trop grande (%dx%d, max %d)."), *Name, Width, Height, MaxTextureSize);
                Texture->MarkAsGarbage();
                continue;
            }
            UE_LOG(LogTemp, Display, TEXT("Texture2D '%s' importée OK: %dx%d (max %d)"), *Name, Width, Height, MaxTextureSize);
        }
    }
}
