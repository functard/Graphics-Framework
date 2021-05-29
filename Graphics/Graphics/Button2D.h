#pragma once
#include "Image2D.h"

class Button2D;

typedef void(*BTNFUNCTION) (Button2D*);

class Button2D :
    public Image2D
{
public:
    Button2D(std::wstring _path, EResourceType _type, BTNFUNCTION _func,XMFLOAT2 _size, XMFLOAT2 _pos = XMFLOAT2(0, 0));

    inline void SetPos(XMFLOAT2 _pos) { m_position = _pos; }

public:
    virtual void Update(float _deltaTime) override;

protected:
    BTNFUNCTION m_function;
};

