#pragma once
#include "UIComponent.h"
#include <functional>

//��ʏ��
enum class MenuType
{
    MAIN_MENU,            //���C�����j���[���
    SELECT_DIFFICULTY,    //��Փx�I�����
    SETTINGS,             //�ݒ���
    EXIT                  //�I������
};
//���j���[���ڂ�\���\����
struct Menu
{
    std::function<void()>SelectCallback;//�I�������肳�ꂽ�������p�̃R�[���o�b�N�֐�

    //�X�v���C�g�`��ʒu
	DirectX::XMFLOAT2 position;       //���݂̈ʒu
	DirectX::XMFLOAT2 targetPosition; //�A�j���[�V�����̖ڕW�ʒu
    float animationSpeed;//px/�b
    bool is_Animating;//
};

//�^�C�g���pUI
class TitleUI : public UIComponent
{
public:
    TitleUI() {};
    ~TitleUI() override {};

    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;

private:
    //�^�C�g�����S�摜
    std::unique_ptr<Sprite> m_TitleLogo;
 
    MenuType m_State = MenuType::MAIN_MENU; //���
    int m_selectIndex; //���j���[���őI������Ă���C���f�b�N�X

    // ���j���[�p�X�v���C�g�Q
    std::vector<std::unique_ptr<Sprite>> m_MainMenuSprites;
    std::vector<std::unique_ptr<Sprite>> m_DifficultyMenuSprites;
    std::vector<std::unique_ptr<Sprite>> m_SettingsMenuSprites;

    std::vector<Menu> m_MainMenu;          // ���C�����j���[�p
    std::vector<Menu> m_DifficultyMenu;   // ��Փx���j���[�p
    std::vector<Menu> m_SettingsMenu;     // �ݒ胁�j���[�p
    std::vector<Menu> m_CurrentMenu;       // ���ݕ\�����̃��j���[

private:
    //�������֐��֌W
    void SetupMainMenuCallbacks(); //���C�����j���[����̊e�I���V�[���̃R�[���o�b�N�֐�
    void BuildDifficultyMenu(); //��Փx�I�����j���[
    void BuildSettingsMenu();  //�ݒ胁�j���[
	void SetupMenus(); //���j���[�̏�����

    //�����_�����O�����֌W
    void RenderTitleLogo(ID3D11DeviceContext* dc);
	void RenderMainMenu(ID3D11DeviceContext* dc);      //���C�����j���[�̕`��
	void RenderDifficultyMenu(ID3D11DeviceContext* dc);//��Փx���j���[�̕`��
	void RenderSettingsMenu(ID3D11DeviceContext* dc);  //�ݒ胁�j���[�̕`��
	//���j���[�̕`��
    void RenderMenu(ID3D11DeviceContext* dc,
        const std::vector<Menu>& menu,
        const std::vector<std::unique_ptr<Sprite>>& sprites,
        const std::vector<float>& posXList,
        const std::vector<float>& posYList);

    //���j���[�̃A�j���[�V�����J�n�ݒ�
    void  StartMenuSlideIn(std::vector<Menu>& menuList, float startX, float targetX);
};