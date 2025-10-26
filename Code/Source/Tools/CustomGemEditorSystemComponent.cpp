
#include <AzCore/Serialization/SerializeContext.h>
#include "CustomGemEditorSystemComponent.h"

#include <CustomGem/CustomGemTypeIds.h>

namespace CustomGem
{
    AZ_COMPONENT_IMPL(CustomGemEditorSystemComponent, "CustomGemEditorSystemComponent",
        CustomGemEditorSystemComponentTypeId, BaseSystemComponent);

    void CustomGemEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CustomGemEditorSystemComponent, CustomGemSystemComponent>()
                ->Version(0);
        }
    }

    CustomGemEditorSystemComponent::CustomGemEditorSystemComponent() = default;

    CustomGemEditorSystemComponent::~CustomGemEditorSystemComponent() = default;

    void CustomGemEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("CustomGemEditorService"));
    }

    void CustomGemEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("CustomGemEditorService"));
    }

    void CustomGemEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void CustomGemEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void CustomGemEditorSystemComponent::Activate()
    {
        CustomGemSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void CustomGemEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        CustomGemSystemComponent::Deactivate();
    }

} // namespace CustomGem
