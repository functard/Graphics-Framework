#pragma once
#include "TexturedEntity.h"
class Quad :
    public CTexturedEntity

{
public:
    Quad(std::wstring _path, EResourceType _type, float _heigth = 1, float _width = 1, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0), XMFLOAT3 _rotation = XMFLOAT3(0, 0, 0));
    
public:
    virtual void Update(float _deltaTime) override;
};