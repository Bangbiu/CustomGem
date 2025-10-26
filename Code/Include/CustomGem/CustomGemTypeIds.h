
#pragma once

namespace CustomGem
{
    // System Component TypeIds
    inline constexpr const char* CustomGemSystemComponentTypeId = "{0EB5F513-59E6-46FA-9E1A-A0DA07C333F6}";
    inline constexpr const char* CustomGemEditorSystemComponentTypeId = "{BC0916BA-994E-445E-8EC0-581D7C9CBB89}";

    // Module derived classes TypeIds
    inline constexpr const char* CustomGemModuleInterfaceTypeId = "{26752AF0-1660-4841-9D6B-1F9FBE683D8F}";
    inline constexpr const char* CustomGemModuleTypeId = "{4D87780B-42DB-4AA5-A7C7-C88D5A2333AA}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* CustomGemEditorModuleTypeId = CustomGemModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* CustomGemRequestsTypeId = "{3D8E549B-B6A3-4213-BFDE-19F9A18C57FC}";
} // namespace CustomGem
