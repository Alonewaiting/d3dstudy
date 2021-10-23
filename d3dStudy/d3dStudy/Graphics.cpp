#include "Graphics.h"
#include "dxerror.h"
#include <sstream>

#pragma comment(lib,"d3d11.lib")
#define GFX_THROW_FAILED(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )
// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#endif
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )

Graphics::Graphics(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = {0};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator =0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count =1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    UINT swapCreateFlags = 0u;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    HRESULT hr;

    GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        swapCreateFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &pSwap,
        &pDevice,
        nullptr,
        &pContext
    ));

    ID3D11Resource* pBackBuffer = nullptr;
    GFX_THROW_INFO(pSwap->GetBuffer(1,__uuidof(ID3D11Resource),reinterpret_cast<void**>(&pBackBuffer)));
    if (pBackBuffer) {
        GFX_THROW_INFO( pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget) );
    }
    pBackBuffer->Release();
}

Graphics::~Graphics() {
    if (pContext != nullptr) {
        pContext->Release();
    }
    if (pSwap != nullptr) {
        pSwap->Release();
    }
    if (pDevice != nullptr) {
        pDevice->Release();
    }
    if (pTarget) {
        pTarget->Release();
    }
}

void Graphics::EndFrame() {
    HRESULT hr;
#ifndef NDEBUG
    infoManager.Set();
#endif
    if (FAILED(hr = pSwap->Present(1u, 0u))) {
        if (hr == DXGI_ERROR_DEVICE_REMOVED) {
            throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
        }
        else {
            throw GFX_EXCEPT(hr);
        }
    }
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept {
    const float color[] = { red,green,blue,1.0f };
    if (pContext && pTarget) {
        pContext->ClearRenderTargetView(pTarget, color);
    }
}

// Graphics exception stuff
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
    :
    Exception(line, file),
    hr(hr) {
    // join all info messages with newlines into single string
    for (const auto& m : infoMsgs) {
        info += m;
        info.push_back('\n');
    }
    // remove final newline if exists
    if (!info.empty()) {
        info.pop_back();
    }
}

const char* Graphics::HrException::what() const noexcept {
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
        << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Error String] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorDescription() << std::endl;
    if (!info.empty()) {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    }
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept {
    return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept {
    return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept {
    return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept {
    char buf[512];
    DXGetErrorDescription(hr, buf, sizeof(buf));
    return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept {
    return info;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept {
    return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
