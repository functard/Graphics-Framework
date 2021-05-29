#include "PCH.h"
#include "Camera.h"
#include "Engine.h"
#include "Helpers.h"

void CCamera::Update(float _deltaTime, XMFLOAT3 _player, bool _isFirstLevel)
{
	if (_isFirstLevel)
	{
		m_position.x = _player.x;
		m_position.y = _player.y;
		m_position.z = _player.z - 65;
	}
	else
	{
		XMVECTOR x = { 1, 0, 0, 0 };
		XMVECTOR y = { 0, 1, 0, 0 };
		XMVECTOR z = { 0, 0, 1, 0 };

		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw
		(
			XMConvertToRadians(m_rotation.x),
			XMConvertToRadians(m_rotation.y),
			XMConvertToRadians(m_rotation.z)
		);

		x = XMVector3Transform(x, rotationMatrix);
		y = XMVector3Transform(y, rotationMatrix);
		z = XMVector3Transform(z, rotationMatrix);

		XMFLOAT3 forward = XMFLOAT3(z.m128_f32[0], z.m128_f32[1], z.m128_f32[2]);
		XMFLOAT3 right = XMFLOAT3(x.m128_f32[0], x.m128_f32[1], x.m128_f32[2]);
		XMFLOAT3 up = XMFLOAT3(y.m128_f32[0], y.m128_f32[1], y.m128_f32[2]);

		if (IPM.GetKey(DIK_W))
		{
			m_position = m_position + (forward * _deltaTime * m_movementSpeed);
		}
		if (IPM.GetKey(DIK_S))
		{
			m_position = m_position - (forward * _deltaTime * m_movementSpeed);
		}
		if (IPM.GetKey(DIK_A))
		{
			m_position = m_position - (right * _deltaTime * m_movementSpeed);
		}
		if (IPM.GetKey(DIK_D))
		{
			m_position = m_position + (right * _deltaTime * m_movementSpeed);
		}
		if (IPM.GetKey(DIK_Q))
		{
			m_position = m_position + (up * _deltaTime * m_movementSpeed);
		}
		if (IPM.GetKey(DIK_E))
		{
			m_position = m_position - (up * _deltaTime * m_movementSpeed);
		}

		m_movementSpeed += IPM.GetMouseWheel() * 0.01f;

		if (m_movementSpeed <= 1)
			m_movementSpeed = 1;
		
		XMFLOAT2 rotation = IPM.GetMouseMovement();

		m_rotation.x += rotation.y * _deltaTime * m_rotationSpeed;
		m_rotation.y += rotation.x * _deltaTime * m_rotationSpeed;
		
	}

	
}
