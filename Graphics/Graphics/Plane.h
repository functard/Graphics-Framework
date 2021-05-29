#pragma once
#include "TexturedEntity.h"
class Plane :
    public CTexturedEntity
{
public:
    Plane(int _resolution, XMFLOAT3 _scale, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

public:
    virtual bool Init() override;
    virtual void Render() override;

protected:
#pragma region ---Heightmap Blending---
    /*
    ComPtr<ID3D12Resource> m_stoneTexture;
    ComPtr<ID3D12Resource> m_stoneHeightmap;
    ComPtr<ID3D12Resource> m_sandTexture;
    ComPtr<ID3D12Resource> m_sandHeightmap;
    */
#pragma endregion
    ComPtr<ID3D12Resource> m_splatmapPixelTexture;
    ComPtr<ID3D12Resource> m_splatmapVertexTexture;
    ComPtr<ID3D12Resource> m_atlasTexture;
};

