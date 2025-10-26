
#pragma once

#include <CustomGem/CustomGemTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace CustomGem
{
    class CustomGemRequests
    {
    public:
        AZ_RTTI(CustomGemRequests, CustomGemRequestsTypeId);
        virtual ~CustomGemRequests() = default;
        // Put your public methods here
    };

    class CustomGemBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using CustomGemRequestBus = AZ::EBus<CustomGemRequests, CustomGemBusTraits>;
    using CustomGemInterface = AZ::Interface<CustomGemRequests>;

} // namespace CustomGem
