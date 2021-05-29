#pragma once
class CCamera
{
public:
	void Update(float _deltaTime, XMFLOAT3 _player, bool _isFirstLevel);

	inline XMFLOAT3 GetRotation() const { return m_rotation; }
	inline XMFLOAT3 GetPosition() const { return m_position; }
	inline void SetPos(XMFLOAT3 _pos) { m_position = _pos; }
	inline float GetFOV() const { return m_fov; }

private:
	XMFLOAT3 m_position = XMFLOAT3(0, 0, -20);
	XMFLOAT3 m_rotation = XMFLOAT3(0, 0, 0);

	float m_movementSpeed = 5;
	float m_rotationSpeed = 10;
	float m_fov = 90;
};

