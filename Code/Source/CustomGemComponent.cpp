
#include <CustomGemComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Math/Quaternion.h>
#include <AzCore/Math/Color.h>
#include <AzCore/Asset/AssetSerializer.h>

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Debug/Trace.h>



namespace CustomGem
{
    //AZ_COMPONENT_IMPL(CustomGemComponent, "CustomGemComponent", "{84F70987-84AC-4670-9F3D-16640ED10BBE}");
    CustomGemComponent::CustomGemComponent()
        : m_exampleOfEntityID(AZ::EntityId())    // or AZ::EntityId(12345)
        , m_exampleOfVector3(AZ::Vector3(10.0f, 5.0f, -1.0f))
        , m_exampleOfQuaternion(AZ::Quaternion::CreateIdentity())
        , m_exampleOfColor(AZ::Color(255.0f,178.0f,0.0f,1.0f))
        , m_exampleOfEntities()
        , m_exampleOfFloat(50.0f)
        , m_exampleOfSliderValue(20.0f)
        , exampleOfBool(true)
        , m_imageAsset()
    {}

    void CustomGemComponent::Activate()
    {
        CustomGemComponentRequestBus::Handler::BusConnect(GetEntityId());
        if (AZ::RPI::Scene* scene = AZ::RPI::Scene::GetSceneForEntityId(GetEntityId()))
        {
            m_meshFp = scene->GetFeatureProcessor<AZ::Render::MeshFeatureProcessorInterface>();
        }
    }

    void CustomGemComponent::Deactivate()
    {
        CustomGemComponentRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    AZ::Data::Asset<AZ::RPI::ModelAsset> CustomGemComponent::BuildModel() {
        // 3x3 meter quad (two triangles), centered at origin, +Z up
        struct VertexPNTBuV
        {
            AZ::Vector3 position;
            AZ::Vector3 normal;
            AZ::Vector4 tangent;   // xyz = tangent, w = handedness (1 or -1)
            AZ::Vector3 bitangent; // optional stream; included to match standard input
            AZ::Vector2 uv;
        };

        float h = 1.5f;
        AZ::Vector3 N  = AZ::Vector3(0.f, 0.f, 1.f);
        AZ::Vector4 T  = AZ::Vector4(1.f, 0.f, 0.f, 1.f); // +X, RH = +1
        AZ::Vector3 B  = AZ::Vector3(0.f, 1.f, 0.f);

        const VertexPNTBuV verts[4] = {
            { {-h, -h, 0.f}, N, T, B, AZ::Vector2(0.f, 0.f) }, // 0
            { { h, -h, 0.f}, N, T, B, AZ::Vector2(1.f, 0.f) }, // 1
            { { h,  h, 0.f}, N, T, B, AZ::Vector2(1.f, 1.f) }, // 2
            { {-h,  h, 0.f}, N, T, B, AZ::Vector2(0.f, 1.f) }, // 3
        };
        const uint16_t indices[6] = { 0,1,2,  0,2,3 };


        // --- Create vertex buffer asset ---
        AZ::RPI::BufferAssetCreator vbCreator;
        AZ::Data::AssetId vbAssetId = AZ::Data::AssetId(AZ::Uuid::CreateRandom());

        AZ::RHI::BufferDescriptor vbufferDesc;
        vbufferDesc.m_bindFlags = AZ::RHI::BufferBindFlags::InputAssembly; // vertex/index usage
        vbufferDesc.m_byteCount = sizeof(verts);

        vbCreator.Begin(vbAssetId);
        vbCreator.SetBuffer(verts, sizeof(verts), vbufferDesc);
        

        vbCreator.SetBufferViewDescriptor(
            AZ::RHI::BufferViewDescriptor::CreateStructured(
                /*byteOffset*/ 0,
                /*elementCount*/ static_cast<uint32_t>(AZ_ARRAY_SIZE(verts)),
                /*elementSize*/ static_cast<uint32_t>(sizeof(VertexPNTBuV))
            )
        );
        
        // Use the common pool for static input buffers
        vbCreator.SetUseCommonPool(AZ::RPI::CommonBufferPoolType::StaticInputAssembly);

        AZ::Data::Asset<AZ::RPI::BufferAsset> vbAsset;
        vbCreator.End(vbAsset);


        AZ::RPI::BufferAssetCreator ibCreator;
        AZ::Data::AssetId ibAssetId = AZ::Data::AssetId(AZ::Uuid::CreateRandom());

        AZ::RHI::BufferDescriptor ibufferDesc;
        ibufferDesc.m_bindFlags = AZ::RHI::BufferBindFlags::InputAssembly; 
        ibufferDesc.m_byteCount = sizeof(indices);

        ibCreator.Begin(ibAssetId);

        ibCreator.SetBuffer(indices, sizeof(indices), vbufferDesc);
        ibCreator.SetBufferViewDescriptor(
            AZ::RHI::BufferViewDescriptor::CreateTyped(
                0,
                static_cast<uint32_t>(AZ_ARRAY_SIZE(indices)),  // element COUNT
                AZ::RHI::Format::R16_UINT
            )
        );

        ibCreator.SetUseCommonPool(AZ::RPI::CommonBufferPoolType::StaticInputAssembly);
        AZ::Data::Asset<AZ::RPI::BufferAsset> ibAsset;
        ibCreator.End(ibAsset);


        
        AZ::Data::Asset<AZ::RPI::ModelAsset> modelAsset;
        return modelAsset;
    }

    AZ::Crc32 CustomGemComponent::OnAnyChanged()
    {
        if (m_imageAsset.GetId().IsValid())
        {
            m_imageAsset.QueueLoad();
            m_imageAsset.BlockUntilLoadComplete();
            const AZ::RPI::StreamingImageAsset* img = m_imageAsset.Get();
            if (img) {
                AZ_Printf("CustomGemComponent", "Loaded");
            }
        }
        return AZ::Edit::PropertyRefreshLevels::ValuesOnly;
    }
    void CustomGemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CustomGemComponent, AZ::Component>()
                ->Version(1)
                ->Field("Float Value", &CustomGemComponent::m_exampleOfFloat)
                ->Field("Slider Value", &CustomGemComponent::m_exampleOfSliderValue)
                
                ->Field("bool value", &CustomGemComponent::exampleOfBool)
                ->Field("EntityID", &CustomGemComponent::m_exampleOfEntityID)
                ->Field("Multiple entities IDs", &CustomGemComponent::m_exampleOfEntities)
                ->Field("Vector3", &CustomGemComponent::m_exampleOfVector3)
                ->Field("Quaternion", &CustomGemComponent::m_exampleOfQuaternion)
                ->Field("Color", &CustomGemComponent::m_exampleOfColor)
                ->Field("Image Asset", &CustomGemComponent::m_imageAsset)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<CustomGemComponent>("CustomGemComponent", "[Description of functionality provided by this component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    // Float Input Box
                    ->Attribute("ChangeNotify", AZ_CRC("RefreshValues", 0x28e720d4))
                        ->DataElement(0, &CustomGemComponent::m_exampleOfFloat, "Float", "Comma value within the range [1-100]")
                        ->Attribute("Min", 1.0f)
                        ->Attribute("Max", 100.0f)
                        ->Attribute(AZ::Edit::Attributes::Suffix, " m")
                    // Float (slider with unit suffix)
                    ->DataElement(AZ::Edit::UIHandlers::Slider, &CustomGemComponent::m_exampleOfSliderValue,
                                "Slider", "Comma value within the range [1-100]")
                        ->Attribute(AZ::Edit::Attributes::Min, 1.0f)
                        ->Attribute(AZ::Edit::Attributes::Max, 100.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.1f)
                        ->Attribute(AZ::Edit::Attributes::Suffix, " m")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &CustomGemComponent::OnAnyChanged)
                    
                    // Bool (checkbox)
                    ->DataElement(AZ::Edit::UIHandlers::CheckBox, &CustomGemComponent::exampleOfBool,
                                "Bool", "Toggle this feature")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &CustomGemComponent::OnAnyChanged)

                    // Single EntityId (entity picker)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CustomGemComponent::m_exampleOfEntityID,
                                "EntityID", "Pick one entity")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &CustomGemComponent::OnAnyChanged)

                    // Multiple EntityIds (container editor with Add button)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CustomGemComponent::m_exampleOfEntities,
                                "Multiple entities IDs", "Pick multiple entities")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &CustomGemComponent::OnAnyChanged)

                    // Vector3 (default vector editor; step & decimals help precision)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CustomGemComponent::m_exampleOfVector3,
                                "Vector3", "3D vector")
                        ->Attribute(AZ::Edit::Attributes::Decimals, 3)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.01f)
                        ->Attribute(AZ::Edit::Attributes::Suffix, " m")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &CustomGemComponent::OnAnyChanged)

                    // Quaternion (default 4D editor)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CustomGemComponent::m_exampleOfQuaternion,
                                "Quaternion", "Orientation (w, x, y, z)")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &CustomGemComponent::OnAnyChanged)

                    // Color (color picker with alpha)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CustomGemComponent::m_exampleOfColor,
                                "Color", "Tint color")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &CustomGemComponent::OnAnyChanged)
                    
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CustomGemComponent::m_imageAsset,
                              "Image", "Pick a Image")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &CustomGemComponent::OnAnyChanged)
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<CustomGemComponent>("CustomGem Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "CustomGem Gem Group")
                ;
        }
    }

    void CustomGemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("CustomGemComponentService"));
    }

    void CustomGemComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }

    void CustomGemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void CustomGemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
} // namespace CustomGem
