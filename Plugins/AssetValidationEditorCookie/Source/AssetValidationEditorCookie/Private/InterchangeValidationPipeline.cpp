#include "InterchangeValidationPipeline.h"
#include "Nodes/InterchangeBaseNodeContainer.h"
#include "InterchangeMeshFactoryNode.h"
#include "InterchangeTextureFactoryNode.h"
#include "InterchangeActorFactoryNode.h"
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
    const int32 MaxTriangles = 10;
    TArray<FString> NodesToRemove;

    BaseNodeContainer->IterateNodesOfType<UInterchangeMeshFactoryNode>(
        [&](const FString& NodeID, UInterchangeMeshFactoryNode* Node)
    {
        if (!Node)
        {
            UE_LOG(LogTemp, Warning, TEXT("Node mesh nul détecté pour %s"), *NodeID);
            return;
        }

        FString AssetName = Node->GetDisplayLabel();

        if (!AssetName.StartsWith(TEXT("SM_")))
        {
            NodesToRemove.Add(NodeID);
            bCancelImportEntire = true;
            UE_LOG(LogTemp, Warning, TEXT("StaticMesh '%s' nom invalide (attendu 'SM_')"), *AssetName);
            return;
        }

        int32 TotalTriangles = 0;
        bool bHasTriangleCount = Node->GetPayloadKeyInt32Attribute(TEXT("TotalTriangles"), TotalTriangles);

        if (!bHasTriangleCount)
        {
            UE_LOG(LogTemp, Warning, TEXT("Triangle count non trouvé pour node %s"), *NodeID);
        }
        else if (TotalTriangles > MaxTriangles)
        {
            NodesToRemove.Add(NodeID);
            bCancelImportEntire = true;
            UE_LOG(LogTemp, Warning, TEXT("StaticMesh '%s' trop lourd triangles : %d > %d"), *AssetName, TotalTriangles, MaxTriangles);
        }
    });

    BaseNodeContainer->IterateNodesOfType<UInterchangeTextureFactoryNode>(
        [&](const FString& NodeID, UInterchangeTextureFactoryNode* Node)
    {
        if (!Node)
        {
            UE_LOG(LogTemp, Warning, TEXT("Node texture nul détecté pour %s"), *NodeID);
            return;
        }

        FString AssetName = Node->GetDisplayLabel();

        if (!AssetName.StartsWith(TEXT("T_")))
        {
            NodesToRemove.Add(NodeID);
            bCancelImportEntire = true;
            UE_LOG(LogTemp, Warning, TEXT("Texture2D '%s' nom invalide (attendu 'T_')"), *AssetName);
        }
    });

    // Supprimer nodes invalides
    for (const FString& ID : NodesToRemove)
    {
        BaseNodeContainer->RemoveNode(ID);
        UE_LOG(LogTemp, Warning, TEXT("Node supprimé : %s"), *ID);
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

        UE_LOG(LogTemp, Warning, TEXT("Tout l'import a été annulé à cause d'une erreur."));
        //cast interchange mesh node et polygon count---------------------------------------------------------------------+---------------------------------------------------
    }
}
