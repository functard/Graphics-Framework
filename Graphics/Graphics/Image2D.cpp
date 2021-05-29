#include "PCH.h"
#include "Image2D.h"
#include "Engine.h"

Image2D::Image2D(std::wstring _path, EResourceType _type, float _width, float _heigth, XMFLOAT2 _pos)
	: CEntity2D(_pos)
{
	m_path = _path;
	m_type = _type;
	m_width = _width;
	m_heigth = _heigth;
}

Image2D::~Image2D()
{
}

bool Image2D::Init()
{
	int value;

	ResourceUploadBatch uploadBatch(DXS.m_Device.Get());
	uploadBatch.Begin();

	value = ::CreateWICTextureFromFile(DXS.m_Device.Get(), uploadBatch,
		m_path.c_str(), &m_texture);
	if (FAILED(value))
	{
		return false;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle
	{
		DXS.m_ResourceDescriptors->GetCPUDescriptorHandleForHeapStart(),
		(int)m_type,
		DXS.m_CBVDescriptorSize
	};

	::CreateShaderResourceView(DXS.m_Device.Get(), m_texture.Get(), cpuHandle);

	auto uploadFinish = uploadBatch.End(DXS.m_DirectCommandQueue.Get());
	uploadFinish.wait();

	D3D12_RESOURCE_DESC texDesc = m_texture->GetDesc();/*
	m_size.x = texDesc.Width;
	m_size.y = texDesc.Height;*/

	m_size.x = m_width;
	m_size.y = m_heigth;
	m_sourceRect.left = 0;
	m_sourceRect.top = 0;
	m_sourceRect.right = m_size.x;
	m_sourceRect.bottom = m_size.y;

	return true;
}

void Image2D::Render()
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle
	{
		DXS.m_ResourceDescriptors->GetGPUDescriptorHandleForHeapStart(),
		(int)m_type,
		DXS.m_CBVDescriptorSize
	};

	DXS.m_SpriteBatch->Draw
	(
		gpuHandle,
		m_size,
		m_position,
		&m_sourceRect,
		Colors::White,
		XMConvertToRadians(m_rotation),
		m_origin,
		m_scale
	);
}
