#pragma once
#include "Bindable.h"
class InputLayout :public Bindable{
public:
    InputLayout(Graphics& gfx,
                const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                ID3DBlob* pVertexShaderBytecode);
    virtual~InputLayout() = default;
    virtual void Bind(Graphics& gf) noexcept override;

private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_spInputLayout;
};

