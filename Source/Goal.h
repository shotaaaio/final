#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Obj.h"

class Goal : public Obj
{
public:
    Goal();
    ~Goal() override;

    // XVˆ—
    void update(float elapsedTime) override;

    // •`‰æˆ—
    void render(ID3D11DeviceContext* dc) override;

private:
    SkinnedMesh* model = nullptr;
};