#pragma once
#include"Bindable.h"
class IndexBuffer :public Bindable{
public:
    virtual void Bind(Graphics& gfx) noexcept override;
    
};

