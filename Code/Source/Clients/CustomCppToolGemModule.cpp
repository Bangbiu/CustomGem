
#include <CustomCppToolGem/CustomCppToolGemTypeIds.h>
#include <CustomCppToolGemModuleInterface.h>
#include "CustomCppToolGemSystemComponent.h"

namespace CustomCppToolGem
{
    class CustomCppToolGemModule
        : public CustomCppToolGemModuleInterface
    {
    public:
        AZ_RTTI(CustomCppToolGemModule, CustomCppToolGemModuleTypeId, CustomCppToolGemModuleInterface);
        AZ_CLASS_ALLOCATOR(CustomCppToolGemModule, AZ::SystemAllocator);
    };
}// namespace CustomCppToolGem

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), CustomCppToolGem::CustomCppToolGemModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_CustomCppToolGem, CustomCppToolGem::CustomCppToolGemModule)
#endif
