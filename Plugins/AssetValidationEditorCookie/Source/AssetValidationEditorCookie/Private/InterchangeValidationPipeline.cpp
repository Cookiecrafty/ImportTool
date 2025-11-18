#include "InterchangeValidationPipeline.h"
#include "Nodes/InterchangeBaseNodeContainer.h"
#include "InterchangeMeshFactoryNode.h"
#include "InterchangeMeshNode.h"
#include "InterchangeStaticMeshLodDataNode.h"
#include "InterchangeTextureFactoryNode.h"
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

    BaseNodeContainer->IterateNodesOfType<UInterchangeMeshFactoryNode>(
        [&](const FString& NodeID, UInterchangeMeshFactoryNode* FactoryNode)
    {
        if (!FactoryNode)
        {
            UE_LOG(LogTemp, Warning, TEXT("Node mesh nul détecté pour %s"), *NodeID);
            return;
        }

        FString AssetName = FactoryNode->GetDisplayLabel();

        // Validation du nommage
        if (!AssetName.StartsWith(TEXT("SM_")))
        {
            NodesToRemove.Add(NodeID);
            bCancelImportEntire = true;
            UE_LOG(LogTemp, Warning, TEXT("❌ StaticMesh '%s' nom invalide (attendu 'SM_')"), *AssetName);
            return;
        }
        UE_LOG(LogTemp, Display, TEXT("✓ StaticMesh '%s' nom valide"), *AssetName);

        // Récupérer les LOD Data Unique IDs
        TArray<FString> LodDataUniqueIds;
        FactoryNode->GetLodDataUniqueIds(LodDataUniqueIds);

        if (LodDataUniqueIds.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ Aucun LOD Data trouvé pour %s"), *AssetName);
            return;
        }

        // Récupérer le premier LOD (LOD 0)
        FString LodNodeID = LodDataUniqueIds[0];
        auto LodDataNode = Cast<UInterchangeStaticMeshLodDataNode>(
            const_cast<UInterchangeBaseNode*>(BaseNodeContainer->GetNode(LodNodeID))
        );

        if (!LodDataNode)
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ LodDataNode nul pour %s"), *LodNodeID);
            return;
        }

        // Récupérer tous les mesh UIDs du LOD
        TArray<FString> MeshUids;
        LodDataNode->GetMeshUids(MeshUids);

        if (MeshUids.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ Aucun mesh trouvé dans le LOD pour %s"), *AssetName);
            return;
        }

        // Récupérer le polygon count total
        int32 TotalPolygons = 0;

        for (const FString& MeshUid : MeshUids)
        {
            UE_LOG(LogTemp, Display, TEXT("  MeshUid: %s"), *MeshUid);

            auto BaseNode = const_cast<UInterchangeBaseNode*>(BaseNodeContainer->GetNode(MeshUid));
            if (BaseNode)
            {
                UE_LOG(LogTemp, Display, TEXT("    Type réel: %s"), *BaseNode->GetClass()->GetName());

                auto MeshNode = Cast<UInterchangeMeshNode>(BaseNode);
                if (MeshNode)
                {
                    int32 PolygonCount = 0;
                    if (MeshNode->GetCustomPolygonCount(PolygonCount))
                    {
                        UE_LOG(LogTemp, Display, TEXT("    ✓ MeshNode PolygonCount: %d"), PolygonCount);
                        TotalPolygons += PolygonCount;
                    }
                }
                else
                {
                    // C'est un SceneNode, récupérer l'AssetInstanceUid
                    FString AssetInstanceUid;
                    if (BaseNode->GetStringAttribute(TEXT("AssetInstanceUid"), AssetInstanceUid))
                    {
                        UE_LOG(LogTemp, Display, TEXT("    SceneNode AssetInstanceUid: %s"), *AssetInstanceUid);

                        // Récupérer le vrai mesh node
                        auto ActualMeshNode = Cast<UInterchangeMeshNode>(
                            const_cast<UInterchangeBaseNode*>(BaseNodeContainer->GetNode(AssetInstanceUid))
                        );

                        if (ActualMeshNode)
                        {
                            int32 PolygonCount = 0;
                            if (ActualMeshNode->GetCustomPolygonCount(PolygonCount))
                            {
                                UE_LOG(LogTemp, Display, TEXT("    ✓ ActualMeshNode PolygonCount: %d"), PolygonCount);
                                TotalPolygons += PolygonCount;
                            }
                        }
                        else
                        {
                            UE_LOG(LogTemp, Display, TEXT("    ✗ Cast to MeshNode failed for %s"), *AssetInstanceUid);
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Display, TEXT("    ✗ No AssetInstanceUid found"));
                    }
                }
            }
        }

        // Validation du nombre de polygones
        UE_LOG(LogTemp, Warning, TEXT("========== POLYGONE COUNT =========="));
        UE_LOG(LogTemp, Warning, TEXT("Asset: '%s'"), *AssetName);
        UE_LOG(LogTemp, Warning, TEXT("Total Polygones : %d"), TotalPolygons);
        UE_LOG(LogTemp, Warning, TEXT("Limite : %d"), MaxPolygons);
        UE_LOG(LogTemp, Warning, TEXT("====================================="));

        if (TotalPolygons > MaxPolygons)
        {
            NodesToRemove.Add(NodeID);
            bCancelImportEntire = true;
            UE_LOG(LogTemp, Error, TEXT("❌ StaticMesh '%s' trop lourd : %d polygones > max %d"), *AssetName, TotalPolygons, MaxPolygons);
        }
        else if (TotalPolygons > 0)
        {
            UE_LOG(LogTemp, Display, TEXT("✓ StaticMesh '%s' OK : %d polygones <= max %d"), *AssetName, TotalPolygons, MaxPolygons);
        }
    });

    BaseNodeContainer->IterateNodesOfType<UInterchangeTextureFactoryNode>(
        [&](const FString& NodeID, UInterchangeTextureFactoryNode* TextureNode)
    {
        if (!TextureNode)
        {
            UE_LOG(LogTemp, Warning, TEXT("Node texture nul détecté pour %s"), *NodeID);
            return;
        }

        FString AssetName = TextureNode->GetDisplayLabel();

        if (!AssetName.StartsWith(TEXT("T_")))
        {
            NodesToRemove.Add(NodeID);
            bCancelImportEntire = true;
            UE_LOG(LogTemp, Warning, TEXT("❌ Texture2D '%s' nom invalide (attendu 'T_')"), *AssetName);
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("✓ Texture2D '%s' nom valide"), *AssetName);
        }
    });

    // Supprimer nodes invalides
    for (const FString& ID : NodesToRemove)
    {
        BaseNodeContainer->RemoveNode(ID);
        UE_LOG(LogTemp, Warning, TEXT("❌ Node supprimé : %s"), *ID);
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

        UE_LOG(LogTemp, Error, TEXT("🚫 IMPORT ANNULÉ - Erreurs de validation détectées"));
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("✅ IMPORT VALIDÉ - Tous les assets sont conformes"));
    }
}
