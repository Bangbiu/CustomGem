#pragma once
#include <AzCore/Component/ComponentBus.h>

namespace CustomGem
{
    // Renamed to avoid clashing with the gem-level bus
    class CustomGemComponentRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(CustomGem::CustomGemComponentRequests, "{B284A952-5BD9-46D1-A291-06818F453798}");
        // Add per-component request methods here, e.g.:
        // virtual void EnableFeature(bool enable) = 0;
    };

    // The ComponentBus uses EntityId addressing by default; spell it out if you like:
    using CustomGemComponentRequestBus = AZ::EBus<CustomGemComponentRequests>;
    // If you want to be explicit about address policy / id type:
    // template<>
    // struct AZ::EBusTraits<CustomGem::CustomGemComponentRequests> : public AZ::ComponentBus { using BusIdType = AZ::EntityId; };

} // namespace CustomGem
