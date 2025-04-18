#pragma once

#include <Windows.h>

using MouseButton = unsigned int;

// �}�E�X
class Mouse
{
public:
	static const MouseButton BTN_LEFT = (1 << 0);
	static const MouseButton BTN_MIDDLE = (1 << 1);
	static const MouseButton BTN_RIGHT = (1 << 2);

public:
	Mouse(HWND hWnd);
	~Mouse() {}

	// �X�V
	void update();

	// �{�^�����͏�Ԃ̎擾
	MouseButton getButton() const { return buttonState[0]; }

	// �{�^��������Ԃ̎擾
	MouseButton getButtonDown() const { return buttonDown; }

	// �{�^�������Ԃ̎擾
	MouseButton getButtonUp() const { return buttonUp; }

	// �z�C�[���l�̐ݒ�
	void setWheel(int wheel) { this->wheel[0] += wheel; }

	// �z�C�[���l�̎擾
	int getWheel() const { return wheel[1]; }

	// �}�E�X�J�[�\��X���W�擾
	int getPositionX() const { return positionX[0]; }

	// �}�E�X�J�[�\��Y���W�擾
	int getPositionY() const { return positionY[0]; }

	// �O��̃}�E�X�J�[�\��X���W�擾
	int getPrevPositionX() const { return positionX[1]; }

	// �O��̃}�E�X�J�[�\��Y���W�擾
	int getPrevPositionY() const { return positionY[1]; }

	// �X�N���[�����ݒ�
	void setScreenWidth(int width) { screenWidth = width; }

	// �X�N���[�������ݒ�
	void setScreenHeight(int height) { screenHeight = height; }

	// �X�N���[�����擾
	int getScreenWidth() const { return screenWidth; }

	// �X�N���[�������擾
	int getScreenHeight() const { return screenHeight; }

private:
	MouseButton		buttonState[2] = { 0 };
	MouseButton		buttonDown = 0;
	MouseButton		buttonUp = 0;
	int				positionX[2];
	int				positionY[2];
	int				wheel[2];
	int				screenWidth = 0;
	int				screenHeight = 0;
	HWND			hwnd = nullptr;
};
