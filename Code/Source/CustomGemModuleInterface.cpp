
#include "CustomGemModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <CustomGem/CustomGemTypeIds.h>

#include <Clients/CustomGemSystemComponent.h>

namespace CustomGem
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(CustomGemModuleInterface,
        "CustomGemModuleInterface", CustomGemModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(CustomGemModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(CustomGemModuleInterface, AZ::SystemAllocator);

    CustomGemModuleInterface::CustomGemModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            CustomGemSystemComponent::CreateDescriptor(),
            CustomGemComponent::CreateDescriptor()
            });
    }

    AZ::ComponentTypeList CustomGemModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<CustomGemSystemComponent>(),
        };
    }
} // namespace CustomGem
