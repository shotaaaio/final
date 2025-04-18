#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>

// �f�o�C�X�Ǘ�
class DeviceManager
{
private:
	DeviceManager() : screenWidth(0), screenHeight(0){}
	~DeviceManager() {}

public:
	static DeviceManager* instance()
	{
		static DeviceManager inst;
		return &inst;
	}

	// ����������
	DeviceManager* initialize(HWND hwnd);

	// �f�o�C�X�擾
	ID3D11Device* getDevice() const { return device.Get(); }

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* getDeviceContext() const { return immediateContext.Get(); }

	// �X���b�v�`�F�[���擾
	IDXGISwapChain* getSwapChain() const { return swapchain.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* getRenderTargetView() const { return renderTargetView.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* getDepthStencilView() const { return depthStencilView.Get(); }

	//// �V�F�[�_�[�擾
	//Shader* GetShader() const { return shader.get(); }

	// �X�N���[�����擾
	float getScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	float getScreenHeight() const { return screenHeight; }

	// �r������
	std::mutex& getMutex() { return mutex; }

private:
	float	screenWidth;
	float	screenHeight;

	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

	std::mutex mutex;
};