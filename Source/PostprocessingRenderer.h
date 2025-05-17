//#pragma once
//
//#include "Graphics/ShaderManager.h"
//#include "Graphics/Sprite.h"
//#include "Graphics/GraphicsManager.h"
//#include "LuminanceExtractionShader.h"
//#include "RenderTarget.h"
//
//
//// �V�F�[�_�[���\�[�X�r���[�̏��
//struct ShaderResourceViewData
//{
//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>srv;
//	int	width  = 0;
//	int height = 0;
//};
//
////�|�X�g�v���Z�X�p�̃����_���[
//class PostprosessingRenderer
//{
//	//�u���[�����
//	struct BloomData
//	{
//		//	���P�x���o�p���
//		LumianceExtractionData	luminanceExtractionData;
//	};
//public:
//	PostprosessingRenderer();
//	~PostprosessingRenderer() {};
//
//	//	�`��
//	void Render(ID3D11DeviceContext* dec);
//
//	//	�V�[�����̐ݒ�
//	void SetSceneData(ShaderResourceViewData srvData);
//
//	//	�f�o�b�O���̕\��
//	void DrawDebugGUI();
//private:
//	// �V�[���`��f�[�^
//	ShaderResourceViewData	sceneData;
//
//	// �`��p�X�v���C�g
//	std::unique_ptr<Sprite>			renderSprite;
//
//	// �u���[���f�[�^
//	BloomData						bloomData;
//
//	//	���P�x���o�e�N�X�`��
//	std::unique_ptr<RenderTarget>	luminanceExtractRenderTarget;
//
//};
//
