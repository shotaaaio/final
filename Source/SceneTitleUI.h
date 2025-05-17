#pragma once
#include "UIComponent.h"

//タイトルロゴ用UI
class TitleLogo : public UIComponent
{
public:
    TitleLogo() = default;
    ~TitleLogo() override = default;

    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;

private:
    std::unique_ptr<Sprite> m_TitleLogo;
};