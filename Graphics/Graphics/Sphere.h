#pragma once
#include "Entity.h"
class Sphere :
    public CEntity
{
public:
    Sphere(int _resolution, float _radius, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

public:
    inline float GetRadius() { return m_radius; }
    inline XMFLOAT3 GetCenter() { return m_position; }

protected:
    float m_radius;
public:
    virtual void Update(float _deltaTime) override;
};

