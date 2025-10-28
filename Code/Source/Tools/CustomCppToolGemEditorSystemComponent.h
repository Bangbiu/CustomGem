
#pragma once

#include <Clients/CustomCppToolGemSystemComponent.h>

#include <AzToolsFramework/Entity/EditorEntityContextBus.h>

namespace CustomCppToolGem
{
    /// System component for CustomCppToolGem editor
    class CustomCppToolGemEditorSystemComponent
        : public CustomCppToolGemSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = CustomCppToolGemSystemComponent;
    public:
        AZ_COMPONENT_DECL(CustomCppToolGemEditorSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        CustomCppToolGemEditorSystemComponent();
        ~CustomCppToolGemEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        // AzToolsFramework::EditorEventsBus overrides ...
        void NotifyRegisterViews() override;
    };
} // namespace CustomCppToolGem
