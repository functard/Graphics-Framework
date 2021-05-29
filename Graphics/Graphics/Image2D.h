#pragma once
#include "Entity2D.h"
#include "DirectXSettings.h"
#include <string>

class Image2D :
    public CEntity2D
{
public:
    Image2D(std::wstring _path, EResourceType _type, float _width, float _heigth, XMFLOAT2 _pos = XMFLOAT2(0, 0));
    ~Image2D();

    inline void SetPos(XMFLOAT2 _pos) { m_position = _pos; }

public:
    bool Init() override;
    void Render() override;

protected:
    std::wstring m_path;
    EResourceType m_type;
    ComPtr<ID3D12Resource> m_texture;
    RECT m_sourceRect;
    XMFLOAT2 m_origin = XMFLOAT2(0, 0);
    float m_width;
    float m_heigth;
};

