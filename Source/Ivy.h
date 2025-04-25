#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Obj.h"

// �c�^
class Ivy : public Obj
{
public:
    Ivy();
    ~Ivy() override;

    // �X�V����
    void update(float elapsedTime) override;

    // �`�揈��
    void render(ID3D11DeviceContext* dc) override;

private:
    SkinnedMesh* model = nullptr;
};