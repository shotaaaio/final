#pragma once
#include "Graphics/Sprite.h"
#include "Graphics/SkinnedMesh.h"
struct  RenderContext;

class ModelShader
{
    virtual ~ModelShader() {}

    /// <summary>
  /// �`��J�n�O�ɌĂяo���B�V�F�[�_�E�X�e�[�g�̃o�C���h�Ȃǂ��s���B
  /// </summary>
  /// <param name="immediate_context">�f�o�C�X�R���e�L�X�g</param>
    virtual void Begin(ID3D11DeviceContext* immediate_context) = 0;

    /// <summary>
    /// �X�v���C�g��`�悷��B
    /// </summary>
    /// <param name="immediate_context">�f�o�C�X�R���e�L�X�g</param>
    /// <param name="rc">�����_�[�R���e�L�X�g</param>
    /// <param name="sprite">�`�悷��X�v���C�g</param>
    virtual void Draw(
        ID3D11DeviceContext* immediate_context,
        const RenderContext& rc,
        const SkinnedMesh* skinnedMesh
    ) = 0;

    /// <summary>
    /// �`���ɌĂяo���B�V�F�[�_�E�X�e�[�g�̉����Ȃǂ��s���B
    /// </summary>
    /// <param name="immediate_context">�f�o�C�X�R���e�L�X�g</param>
    virtual void End(ID3D11DeviceContext* immediate_context) = 0;

};

class SpriteShader
{
public:
    virtual ~SpriteShader() = default;

    /// <summary>
    /// �`��J�n�O�ɌĂяo���B�V�F�[�_�E�X�e�[�g�̃o�C���h�Ȃǂ��s���B
    /// </summary>
    /// <param name="immediate_context">�f�o�C�X�R���e�L�X�g</param>
    virtual void Begin(ID3D11DeviceContext* immediate_context) = 0;

    /// <summary>
    /// �X�v���C�g��`�悷��B
    /// </summary>
    /// <param name="immediate_context">�f�o�C�X�R���e�L�X�g</param>
    /// <param name="rc">�����_�[�R���e�L�X�g</param>
    /// <param name="sprite">�`�悷��X�v���C�g</param>
    virtual void Draw(
        ID3D11DeviceContext* immediate_context,
        const RenderContext& rc,
        const Sprite* sprite
    ) = 0;

    /// <summary>
    /// �`���ɌĂяo���B�V�F�[�_�E�X�e�[�g�̉����Ȃǂ��s���B
    /// </summary>
    /// <param name="immediate_context">�f�o�C�X�R���e�L�X�g</param>
    virtual void End(ID3D11DeviceContext* immediate_context) = 0;
};

