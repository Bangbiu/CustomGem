#pragma once

struct JsonAsset
{
    AZ_RTTI(MyJsonData, "{YOUR_UUID_HERE}"); // Replace with a unique UUID
    AZ_CLASS_ALLOCATOR(JsonAsset, AZ::SystemAllocator);

    AZStd::string m_name;
    int m_value;

    // ... other members
};