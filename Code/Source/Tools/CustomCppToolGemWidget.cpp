
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
#include <Atom/RPI.Reflect/Material/MaterialTypeAsset.h>

//Browser
#include <AzToolsFramework/AssetBrowser/Entries/AssetBrowserEntry.h>
#include <AzToolsFramework/AssetBrowser/Entries/SourceAssetBrowserEntry.h>
#include <AzToolsFramework/AssetBrowser/Entries/ProductAssetBrowserEntry.h>

// Debug
#include <AzCore/Debug/Trace.h>
#include <AzCore/IO/FileIO.h>

// Header
#include "CustomCppToolGemWidget.h"
#include "ModelBuilder.h"

namespace CustomCppToolGem
{
    CustomCppToolGemWidget::CustomCppToolGemWidget(QWidget* parent)
        : QWidget(parent)
        , m_matAssetID()
    {

        setAcceptDrops(true);

        auto* main = new QVBoxLayout(this);

        auto* intro = new QLabel(tr("Click the button to spawn a cube at world origin."), this);
        main->addWidget(intro);

        auto* row = new QHBoxLayout();
        auto* btnGenerate = new QPushButton(tr("Generate"), this);
        btnGenerate->setObjectName("Generate Button");
        row->addStretch(1);
        row->addWidget(btnGenerate);
        row->addStretch(1);

        m_pathEdit = new QLineEdit(this);
        m_pathEdit->setPlaceholderText(tr(R"(Drag Material Assets)"));
        m_pathEdit->setObjectName("PathDropZone");
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

    bool IsMaterialAssetId(const AZ::Data::AssetId& assetId)
    {
        if (!assetId.IsValid())
        {
            return false;
        }

        // Ask the catalog for info (includes the asset's type UUID)
        AZ::Data::AssetInfo info;
        AZ::Data::AssetCatalogRequestBus::BroadcastResult(
            info, &AZ::Data::AssetCatalogRequests::GetAssetInfoById, assetId);

        if (!info.m_assetId.IsValid())
        {
            return false;
        }

        const AZ::Data::AssetType matType      = azrtti_typeid<AZ::RPI::MaterialAsset>();
        const AZ::Data::AssetType matTypeType  = azrtti_typeid<AZ::RPI::MaterialTypeAsset>();

        return (info.m_assetType == matType) || (info.m_assetType == matTypeType);
    }
    
    void CustomCppToolGemWidget::dropEvent(QDropEvent* event) {

        const QPoint localPos = event->pos();

        QWidget* target = childAt(localPos);   // nullptr means it hit the parent background
        if (!target) target = this;

        // Identify the zone by objectName
        const QString targetName = target->objectName();
        AZ_Printf("CustomCppToolGem", "On Asset Box: %s", targetName.toStdString().c_str());

        if (HasAssetBrowserEntries(event->mimeData()))
        {
            using namespace AzToolsFramework::AssetBrowser;
            event->acceptProposedAction();
            AZ_Printf("CustomCppToolGem", "Asset Detected");
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

                    if (IsMaterialAssetId(assetId)) {
                        m_matAssetID = assetId;
                        m_pathEdit->setText(product->GetName().c_str());
                    }
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

    void CustomCppToolGemWidget::OnGenerateClicked()
    {
        AZ_Printf("CustomGem", "Information");
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
        // AZ::Data::Asset<AZ::RPI::ModelAsset> modelAsset;
        // {
        //     const AZ::Data::AssetId assetId(AZ::Uuid::CreateRandom());
        //     modelAsset = AZ::Data::AssetManager::Instance().CreateAsset(
        //         assetId, azrtti_typeid<AZ::RPI::ModelAsset>(), AZ::Data::AssetLoadBehavior::PreLoad);
        //     AZ::RPI::ModelAssetHelpers::CreateUnitX(modelAsset.Get());
        // }
        AZ::Data::Asset<AZ::RPI::ModelAsset> modelAsset = CustomGem::ModelBuilder::BuildCube();

        AZ::Render::MeshComponentRequestBus::Event(
            entityId, &AZ::Render::MeshComponentRequests::SetModelAsset, modelAsset);

        // 5) Apply it to the material component
        if (m_matAssetID.IsValid()) {
            AZ::Render::MaterialComponentRequestBus::Event(
                        entityId, &AZ::Render::MaterialComponentRequests::SetMaterialAssetIdOnDefaultSlot, m_matAssetID);
        }
        
        // 6) Make it easy to see: select the new entity in the outliner
        AzToolsFramework::EntityIdList selection{ entityId };
        AzToolsFramework::ToolsApplicationRequestBus::Broadcast(
            &AzToolsFramework::ToolsApplicationRequests::SetSelectedEntities, selection);
    }

}

#include <moc_CustomCppToolGemWidget.cpp>
