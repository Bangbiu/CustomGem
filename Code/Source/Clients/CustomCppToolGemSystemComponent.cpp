
#include "CustomCppToolGemSystemComponent.h"

#include <CustomCppToolGem/CustomCppToolGemTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

namespace CustomCppToolGem
{
    AZ_COMPONENT_IMPL(CustomCppToolGemSystemComponent, "CustomCppToolGemSystemComponent",
        CustomCppToolGemSystemComponentTypeId);

    void CustomCppToolGemSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CustomCppToolGemSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void CustomCppToolGemSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("CustomCppToolGemService"));
    }

    void CustomCppToolGemSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("CustomCppToolGemService"));
    }

    void CustomCppToolGemSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void CustomCppToolGemSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    CustomCppToolGemSystemComponent::CustomCppToolGemSystemComponent()
    {
        if (CustomCppToolGemInterface::Get() == nullptr)
        {
            CustomCppToolGemInterface::Register(this);
        }
    }

    CustomCppToolGemSystemComponent::~CustomCppToolGemSystemComponent()
    {
        if (CustomCppToolGemInterface::Get() == this)
        {
            CustomCppToolGemInterface::Unregister(this);
        }
    }

    void CustomCppToolGemSystemComponent::Init()
    {
    }

    void CustomCppToolGemSystemComponent::Activate()
    {
        CustomCppToolGemRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void CustomCppToolGemSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        CustomCppToolGemRequestBus::Handler::BusDisconnect();
    }

    void CustomCppToolGemSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace CustomCppToolGem
