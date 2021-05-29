#pragma once
#include "Image2D.h"
class Cursor2D :
    public Image2D
{
public:
    Cursor2D(std::wstring _path, XMFLOAT2 _size);

public:
    virtual bool Init() override;
    virtual void Update(float _deltaTime) override;
};

