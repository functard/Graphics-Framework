#pragma once
#include "Text2D.h"
class FPSDisplay2D :
    public Text2D
{
public:
    FPSDisplay2D(XMFLOAT2 _pos = XMFLOAT2(0, 0));

public:
    virtual void Update(float _deltaTime) override;
};

