#pragma once
#include "Entity.h"
#include "Engine.h"

#include <string>

class CTexturedEntity :
    public CEntity
{
public:
    CTexturedEntity(std::wstring _path, EResourceType _type, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

public:
    virtual bool Init() override;
    virtual void Render() override;

protected:
    std::wstring m_path;
    EResourceType m_type;
    ComPtr<ID3D12Resource> m_texture;
};

