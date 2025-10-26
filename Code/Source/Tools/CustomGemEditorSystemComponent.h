
#pragma once

#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <Clients/CustomGemSystemComponent.h>

namespace CustomGem
{
    /// System component for CustomGem editor
    class CustomGemEditorSystemComponent
        : public CustomGemSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = CustomGemSystemComponent;
    public:
        AZ_COMPONENT_DECL(CustomGemEditorSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        CustomGemEditorSystemComponent();
        ~CustomGemEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    };
} // namespace CustomGem
