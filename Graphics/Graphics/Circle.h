#pragma once
#include "Entity.h"
#include "Helpers.h"
class Circle :
    public CEntity
{
public:
    Circle(int _resolution, float _radius, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

public:
    //virtual void Update(float _deltaTime) override;

    inline const float GetRadius() { return m_radius; }
    /*
    inline const XMFLOAT3 GetCenter() { return m_position; }
    inline void SetPos(XMFLOAT3 _pos) { m_position = _pos; }
    inline void SetRot(XMFLOAT3 _rot) { m_rotation = _rot; }
    inline void AddToRotation(XMFLOAT3 _value) { m_rotation = m_rotation + _value; }
    inline void AddToPos(XMFLOAT3 _value) { m_position = m_position + _value; }*/

    bool inBlackHole;
protected:
    float m_radius;
    XMFLOAT3 forward;
    XMFLOAT3 right;
    XMFLOAT3 up;




};

