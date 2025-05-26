#pragma once
#include "Graphics/Sprite.h"
#include "Graphics/GraphicsManager.h"
#include "DeviceManager.h"
//���UI���ۃN���X
class UIComponent
{
public:
    UIComponent() {};
    virtual ~UIComponent() {};

    virtual void Initialize() = 0;
    virtual void Update(float elapsedTime) = 0;
    virtual void Render(ID3D11DeviceContext* dc) = 0;
};

