// resource_load.h
#pragma once
#include <windows.h>
#include <cstddef>
#include <stdexcept>

struct ResourceView
{
    const void* data = nullptr;
    std::size_t size = 0;
};

inline ResourceView LoadResourceView(HMODULE hModule, int resourceId, const wchar_t* resourceType)
{
    if (!hModule) throw std::runtime_error("hModule is null");

    HRSRC hResInfo = FindResourceW(hModule, MAKEINTRESOURCEW(resourceId), resourceType);
    if (!hResInfo) throw std::runtime_error("FindResourceW failed");

    DWORD size = SizeofResource(hModule, hResInfo);
    if (size == 0) throw std::runtime_error("SizeofResource returned 0");

    HGLOBAL hResData = LoadResource(hModule, hResInfo);
    if (!hResData) throw std::runtime_error("LoadResource failed");

    void* p = LockResource(hResData);
    if (!p) throw std::runtime_error("LockResource failed");

    return ResourceView{ p, static_cast<std::size_t>(size) };
}