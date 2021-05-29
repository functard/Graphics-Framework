#pragma once

struct SLightConstantBuffer
{
	XMFLOAT4 m_AmbientLightColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	XMFLOAT4 m_DirectionLightColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	XMFLOAT3 m_DirectionLightDir = XMFLOAT3(1, -1, 1.0f);
	float m_padding;
	XMFLOAT4 m_SpecularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT3 m_CameraPosition;
};