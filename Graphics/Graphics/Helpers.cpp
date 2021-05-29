#include "PCH.h"
#include "Helpers.h"
#include "Engine.h"

XMFLOAT2 operator+(XMFLOAT2 _lv, XMFLOAT2 _rv)
{
	return XMFLOAT2(_lv.x + _rv.x, _lv.y + _rv.y);
}

XMFLOAT2 operator-(XMFLOAT2 _lv, XMFLOAT2 _rv)
{
	return XMFLOAT2(_lv.x - _rv.x, _lv.y - _rv.y);
}

XMFLOAT2 operator*(XMFLOAT2 _lv, float _rv)
{
	return XMFLOAT2(_lv.x * _rv, _lv.y * _rv);
}

XMFLOAT2 operator/(XMFLOAT2 _lv, float _rv)
{
	return XMFLOAT2(_lv.x / _rv, _lv.y / _rv);
}

XMFLOAT3 operator+(XMFLOAT3 _lv, XMFLOAT3 _rv)
{
	return XMFLOAT3(_lv.x + _rv.x, _lv.y + _rv.y, _lv.z + _rv.z);
}

XMFLOAT3 operator-(XMFLOAT3 _lv, XMFLOAT3 _rv)
{
	return XMFLOAT3(_lv.x - _rv.x, _lv.y - _rv.y, _lv.z - _rv.z);
}

XMFLOAT3 operator*(XMFLOAT3 _lv, float _rv)
{
	return XMFLOAT3(_lv.x * _rv, _lv.y * _rv, _lv.z * _rv);
}

XMFLOAT3 operator/(XMFLOAT3 _lv, float _rv)
{
	return XMFLOAT3(_lv.x / _rv, _lv.y / _rv, _lv.z / _rv);
}

XMFLOAT4 operator+(XMFLOAT4 _lv, XMFLOAT4 _rv)
{
	return XMFLOAT4(_lv.x + _rv.x, _lv.y + _rv.y, _lv.z + _rv.z, _lv.w + _rv.w);
}

XMFLOAT4 operator-(XMFLOAT4 _lv, XMFLOAT4 _rv)
{
	return XMFLOAT4(_lv.x - _rv.x, _lv.y - _rv.y, _lv.z - _rv.z, _lv.w - _rv.w);
}

XMFLOAT4 operator*(XMFLOAT4 _lv, float _rv)
{
	return XMFLOAT4(_lv.x * _rv, _lv.y * _rv, _lv.z * _rv, _lv.w * _rv);
}

XMFLOAT4 operator/(XMFLOAT4 _lv, float _rv)
{
	return XMFLOAT4(_lv.x / _rv, _lv.y / _rv, _lv.z / _rv, _lv.w / _rv);
}

float Clamp(float _value, float _min, float _max)
{
	return _value < _min
		? _min
		: _value > _max
		? _max
		: _value;
}

XMFLOAT2 Clamp(XMFLOAT2 _value, XMFLOAT2 _min, XMFLOAT2 _max)
{
	return XMFLOAT2(Clamp(_value.x, _min.x, _max.x),
		Clamp(_value.y, _min.y, _max.y));
}

XMFLOAT3 Clamp(XMFLOAT3 _value, XMFLOAT3 _min, XMFLOAT3 _max)
{
	return XMFLOAT3(Clamp(_value.x, _min.x, _max.x),
		Clamp(_value.y, _min.y, _max.y),
		Clamp(_value.z, _min.z, _max.z));
}

float Magnitude(XMFLOAT2 _input)
{
	return sqrtf(_input.x * _input.x + _input.y * _input.y);
}

float Magnitude(XMFLOAT3 _input)
{
	return sqrtf(_input.x * _input.x + _input.y * _input.y + _input.z * _input.z);
}

XMFLOAT2 Normalize(XMFLOAT2 _input)
{
	float length = Magnitude(_input);
	if (length == 0)
		return _input;
	return _input / length;
}

XMFLOAT3 Normalize(XMFLOAT3 _input)
{
	float length = Magnitude(_input);
	if (length == 0)
		return _input;

	return _input / length;
}

bool PointInRect(XMFLOAT2 _point, RECT _rect)
{
	return (_point.x > _rect.left && _point.x < _rect.left + _rect.right
		&& _point.y > _rect.top && _point.y < _rect.top + _rect.bottom);
}

bool PointInRect(XMFLOAT2 _point, XMFLOAT2 _rectPos, XMFLOAT2 _extends)
{
	RECT rect = {};
	rect.left = _rectPos.x;
	rect.top = _rectPos.y;
	rect.right = _extends.x;
	rect.bottom = _extends.y;

	return PointInRect(_point, rect);
}

int CHelpers::WaitForFenceValue(ComPtr<ID3D12Fence> _fence,
	UINT64 _fenceValue, HANDLE _fenceEvent, std::chrono::milliseconds _duration)
{
	if (_fence->GetCompletedValue() < _fenceValue)
	{
		int value = _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);
		if (FAILED(value))
		{
			return -1;
		}
		::WaitForSingleObject(_fenceEvent, static_cast<DWORD>(_duration.count()));
	}
	return 0;
}

void CHelpers::UploadBufferResource(ComPtr<ID3D12GraphicsCommandList2> _commandList, ID3D12Resource** _destinationResource,
	ID3D12Resource** _intermediateResource, int _numElements, int _elementSize,
	const void* _bufferData, D3D12_RESOURCE_FLAGS _flags)
{
	int bufferSize = _numElements * _elementSize;
	int value;
	value = DXS.m_Device->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, _flags),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(_destinationResource)
	);
	if (FAILED(value))
	{
		throw std::exception("Buffer upload failed!");
	}

	if (nullptr != _bufferData)
	{
		value = DXS.m_Device->CreateCommittedResource
		(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(_intermediateResource)
		);

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = _bufferData;
		subresourceData.RowPitch = bufferSize;
		subresourceData.SlicePitch = subresourceData.RowPitch;

		UpdateSubresources(_commandList.Get(), *_destinationResource, *_intermediateResource,
			0, 0, 1, &subresourceData);
	}
}
