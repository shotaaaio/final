#pragma once
#include "Enemy.h"
#include <unordered_map>
#include <mutex>

class OcclusionQuery {
public:
	HRESULT BeginQuery(Enemy* enemy, ID3D11DeviceContext* dc);   //�s�N�Z������J�n
    HRESULT EndQuery(Enemy* enemy, ID3D11DeviceContext* context);//�s�N�Z������I��
    void    CheckPixelCount(ID3D11DeviceContext* dc);            //�s�N�Z�����擾
	void    CheckCountTimer(float elapsedTime);                  //�^�C���J�E���^�[
	void    DrawImGui(); 						                 //�f�o�b�O���

private:
    struct QueryData {
        Microsoft::WRL::ComPtr<ID3D11Query> query;               //�N�G���@�\
        std::atomic<UINT64>pixelCount{ 0 };                      //�`��s�N�Z����
        std::atomic<bool>  started{ false };                     //�N�G���̊J�n���
        std::atomic<bool>  ready{ false };                       //���ʎ擾�\���
        std::atomic<bool>  is_hide{ true };                      //�����t���O
        std::atomic<float> hideTime{ 0.0f };                     //�G����ʓ��������������
		std::atomic<float> hideTimeThereshold{ 5.0f };           //�G����ʓ�������������Ԃ̂������l
    };

    // �f�o�C�X�ƃN�G���f�[�^
	std::mutex mutex;                                            //�X���b�h�Z�[�t�p�~���[�e�b�N�X
    std::unordered_map<Enemy*,QueryData>queries;                 //�Օ��N�G�����`�F�b�N����}�b�v          [�L�[:�I�u�W�F�N�g�|�C���^�A�l:�f�[�^]
    std::unordered_map<Enemy*,UINT64>   recentPixels;            //�ŋߎՕ��N�G�����ʂ��L���b�V������}�b�v[�L�[:�I�u�W�F�N�g�|�C���^�A�l:���ߕ`��s�N�Z����(��������Ɏg��)]
};