#pragma once
#include "Entity.h"
class Triangle :
    public CEntity
{
public:
    Triangle(XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

    void Update(float _deltaTime) override;
};

