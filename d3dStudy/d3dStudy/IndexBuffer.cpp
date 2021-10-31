#include "IndexBuffer.h"
#include"GraphicsThrowMacros.h"
IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices):m_count (indices.size()) {
    INFOMAN(gfx);   
    
    D3D11_BUFFER_DESC ibd = {0 };
    ibd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0U;
    ibd.MiscFlags = 0;
    ibd.ByteWidth = static_cast<UINT>(m_count*sizeof(unsigned short));
    ibd.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA isd = {0};
    isd.pSysMem = indices.data();
    GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &m_spIndexBuffer));
}

void IndexBuffer::Bind(Graphics& gfx) noexcept {
    GetContext(gfx)->IASetIndexBuffer(m_spIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept {
    return m_count;
}
