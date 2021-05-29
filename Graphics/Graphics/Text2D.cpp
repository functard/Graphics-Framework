#include "PCH.h"
#include "Text2D.h"
#include "Engine.h"

Text2D::Text2D(std::string _text, XMFLOAT2 _pos)
	: CEntity2D(_pos)
{
	m_text = _text;
}

void Text2D::Render()
{
	XMVECTOR size = DXS.m_SpriteFont->MeasureString(m_text.c_str(), false) * 0.5f;
	
	const XMFLOAT2 origin = XMFLOAT2(size.m128_f32[0], size.m128_f32[1]);

	DXS.m_SpriteFont->DrawString
	(
		DXS.m_SpriteBatch.get(),
		m_text.c_str(),
		m_position,
		Colors::White,
		XMConvertToRadians(m_rotation),
		origin
	);
}

void Text2D::SetPos(XMFLOAT2 _pos)
{
	m_position = _pos;
}
