#pragma once
#include "Entity.h"
class Cube :
    public CEntity
{
public:
    Cube(XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

public:
    void Update(float _deltaTime) override;
};

