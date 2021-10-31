#include "InputLayout.h"
#include"GraphicsThrowMacros.h"
InputLayout::InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderBytecode) {
    INFOMAN(gfx);

    GFX_THROW_INFO( GetDevice(gfx)->CreateInputLayout(layout.data(),(UINT)layout.size(),pVertexShaderBytecode->GetBufferPointer(),pVertexShaderBytecode->GetBufferSize(),m_spInputLayout.ReleaseAndGetAddressOf()));
}

void InputLayout::Bind(Graphics& gf) noexcept {
    GetContext(gf)->IASetInputLayout(m_spInputLayout.Get());
}
