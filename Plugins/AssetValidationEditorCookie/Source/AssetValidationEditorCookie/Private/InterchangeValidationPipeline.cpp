#include "InterchangeValidationPipeline.h"
#include "Nodes/InterchangeBaseNodeContainer.h"
#include "InterchangeMeshFactoryNode.h"
#include "InterchangeMeshNode.h"
#include "InterchangeStaticMeshLodDataNode.h"
#include "InterchangeTextureFactoryNode.h"
#include "InterchangeMaterialFactoryNode.h"
#include "InterchangeSkeletonFactoryNode.h"
#include "InterchangePhysicsAssetFactoryNode.h"
#include "Nodes/InterchangeBaseNode.h"
#include "Misc/Paths.h"

void UInterchangeValidationPipeline::ExecutePipeline(
    UInterchangeBaseNodeContainer* BaseNodeContainer,
    const TArray<UInterchangeSourceData*>& SourceDatas,
    const FString& ContentBasePath)
{
    if (!BaseNodeContainer)
    {
        UE_LOG(LogTemp, Error, TEXT("BaseNodeContainer est nul dans ExecutePipeline"));
        return;
    }

    Super::ExecutePipeline(BaseNodeContainer, SourceDatas, ContentBasePath);
    UE_LOG(LogTemp, Warning, TEXT("ExecutePipeline: début de validation"));

    bool bCancelImportEntire = false;
    const int32 MaxPolygons = PolygonLimit;
    TArray<FString> NodesToRemove;

    // Static Meshes
    BaseNodeContainer->IterateNodesOfType<UInterchangeMeshFactoryNode>(
        [&](const FString& NodeID, UInterchangeMeshFactoryNode* FactoryNode)
    {
        if (!FactoryNode)
        {
            UE_LOG(LogTemp, Warning, TEXT("Node mesh nul détecté pour %s"), *NodeID);
            return;
        }

        FString AssetName = FactoryNode->GetDisplayLabel();

        // Renommage automatique du nom si non conforme
        if (!AssetName.StartsWith(TEXT("SM_")))
        {
            FString NewAssetName = FString(TEXT("SM_")) + AssetName;
            FactoryNode->SetDisplayLabel(NewAssetName);
            UE_LOG(LogTemp, Warning, TEXT(" StaticMesh '%s' renommé auto en '%s'"), *AssetName, *NewAssetName);
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("✓ StaticMesh '%s' nom valide"), *AssetName);
        }

        // LOD
        TArray<FString> LodDataUniqueIds;
        FactoryNode->GetLodDataUniqueIds(LodDataUniqueIds);

        if (LodDataUniqueIds.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT(" Aucun LOD Data trouvé pour %s"), *AssetName);
            return;
        }

        FString LodNodeID = LodDataUniqueIds[0];
        auto LodDataNode = Cast<UInterchangeStaticMeshLodDataNode>(
            const_cast<UInterchangeBaseNode*>(BaseNodeContainer->GetNode(LodNodeID))
        );

        if (!LodDataNode)
        {
            UE_LOG(LogTemp, Warning, TEXT(" LodDataNode nul pour %s"), *LodNodeID);
            return;
        }

        TArray<FString> MeshUids;
        LodDataNode->GetMeshUids(MeshUids);

        if (MeshUids.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT(" Aucun mesh trouvé dans le LOD pour %s"), *AssetName);
            return;
        }

        int32 TotalPolygons = 0;
        for (const FString& MeshUid : MeshUids)
        {
            auto BaseNode = const_cast<UInterchangeBaseNode*>(BaseNodeContainer->GetNode(MeshUid));
            if (BaseNode)
            {
                auto MeshNode = Cast<UInterchangeMeshNode>(BaseNode);
                if (MeshNode)
                {
                    int32 PolygonCount = 0;
                    if (MeshNode->GetCustomPolygonCount(PolygonCount))
                        TotalPolygons += PolygonCount;
                }
                else
                {
                    FString AssetInstanceUid;
                    if (BaseNode->GetStringAttribute(TEXT("AssetInstanceUid"), AssetInstanceUid))
                    {
                        auto ActualMeshNode = Cast<UInterchangeMeshNode>(
                            const_cast<UInterchangeBaseNode*>(BaseNodeContainer->GetNode(AssetInstanceUid))
                        );
                        if (ActualMeshNode)
                        {
                            int32 PolygonCount = 0;
                            if (ActualMeshNode->GetCustomPolygonCount(PolygonCount))
                                TotalPolygons += PolygonCount;
                        }
                    }
                }
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("========== POLYGONE COUNT =========="));
        UE_LOG(LogTemp, Warning, TEXT("Asset: '%s'"), *FactoryNode->GetDisplayLabel());
        UE_LOG(LogTemp, Warning, TEXT("Total Polygones : %d"), TotalPolygons);
        UE_LOG(LogTemp, Warning, TEXT("Limite : %d"), MaxPolygons);
        UE_LOG(LogTemp, Warning, TEXT("====================================="));

        if (TotalPolygons > MaxPolygons)
        {
            NodesToRemove.Add(NodeID);
            bCancelImportEntire = true;
            UE_LOG(LogTemp, Error, TEXT(" StaticMesh '%s' trop lourd : %d polygones > max %d"), *FactoryNode->GetDisplayLabel(), TotalPolygons, MaxPolygons);
        }
        else if (TotalPolygons > 0)
        {
            UE_LOG(LogTemp, Display, TEXT("✓ StaticMesh '%s' OK : %d polygones <= max %d"), *FactoryNode->GetDisplayLabel(), TotalPolygons, MaxPolygons);
        }
    });

    // Textures
    BaseNodeContainer->IterateNodesOfType<UInterchangeTextureFactoryNode>(
        [&](const FString& NodeID, UInterchangeTextureFactoryNode* TextureNode)
    {
        if (!TextureNode)
        {
            UE_LOG(LogTemp, Warning, TEXT("Node texture nul détecté pour %s"), *NodeID);
            return;
        }

        FString AssetName = TextureNode->GetAssetName();  // ✅ Utilise GetAssetName()

        if (!AssetName.StartsWith(TEXT("T_")))
        {
            FString NewAssetName = FString(TEXT("T_")) + AssetName;
            TextureNode->SetAssetName(NewAssetName);  // ✅ Utilise SetAssetName()
            UE_LOG(LogTemp, Warning, TEXT(" Texture2D '%s' renommée auto en '%s'"), *AssetName, *NewAssetName);
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("✓ Texture2D '%s' nom valide"), *AssetName);
        }
    });

    // Materials (nouveau)
    BaseNodeContainer->IterateNodesOfType<UInterchangeMaterialFactoryNode>(
        [&](const FString& NodeID, UInterchangeMaterialFactoryNode* MaterialNode)
    {
        if (!MaterialNode)
        {
            UE_LOG(LogTemp, Warning, TEXT("Node material nul détecté pour %s"), *NodeID);
            return;
        }

        FString AssetName = MaterialNode->GetDisplayLabel();

        if (!AssetName.StartsWith(TEXT("M_")))
        {
            FString NewAssetName = FString(TEXT("M_")) + AssetName;
            MaterialNode->SetDisplayLabel(NewAssetName);
            UE_LOG(LogTemp, Warning, TEXT(" Material '%s' renommé auto en '%s'"), *AssetName, *NewAssetName);
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("✓ Material '%s' nom valide"), *AssetName);
        }
    });

    for (const FString& ID : NodesToRemove)
    {
        BaseNodeContainer->RemoveNode(ID);
        UE_LOG(LogTemp, Warning, TEXT(" Node supprimé : %s"), *ID);
    }

    if (bCancelImportEntire)
    {
        TArray<FString> AllNodeIDs;
        BaseNodeContainer->IterateNodesOfType<UInterchangeBaseNode>(
            [&](const FString& NodeID, UInterchangeBaseNode* Node)
        {
            AllNodeIDs.Add(NodeID);
        });

        for (const FString& ID : AllNodeIDs)
        {
            BaseNodeContainer->RemoveNode(ID);
        }

        UE_LOG(LogTemp, Error, TEXT(" IMPORT ANNULÉ - Erreurs de validation détectées"));
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT(" IMPORT VALIDÉ - Tous les assets sont conformes"));
    }
    // Skeletons
    BaseNodeContainer->IterateNodesOfType<UInterchangeSkeletonFactoryNode>(
        [&](const FString& NodeID, UInterchangeSkeletonFactoryNode* SkeletonNode)
    {
        if (!SkeletonNode)
        {
            UE_LOG(LogTemp, Warning, TEXT("Node skeleton nul détecté pour %s"), *NodeID);
            return;
        }

        FString AssetName = SkeletonNode->GetAssetName();

        if (!AssetName.StartsWith(TEXT("SKEL_")))
        {
            FString NewAssetName = FString(TEXT("SKEL_")) + AssetName;
            SkeletonNode->SetAssetName(NewAssetName);
            UE_LOG(LogTemp, Warning, TEXT(" Skeleton '%s' renommé auto en '%s'"), *AssetName, *NewAssetName);
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("✓ Skeleton '%s' nom valide"), *AssetName);
        }
    });

    // Physics Assets
    BaseNodeContainer->IterateNodesOfType<UInterchangePhysicsAssetFactoryNode>(
        [&](const FString& NodeID, UInterchangePhysicsAssetFactoryNode* PhysicsAssetNode)
    {
        if (!PhysicsAssetNode)
        {
            UE_LOG(LogTemp, Warning, TEXT("Node physics asset nul détecté pour %s"), *NodeID);
            return;
        }

        FString AssetName = PhysicsAssetNode->GetDisplayLabel();

        if (!AssetName.StartsWith(TEXT("PHYS_")))
        {
            FString NewAssetName = FString(TEXT("PHYS_")) + AssetName;
            PhysicsAssetNode->SetDisplayLabel(NewAssetName);
            UE_LOG(LogTemp, Warning, TEXT(" PhysicsAsset '%s' renommé auto en '%s'"), *AssetName, *NewAssetName);
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("✓ PhysicsAsset '%s' nom valide"), *AssetName);
        }
    });
}
