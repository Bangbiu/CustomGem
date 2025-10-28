
#include <AzCore/Utils/Utils.h>

// UI
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>

// Util
#include <AzCore/Asset/AssetManager.h>
#include <Atom/RPI.Edit/Common/AssetUtils.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Vector3.h>

#include <AzToolsFramework/API/ToolsApplicationAPI.h>
#include <AzToolsFramework/API/EditorEntityAPI.h>
#include <AzToolsFramework/API/EditorAssetSystemAPI.h>
#include <AzToolsFramework/ToolsComponents/TransformComponent.h>

// Atom (Model helpers)
#include <Atom/RPI.Reflect/Model/ModelAsset.h>
#include <Atom/RPI.Reflect/Model/ModelAssetHelpers.h>

// Editor Mesh (preview in Editor)
#include <AtomLyIntegration/CommonFeatures/Mesh/MeshComponentBus.h>
#include <AtomLyIntegration/CommonFeatures/Mesh/MeshComponentConstants.h>
#include <AzToolsFramework/Component/EditorComponentAPIBus.h>

// Material
#include <Atom/RPI.Reflect/Material/MaterialAsset.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentConstants.h>
#include <Atom/RPI.Reflect/Material/MaterialAssetCreator.h>
#include <Atom/RPI.Reflect/Material/MaterialTypeAsset.h>
#include <Atom/RPI.Edit/Material/MaterialUtils.h>

//Browser
#include <AzToolsFramework/AssetBrowser/Entries/AssetBrowserEntry.h>
#include <AzToolsFramework/AssetBrowser/Entries/SourceAssetBrowserEntry.h>
#include <AzToolsFramework/AssetBrowser/Entries/ProductAssetBrowserEntry.h>

// Debug
#include <AzCore/Debug/Trace.h>

// Header
#include "CustomCppToolGemWidget.h"



namespace CustomCppToolGem
{
    CustomCppToolGemWidget::CustomCppToolGemWidget(QWidget* parent)
        : QWidget(parent)
    {

        setAcceptDrops(true);

        auto* main = new QVBoxLayout(this);

        auto* intro = new QLabel(tr("Click the button to spawn a cube at world origin."), this);
        main->addWidget(intro);

        auto* row = new QHBoxLayout();
        auto* btnGenerate = new QPushButton(tr("Generate"), this);
        row->addStretch(1);
        row->addWidget(btnGenerate);
        row->addStretch(1);

        m_pathEdit = new QLineEdit(this);
        m_pathEdit->setPlaceholderText(tr(R"(e.g. C:\Projects\MyGame\Assets or @projectroot@/Assets)"));
        row->addWidget(m_pathEdit);

        main->addLayout(row);

        connect(btnGenerate, &QPushButton::clicked, this, &CustomCppToolGemWidget::OnGenerateClicked);
        connect(m_pathEdit, &QLineEdit::returnPressed, this, &CustomCppToolGemWidget::OnPathEntered);

        setLayout(main);
    }

    bool HasAssetBrowserEntries(const QMimeData* mime)
    {
        AZStd::vector<const AzToolsFramework::AssetBrowser::AssetBrowserEntry*> entries;
        AzToolsFramework::AssetBrowser::AssetBrowserEntry::FromMimeData(mime, entries);
        return !entries.empty();
    }

    // Event
    void CustomCppToolGemWidget::dragEnterEvent(QDragEnterEvent* event)
    {
        if (HasAssetBrowserEntries(event->mimeData()))
        {
            event->acceptProposedAction();
        }
    }

    void CustomCppToolGemWidget::dragMoveEvent(QDragMoveEvent* event)
    {
        if (HasAssetBrowserEntries(event->mimeData()))
        {
            event->acceptProposedAction();
        }
    }
    
    void CustomCppToolGemWidget::dropEvent(QDropEvent* event) {
        if (HasAssetBrowserEntries(event->mimeData()))
        {
            using namespace AzToolsFramework::AssetBrowser;
            event->acceptProposedAction();
            AZ_Printf("CustomCppToolGem", "asset detected");
            AZStd::vector<const AssetBrowserEntry*> entries;
            AssetBrowserEntry::FromMimeData(event->mimeData(), entries);

            for (const AssetBrowserEntry* entry : entries)
            {
                if (const auto* product = azrtti_cast<const ProductAssetBrowserEntry*>(entry))
                {
                    const AZ::Data::AssetId assetId = product->GetAssetId();
                    const AZStd::string productPath = product->GetFullPath(); // product path on disk (e.g., cache file)
                    const AZStd::string assetIdStr = assetId.ToString<AZStd::string>();

                    AZ_Printf("CustomCppToolGem",
                        "[Product] name='%s'\n  path='%s'\n  assetId=%s\n",
                        product->GetName().c_str(),
                        productPath.c_str(),
                        assetIdStr.c_str());
                } else if (const auto* source = azrtti_cast<const SourceAssetBrowserEntry*>(entry))
                {
                    const AZStd::string sourcePath = source->GetFullPath(); // absolute source path
                    const AZ::Uuid sourceUuid = source->GetSourceUuid();
                    const AZStd::string uuidStr = sourceUuid.ToString<AZStd::string>();

                    AZ_Printf("CustomCppToolGem",
                        "[Source] name='%s'\n  path='%s'\n  sourceUuid=%s\n",
                        source->GetName().c_str(),
                        sourcePath.c_str(),
                        uuidStr.c_str());

                    // Note: Source entries do not have an AssetId yet; products do.
                }
            } 
        } 
    }

    void CustomCppToolGemWidget::OnPathEntered()
    {
        const QString rawPath = m_pathEdit->text().trimmed();
        if (rawPath.isEmpty())
        {
            AZ_Warning("CustomCppToolGem", false, "Path is empty.");
            return;
        }


    }

    AZ::Data::Asset<AZ::RPI::MaterialTypeAsset> LoadStandardPBRMaterialType()
    {
        return {};
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
