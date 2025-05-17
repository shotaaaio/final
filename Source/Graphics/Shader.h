#pragma once
#include "Graphics/Sprite.h"
#include "Graphics/SkinnedMesh.h"
struct  RenderContext;

class ModelShader
{
    virtual ~ModelShader() {}

    /// <summary>
  /// 描画開始前に呼び出す。シェーダ・ステートのバインドなどを行う。
  /// </summary>
  /// <param name="immediate_context">デバイスコンテキスト</param>
    virtual void Begin(ID3D11DeviceContext* immediate_context) = 0;

    /// <summary>
    /// スプライトを描画する。
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    /// <param name="rc">レンダーコンテキスト</param>
    /// <param name="sprite">描画するスプライト</param>
    virtual void Draw(
        ID3D11DeviceContext* immediate_context,
        const RenderContext& rc,
        const SkinnedMesh* skinnedMesh
    ) = 0;

    /// <summary>
    /// 描画後に呼び出す。シェーダ・ステートの解除などを行う。
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    virtual void End(ID3D11DeviceContext* immediate_context) = 0;

};

class SpriteShader
{
public:
    virtual ~SpriteShader() = default;

    /// <summary>
    /// 描画開始前に呼び出す。シェーダ・ステートのバインドなどを行う。
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    virtual void Begin(ID3D11DeviceContext* immediate_context) = 0;

    /// <summary>
    /// スプライトを描画する。
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    /// <param name="rc">レンダーコンテキスト</param>
    /// <param name="sprite">描画するスプライト</param>
    virtual void Draw(
        ID3D11DeviceContext* immediate_context,
        const RenderContext& rc,
        const Sprite* sprite
    ) = 0;

    /// <summary>
    /// 描画後に呼び出す。シェーダ・ステートの解除などを行う。
    /// </summary>
    /// <param name="immediate_context">デバイスコンテキスト</param>
    virtual void End(ID3D11DeviceContext* immediate_context) = 0;
};

