
#include <AzCore/Utils/Utils.h>

// UI
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>

// Util
#include <AzCore/Asset/AssetManager.h>
#include <Atom/RPI.Edit/Common/AssetUtils.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Vector3.h>

#include <AzToolsFramework/API/ToolsApplicationAPI.h>
#include <AzToolsFramework/API/EditorEntityAPI.h>
#include <AzToolsFramework/ToolsComponents/TransformComponent.h>

// Atom (Model helpers)
#include <Atom/RPI.Reflect/Model/ModelAsset.h>
#include <Atom/RPI.Reflect/Model/ModelAssetHelpers.h>

// Editor Mesh (preview in Editor)
#include <AtomLyIntegration/CommonFeatures/Mesh/MeshComponentBus.h>
#include <AtomLyIntegration/CommonFeatures/Mesh/MeshComponentConstants.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentConstants.h>
#include <AzToolsFramework/Component/EditorComponentAPIBus.h>

// Material
#include <Atom/RPI.Reflect/Material/MaterialAsset.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <Atom/RPI.Reflect/Material/MaterialAssetCreator.h>
#include <Atom/RPI.Reflect/Material/MaterialTypeAsset.h>
#include <Atom/RPI.Edit/Material/MaterialUtils.h>

// Debug
#include <AzCore/Debug/Trace.h>

// Header
#include "CustomCppToolGemWidget.h"


namespace CustomCppToolGem
{
    CustomCppToolGemWidget::CustomCppToolGemWidget(QWidget* parent)
        : QWidget(parent)
    {
        auto* main = new QVBoxLayout(this);

        auto* intro = new QLabel(tr("Click the button to spawn a cube at world origin."), this);
        main->addWidget(intro);

        auto* row = new QHBoxLayout();
        auto* btnGenerate = new QPushButton(tr("Generate"), this);
        row->addStretch(1);
        row->addWidget(btnGenerate);
        row->addStretch(1);
        main->addLayout(row);

        connect(btnGenerate, &QPushButton::clicked, this, &CustomCppToolGemWidget::OnGenerateClicked);

        setLayout(main);
    }

    AZ::Data::Asset<AZ::RPI::MaterialTypeAsset> LoadStandardPBRMaterialType()
    {
        // Source path present in the engine/Gem (works once AP has scanned/processed assets)
        const char* materialTypeSourcePath =
            "@gemroot:Atom_Feature_Common@/Assets/Materials/Types/StandardPBR.materialtype";

        // Ask the Asset Catalog for the AssetId of this MaterialType
        AZ::Data::AssetId typeAssetId;
        AZ::Data::AssetCatalogRequestBus::BroadcastResult(
            typeAssetId,
            &AZ::Data::AssetCatalogRequests::GetAssetIdByPath,
            materialTypeSourcePath,
            azrtti_typeid<AZ::RPI::MaterialTypeAsset>(),
            /*autoRegisterIfNotFound*/ true); // true helps in editor if not yet registered

        if (!typeAssetId.IsValid())
        {
            AZ_Error("CustomGem", false, "MaterialType not found via catalog: %s", materialTypeSourcePath);
            return {};
        }

        // Load the asset via AssetManager and block
        auto matType = AZ::Data::AssetManager::Instance().GetAsset<AZ::RPI::MaterialTypeAsset>(
            typeAssetId, AZ::Data::AssetLoadBehavior::PreLoad);
        matType.BlockUntilLoadComplete();

        if (!matType.IsReady())
        {
            AZ_Error("CustomGem", false, "MaterialType failed to load: %s", typeAssetId.ToString<AZStd::string>().c_str());
            return {};
        }

        return matType;
    }

    AZ::Data::Asset<AZ::RPI::MaterialAsset> CreateDefaultGrayMaterial() {
        using namespace AZ::RPI;

        AZ::Data::Asset<AZ::RPI::MaterialTypeAsset> matType = LoadStandardPBRMaterialType();

        // AZ::Data::Asset<AZ::RPI::MaterialAsset> matAsset;
        // {
        //     const AZ::Data::AssetId matId(AZ::Uuid::CreateRandom());
        //     MaterialAssetCreator creator;
        //     creator.Begin(matId, matType);
        //     creator.SetPropertyValue(AZ::Name("baseColor.color"), AZ::Color(0.8f, 0.8f, 0.8f, 1.0f));
        //     creator.End(matAsset);
        // }

        // return matAsset;
        return {};
    }

    void CustomCppToolGemWidget::OnGenerateClicked()
    {
        AZ_Printf("CustomGem", "Not Implemented");
        GenerateCubeEntityAtOrigin();
    }

    void CustomCppToolGemWidget::GenerateCubeEntityAtOrigin()
    {
        // 1) Create a new *Editor* entity (shows up immediately in the outliner & viewport)
        AZ::EntityId entityId;
        AzToolsFramework::EditorEntityContextRequestBus::BroadcastResult(
            entityId,
            &AzToolsFramework::EditorEntityContextRequests::CreateNewEditorEntity,
            "GeneratedCube");

        if (!entityId.IsValid())
        {
            AZ_Warning("CustomCppToolGem", false, "Failed to create editor entity.");
            return;
        }


        // 2) Add Transform + Mesh (+ Material) editor components
        //    We use the editor component API to add by type id.
        AZ::ComponentTypeList typeIds = {
            azrtti_typeid<AzToolsFramework::Components::TransformComponent>(),
            AZ::Render::EditorMeshComponentTypeId,
            AZ::Render::EditorMaterialComponentTypeId
        };

        AzToolsFramework::EditorComponentAPIBus::Broadcast(
            &AzToolsFramework::EditorComponentAPIRequests::AddComponentsOfType,
            entityId, typeIds);
        
        
        // 3) Assign Model
        AZ::Data::Asset<AZ::RPI::ModelAsset> modelAsset;
        {
            const AZ::Data::AssetId assetId(AZ::Uuid::CreateRandom());
            modelAsset = AZ::Data::AssetManager::Instance().CreateAsset(
                assetId, azrtti_typeid<AZ::RPI::ModelAsset>(), AZ::Data::AssetLoadBehavior::PreLoad);
            AZ::RPI::ModelAssetHelpers::CreateUnitCube(modelAsset.Get());
        }

        AZ::Render::MeshComponentRequestBus::Event(
            entityId, &AZ::Render::MeshComponentRequests::SetModelAsset, modelAsset);

        // 4) Load default material asset (engine-provided)
        AZ::Data::Asset<AZ::RPI::MaterialAsset> materialAsset =
            AZ::RPI::AssetUtils::LoadAssetByProductPath<AZ::RPI::MaterialAsset>(
                "materials/defaultpbr.azmaterial", AZ::RPI::AssetUtils::TraceLevel::Warning);
        CreateDefaultGrayMaterial();

        // 5) Apply it to the material component
        AZ::Render::MaterialComponentRequestBus::Event(
            entityId, &AZ::Render::MaterialComponentRequests::SetMaterialAssetIdOnDefaultSlot, materialAsset.GetId());
        
        // 6) Make it easy to see: select the new entity in the outliner
        AzToolsFramework::EntityIdList selection{ entityId };
        AzToolsFramework::ToolsApplicationRequestBus::Broadcast(
            &AzToolsFramework::ToolsApplicationRequests::SetSelectedEntities, selection);
    }
}

#include <moc_CustomCppToolGemWidget.cpp>
