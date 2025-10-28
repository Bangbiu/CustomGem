
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace CustomCppToolGem
{
    class CustomCppToolGemRequests
    {
    public:
        AZ_RTTI(CustomCppToolGemRequests, "{70FC8083-5232-483D-9F9E-C33A84F6F612}");
        virtual ~CustomCppToolGemRequests() = default;
        // Put your public methods here
    };
    
    class CustomCppToolGemBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using CustomCppToolGemRequestBus = AZ::EBus<CustomCppToolGemRequests, CustomCppToolGemBusTraits>;
    using CustomCppToolGemInterface = AZ::Interface<CustomCppToolGemRequests>;

} // namespace CustomCppToolGem
