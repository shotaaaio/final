#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Obj.h"

// �c�^
class Trash : public Obj
{
public:
    Trash();
    ~Trash() override;

    // �X�V����
    void update(float elapsedTime) override;

    // �`�揈��
    void render(ID3D11DeviceContext* dc) override;

private:
    SkinnedMesh* model = nullptr;
};