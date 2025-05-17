#pragma once
#include "UIComponent.h"

class UIManager
{
public:
	UIManager(){}
	~UIManager(){}

	void Initialize();
	void Update(float elapsedTime);
	void Render(ID3D11DeviceContext* dc);
	void AddUI(std::unique_ptr<UI>ui);//�����UI�������O������o�^����֐�
	//�ȉ��̂悤�ɓo�^
	//UIManager uiManager;
	//uiManager.AddUI(std::make_unique<HPGaugeUI>());
	//uiManager.AddUI(std::make_unique<TimeDisplayUI>());
	//uiManager.Initialize(); // �ǉ���ɏ�����
private:
	std::vector<std::unique_ptr<UI>>m_uiElements;
};