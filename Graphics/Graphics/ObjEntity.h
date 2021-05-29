#pragma once
#include "TexturedEntity.h"

#include <vector>

class ObjEntity :
    public CTexturedEntity
{
public:
    ObjEntity(std::vector<SVertexPosColor>* _vertices, std::vector<WORD>* _indices, 
        std::wstring _path, EResourceType _type, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));
};

