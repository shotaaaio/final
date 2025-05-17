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
	void AddUI(std::unique_ptr<UI>ui);//“Á’è‚ÌUI‚¾‚¯‚ğŠO•”‚©‚ç“o˜^‚·‚éŠÖ”
	//ˆÈ‰º‚Ì‚æ‚¤‚É“o˜^
	//UIManager uiManager;
	//uiManager.AddUI(std::make_unique<HPGaugeUI>());
	//uiManager.AddUI(std::make_unique<TimeDisplayUI>());
	//uiManager.Initialize(); // ’Ç‰ÁŒã‚É‰Šú‰»
private:
	std::vector<std::unique_ptr<UI>>m_uiElements;
};