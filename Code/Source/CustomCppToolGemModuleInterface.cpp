
#include "CustomCppToolGemModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <CustomCppToolGem/CustomCppToolGemTypeIds.h>

#include <Clients/CustomCppToolGemSystemComponent.h>

namespace CustomCppToolGem
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(CustomCppToolGemModuleInterface,
        "CustomCppToolGemModuleInterface", CustomCppToolGemModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(CustomCppToolGemModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(CustomCppToolGemModuleInterface, AZ::SystemAllocator);

    CustomCppToolGemModuleInterface::CustomCppToolGemModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            CustomCppToolGemSystemComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList CustomCppToolGemModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<CustomCppToolGemSystemComponent>(),
        };
    }
} // namespace CustomCppToolGem
