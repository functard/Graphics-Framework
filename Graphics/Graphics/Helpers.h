#pragma once

XMFLOAT2 operator+(XMFLOAT2 _lv, XMFLOAT2 _rv);
XMFLOAT2 operator-(XMFLOAT2 _lv, XMFLOAT2 _rv);

XMFLOAT2 operator*(XMFLOAT2 _lv, float _rv);
XMFLOAT2 operator/(XMFLOAT2 _lv, float _rv);

XMFLOAT3 operator+(XMFLOAT3 _lv, XMFLOAT3 _rv);
XMFLOAT3 operator-(XMFLOAT3 _lv, XMFLOAT3 _rv);

XMFLOAT3 operator*(XMFLOAT3 _lv, float _rv);
XMFLOAT3 operator/(XMFLOAT3 _lv, float _rv);

XMFLOAT4 operator+(XMFLOAT4 _lv, XMFLOAT4 _rv);
XMFLOAT4 operator-(XMFLOAT4 _lv, XMFLOAT4 _rv);

XMFLOAT4 operator*(XMFLOAT4 _lv, float _rv);
XMFLOAT4 operator/(XMFLOAT4 _lv, float _rv);

float Clamp(float _value, float _min, float _max);
XMFLOAT2 Clamp(XMFLOAT2 _value, XMFLOAT2 _min, XMFLOAT2 _max);
XMFLOAT3 Clamp(XMFLOAT3 _value, XMFLOAT3 _min, XMFLOAT3 _max);

float Magnitude(XMFLOAT2 _input);
float Magnitude(XMFLOAT3 _input);

XMFLOAT2 Normalize(XMFLOAT2 _input);
XMFLOAT3 Normalize(XMFLOAT3 _input);

bool PointInRect(XMFLOAT2 _point, RECT _rect);
bool PointInRect(XMFLOAT2 _point, XMFLOAT2 _rectPos, XMFLOAT2 _extends);

class CHelpers
{
public:
	/**
	 * \brief					Holds the thread, until the GPU reached the \c _fence with \c _fenceValue.
	 *							When the fence has reached the value, the resources occupied by it can be reused again.
	 * \param[in]	_fence		The fence which will get the value.
	 * \param[in]	_fenceValue The value to wait for.
	 * \param[in]	_fenceEvent	Which event should be raised, when the fence reached the value.
	 * \param[in]	_duration	How long should be waited [maximum]. Default is <tt>std::chrono::milliseconds::max()</tt>.
	 * \returns					0 if successful else -1.
	 */
	static int WaitForFenceValue(ComPtr<ID3D12Fence> _fence, UINT64 _fenceValue, HANDLE _fenceEvent,
		std::chrono::milliseconds _duration = std::chrono::milliseconds::max());

	/**
	 * \brief								Creates a resources and updates it on the GPU.
	 * \param[in]	_commandList			The CommandList to use for this operation (Direct/Copy).
	 * \param[out]	_destination			The created resource will be placed here.
	 * \param[out]	_intermediateResource	The intermediate resource will be placed here.
	 * \param[in]	_numElements			How many elements are in the buffer.
	 * \param[in]	_elementSize			How big is a single element in the buffer.
	 * \param[in]	_bufferData				The data to upload.
	 * \param[in]	_flags					Optional flags for the resource. Default is \c D3D12_RESOURCE_FLAG_NONE.
	 */
	static void UploadBufferResource(ComPtr<ID3D12GraphicsCommandList2> _commandList,
		ID3D12Resource** _destinationResource, ID3D12Resource** _intermediateResource,
		int _numElements, int _elementSize, const void* _bufferData,
		D3D12_RESOURCE_FLAGS _flags = D3D12_RESOURCE_FLAG_NONE);
};

