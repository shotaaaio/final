#pragma once
#include"Graphics/Sprite.h"
#include"Scene.h"
#include<memory>
#include<thread>

//���[�f�B���O�V�[��
class LoadingScene : public Scene
{
public:
	LoadingScene(Scene*nextScene):nextScene(nextScene){}
	~LoadingScene()override {};

	//����������
	void initialize()override;

	//�I����
	void finalize()override;

	//�X�V����
	void update(float elapsedTime)override;

	//�`�揈��
	void render()override;
private:
	//�ׂ̃X���b�h�Ƃ��ċN��������֐�
	static void LoadingThread(LoadingScene* scene);
private:
	//���[�f�B���O���
	std::unique_ptr<Sprite>loadingImage;

	//��]�p�x
	float angle = 0.0f;

	//���̃V�[��
	Scene* nextScene = nullptr;

	//�}���`�p�X���b�h
	std::thread* thread = nullptr;
};