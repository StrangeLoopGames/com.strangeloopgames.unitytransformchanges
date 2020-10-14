// RuntimeGraphicsInfo.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "RuntimeGraphicsInfo.h"

#include "IUnityInterface.h"
#include "IUnityGraphics.h"

static void UNITY_INTERFACE_API
OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

RunTimeGraphicsMemoryInfo GetDeviceStatsD3D11(IUnityInterfaces* pUnityInterface);
RunTimeGraphicsMemoryInfo GetDeviceStatsMetal(IUnityInterfaces* pUnityInterface);
unsigned short SetMaxTessellationFactorMetal(IUnityInterfaces* pUnityInterface, unsigned short tessellationLevel);


static IUnityInterfaces* s_UnityInterfaces = nullptr;
static IUnityGraphics* s_Graphics = nullptr;
static UnityGfxRenderer s_RendererType = UnityGfxRenderer::kUnityGfxRendererNull;
static RunTimeGraphicsMemoryInfo s_Stats;

// Unity plugin load event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces * unityInterfaces)
{
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = unityInterfaces->Get<IUnityGraphics>();

    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

    // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
    // to not miss the event in case the graphics device is already initialized
    OnGraphicsDeviceEvent(UnityGfxDeviceEventType::kUnityGfxDeviceEventInitialize);
}

// Unity plugin unload event
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginUnload()
{
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

RunTimeGraphicsMemoryInfo GetStatsForDevice(UnityGfxRenderer renderer)
{
    switch (renderer)
    {
    case UnityGfxRenderer::kUnityGfxRendererD3D11:
        return GetDeviceStatsD3D11(s_UnityInterfaces);
    case UnityGfxRenderer::kUnityGfxRendererMetal:
        return GetDeviceStatsMetal(s_UnityInterfaces);
    case UnityGfxRenderer::kUnityGfxRendererVulkan:
        return {};
    default:
        return {};
    }
}

unsigned short SetMaxTessellationFactor(UnityGfxRenderer renderer, unsigned short factor)
{
    switch (renderer)
    {
        case UnityGfxRenderer::kUnityGfxRendererMetal:
            return SetMaxTessellationFactorMetal(s_UnityInterfaces, factor);
        default:
            return s_Stats.MaxTessellationFactor;
    }
}

static void UNITY_INTERFACE_API
OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    switch (eventType)
    {
    case UnityGfxDeviceEventType::kUnityGfxDeviceEventInitialize:
    {
        s_RendererType = s_Graphics->GetRenderer();

        s_Stats = GetStatsForDevice(s_RendererType);

        break;
    }
    case UnityGfxDeviceEventType::kUnityGfxDeviceEventShutdown:
    {
        s_RendererType = UnityGfxRenderer::kUnityGfxRendererNull;
        //TODO: user shutdown code
        break;
    }
    case UnityGfxDeviceEventType::kUnityGfxDeviceEventBeforeReset:
    {
        //TODO: user Direct3D 9 code
        break;
    }
    case UnityGfxDeviceEventType::kUnityGfxDeviceEventAfterReset:
    {
        //TODO: user Direct3D 9 code
        break;
    }
    }
}

extern "C" uint16_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetMaxTessellationFactor(uint16_t factor)
{
    return SetMaxTessellationFactor(s_RendererType, factor);
}

extern "C" uint64_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetDedicatedVideoMemory()
{
    return s_Stats.DedicatedVideoMemory;
}

extern "C" uint64_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetDedicatedSystemMemory()
{
    return s_Stats.DedicatedSystemMemory;
}

extern "C" uint64_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetSharedSystemMemory()
{
    return s_Stats.SharedSystemMemory;
}

extern "C" uint64_t UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetMaxTessellationFactor()
{
    return s_Stats.MaxTessellationFactor;
}
