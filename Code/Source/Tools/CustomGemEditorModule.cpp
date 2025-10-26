
#include <CustomGem/CustomGemTypeIds.h>
#include <CustomGemModuleInterface.h>
#include "CustomGemEditorSystemComponent.h"

namespace CustomGem
{
    class CustomGemEditorModule
        : public CustomGemModuleInterface
    {
    public:
        AZ_RTTI(CustomGemEditorModule, CustomGemEditorModuleTypeId, CustomGemModuleInterface);
        AZ_CLASS_ALLOCATOR(CustomGemEditorModule, AZ::SystemAllocator);

        CustomGemEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                CustomGemEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<CustomGemEditorSystemComponent>(),
            };
        }
    };
}// namespace CustomGem

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), CustomGem::CustomGemEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_CustomGem_Editor, CustomGem::CustomGemEditorModule)
#endif
