
#include <AzCore/Serialization/SerializeContext.h>

#include <AzToolsFramework/API/ViewPaneOptions.h>

#include "CustomCppToolGemWidget.h"
#include "CustomCppToolGemEditorSystemComponent.h"

#include <CustomCppToolGem/CustomCppToolGemTypeIds.h>

namespace CustomCppToolGem
{
    AZ_COMPONENT_IMPL(CustomCppToolGemEditorSystemComponent, "CustomCppToolGemEditorSystemComponent",
        CustomCppToolGemEditorSystemComponentTypeId, BaseSystemComponent);

    void CustomCppToolGemEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CustomCppToolGemEditorSystemComponent, CustomCppToolGemSystemComponent>()
                ->Version(0);
        }
    }

    CustomCppToolGemEditorSystemComponent::CustomCppToolGemEditorSystemComponent() = default;

    CustomCppToolGemEditorSystemComponent::~CustomCppToolGemEditorSystemComponent() = default;

    void CustomCppToolGemEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("CustomCppToolGemEditorService"));
    }

    void CustomCppToolGemEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("CustomCppToolGemEditorService"));
    }

    void CustomCppToolGemEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void CustomCppToolGemEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void CustomCppToolGemEditorSystemComponent::Activate()
    {
        CustomCppToolGemSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void CustomCppToolGemEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        CustomCppToolGemSystemComponent::Deactivate();
    }

    void CustomCppToolGemEditorSystemComponent::NotifyRegisterViews()
    {
        AzToolsFramework::ViewPaneOptions options;
        options.paneRect = QRect(100, 100, 500, 400);
        options.showOnToolsToolbar = true;
        options.toolbarIcon = ":/CustomCppToolGem/toolbar_icon.svg";

        // Register our custom widget as a dockable tool with the Editor under an Examples sub-menu
        AzToolsFramework::RegisterViewPane<CustomCppToolGemWidget>("CustomCppToolGem", "Examples", options);
    }

} // namespace CustomCppToolGem
