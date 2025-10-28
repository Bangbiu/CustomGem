
#pragma once

namespace CustomCppToolGem
{
    // System Component TypeIds
    inline constexpr const char* CustomCppToolGemSystemComponentTypeId = "{4AF81D28-355E-4853-A818-703B89A89214}";
    inline constexpr const char* CustomCppToolGemEditorSystemComponentTypeId = "{7D121993-89C1-4E19-A4F9-D2D58F23E6B3}";

    // Module derived classes TypeIds
    inline constexpr const char* CustomCppToolGemModuleInterfaceTypeId = "{DF4D4577-4BDE-401F-BC94-B3CEC818E06B}";
    inline constexpr const char* CustomCppToolGemModuleTypeId = "{F523D08E-67E0-47F1-94CB-BA593979C72D}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* CustomCppToolGemEditorModuleTypeId = CustomCppToolGemModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* CustomCppToolGemRequestsTypeId = "{1319D896-C8B4-4E1C-8D3F-89C67BD4A29C}";
} // namespace CustomCppToolGem
