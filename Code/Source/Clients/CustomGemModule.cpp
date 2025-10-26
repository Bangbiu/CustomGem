
#include <CustomGem/CustomGemTypeIds.h>
#include <CustomGemModuleInterface.h>
#include "CustomGemSystemComponent.h"

namespace CustomGem
{
    class CustomGemModule
        : public CustomGemModuleInterface
    {
    public:
        AZ_RTTI(CustomGemModule, CustomGemModuleTypeId, CustomGemModuleInterface);
        AZ_CLASS_ALLOCATOR(CustomGemModule, AZ::SystemAllocator);
    };
}// namespace CustomGem

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), CustomGem::CustomGemModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_CustomGem, CustomGem::CustomGemModule)
#endif
