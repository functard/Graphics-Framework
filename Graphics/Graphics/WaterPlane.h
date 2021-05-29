#pragma once
#include "TexturedEntity.h"

class WaterPlane : public CTexturedEntity
{
public:
    WaterPlane(int _resolution, XMFLOAT3 _scale, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

public:
    virtual bool Init() override;
    virtual void Update(float _deltaTime) override;
    virtual void Render() override;

protected:
    ComPtr<ID3D12Resource> m_heightMap;
    ComPtr<ID3D12Resource> m_normalMap;
};

