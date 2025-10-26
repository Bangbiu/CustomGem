
#include "CustomGemSystemComponent.h"

#include <CustomGem/CustomGemTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

namespace CustomGem
{
    AZ_COMPONENT_IMPL(CustomGemSystemComponent, "CustomGemSystemComponent",
        CustomGemSystemComponentTypeId);

    void CustomGemSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CustomGemSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void CustomGemSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("CustomGemService"));
    }

    void CustomGemSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("CustomGemService"));
    }

    void CustomGemSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void CustomGemSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    CustomGemSystemComponent::CustomGemSystemComponent()
    {
        if (CustomGemInterface::Get() == nullptr)
        {
            CustomGemInterface::Register(this);
        }
    }

    CustomGemSystemComponent::~CustomGemSystemComponent()
    {
        if (CustomGemInterface::Get() == this)
        {
            CustomGemInterface::Unregister(this);
        }
    }

    void CustomGemSystemComponent::Init()
    {
    }

    void CustomGemSystemComponent::Activate()
    {
        CustomGemRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void CustomGemSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        CustomGemRequestBus::Handler::BusDisconnect();
    }

    void CustomGemSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace CustomGem
