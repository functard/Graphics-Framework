#pragma once
#include "TexturedEntity.h"
#include "Engine.h"

class Dice :
    public CTexturedEntity
{
public:
    Dice(std::wstring _path, EResourceType _type, XMFLOAT3 pos = XMFLOAT3(0, 0, 0));
};

