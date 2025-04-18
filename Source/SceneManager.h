#pragma once

#include"Scene.h"

//�V�[���Ǘ�
class SceneManager
{
private:
	SceneManager(){}
	~SceneManager(){}
public:
	static SceneManager* instance()
	{
		static SceneManager inst;
		return &inst;
	}

	//�X�V����
	void update(float elapsedTime);

	//�`�揈��
	void render();

	//���݂̃V�[�����N���A
	void clear();

	//�V�[���̐؂�ւ��i�O���Ő��������V�[�������̃V�[���Ƃ��Đݒ�j
	void changeScene(Scene* scene);
private:
	//���݂̃V�[��
	Scene* currentScene = nullptr;

	//���̃V�[��
	Scene* nextScene = nullptr;
};