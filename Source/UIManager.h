#pragma once
#include "UIComponent.h"

class UIManager
{
public:
	UIManager(){}
	~UIManager(){}

	void Initialize();                         //������
	void Update(float elapsedTime);            //�X�V
	void Render(ID3D11DeviceContext* dc);      //�`��
	void AddUI(std::shared_ptr<UIComponent>ui);//�eUI���O������o�^����֐�

private:
	//�e���ۃN���X�̐����擾����ׂ̃����o�ϐ�
	std::vector<std::shared_ptr<UIComponent>>m_uiElements;
};