#pragma once
#include"Scene.h"
#include<memory>
#include "UIManager.h"

//�^�C�g���V�[��
class TitleScene :public Scene
{
public:
	TitleScene(){}
	~TitleScene()override{}

	//������
	void initialize() override;

	void finalize() override;

	//�X�V����
	void update(float elapsedTime)override;

	//�`�揈��
	void render()override;
private:
	//�^�C�g���摜
	std::unique_ptr<UIManager>m_UIManager;
};