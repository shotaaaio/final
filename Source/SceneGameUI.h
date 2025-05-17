#pragma once
#include "Player.h"
#include "UIComponent.h"

//HP�Q�[�W�pUI
class HPGaugeUI : public UIComponent
{
public:
    HPGaugeUI() = default;
    ~HPGaugeUI() override = default;

    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;

private:
    int m_currentHp = 0;
    int  m_maxHp = 0.0f;

    std::unique_ptr<Sprite> m_HP_barSprite;
    std::unique_ptr<Sprite> m_HP_frameSprite;
    std::unique_ptr<Player>player;
};

//���ԕ\���pUI
class TimeDisplayUI : public UIComponent
{
public:
    TimeDisplayUI() = default;
    ~TimeDisplayUI() override = default;

    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;

private:
    // ��Փx�ɂ�鎞�Ԑ���
    struct DifficultyTimeLimit
    {
        float easy;
        float normal;
        float hard;

        DifficultyTimeLimit()
        {
            easy = Time_limit * 10.0f; // 10��
            normal = Time_limit * 7.0f;  // 7��
            hard = Time_limit * 5.0f;  // 5��
        }
    };

    static constexpr float Time_limit = 60.0f;//1�����
    float m_totalTime = 0.0f;
    DifficultyTimeLimit m_timeLimit;

    std::unique_ptr<Sprite> m_Time_FrameSprite;
};

//�A�N�V�����{�^���pUI
class ActionButtonsUI : public UIComponent
{
public:
    ActionButtonsUI() = default;
    ~ActionButtonsUI() override = default;

    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;

private:
    enum class ButtonType { Attack, Jump, Dodge, Combo };
    std::vector<std::unique_ptr<Sprite>> m_buttonSprites;
};
