#pragma once
#include "Entity2D.h"

#include <string>

class Text2D :
    public CEntity2D
{
public:
    Text2D(std::string _text, XMFLOAT2 _pos = XMFLOAT2(0, 0));

public:
    virtual void Render() override;

    void SetPos(XMFLOAT2 _pos);

protected:
    std::string m_text;
};

