
#include <CustomCppToolGem/CustomCppToolGemTypeIds.h>
#include <CustomCppToolGemModuleInterface.h>
#include "CustomCppToolGemEditorSystemComponent.h"

void InitCustomCppToolGemResources()
{
    // We must register our Qt resources (.qrc file) since this is being loaded from a separate module (gem)
    Q_INIT_RESOURCE(CustomCppToolGem);
}

namespace CustomCppToolGem
{
    class CustomCppToolGemEditorModule
        : public CustomCppToolGemModuleInterface
    {
    public:
        AZ_RTTI(CustomCppToolGemEditorModule, CustomCppToolGemEditorModuleTypeId, CustomCppToolGemModuleInterface);
        AZ_CLASS_ALLOCATOR(CustomCppToolGemEditorModule, AZ::SystemAllocator);

        CustomCppToolGemEditorModule()
        {
            InitCustomCppToolGemResources();

            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                CustomCppToolGemEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<CustomCppToolGemEditorSystemComponent>(),
            };
        }
    };
}// namespace CustomCppToolGem

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), CustomCppToolGem::CustomCppToolGemEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_CustomCppToolGem_Editor, CustomCppToolGem::CustomCppToolGemEditorModule)
#endif
