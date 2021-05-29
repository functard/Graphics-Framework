#include "PCH.h"
#include "TexturedEntity.h"

CTexturedEntity::CTexturedEntity(std::wstring _path, EResourceType _type, XMFLOAT3 _pos)
	: CEntity(_pos)
{
	m_path = _path;
	m_type = _type;
}

bool CTexturedEntity::Init()
{
	CEntity::Init();
	
	ResourceUploadBatch resourceUploadBatch(DXS.m_Device.Get());
	resourceUploadBatch.Begin();

	int value = ::CreateWICTextureFromFile(DXS.m_Device.Get(),
		resourceUploadBatch, m_path.c_str(), &m_texture);

	if (FAILED(value))
	{
		return false;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
	{
		DXS.m_ResourceDescriptors->GetCPUDescriptorHandleForHeapStart(),
		(int)m_type,
		DXS.m_CBVDescriptorSize
	};

	::CreateShaderResourceView(DXS.m_Device.Get(), m_texture.Get(), handle);

	auto uploadFinish = resourceUploadBatch.End(DXS.m_DirectCommandQueue.Get());
	uploadFinish.wait();

	return true;
}

void CTexturedEntity::Render()
{
	XMMATRIX modelMatrix;
	XMMATRIX scale = XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw
	(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z)
	);
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	modelMatrix = scale * rotation * translation;
	XMMATRIX view = DXS.m_ViewMatrix;
	XMMATRIX proj = DXS.m_ProjectionMatrix;
	XMMATRIX mvpMatrix = XMMatrixMultiply(modelMatrix, view);
	mvpMatrix = XMMatrixMultiply(mvpMatrix, proj);

	DXS.m_DirectCommandList->SetPipelineState(DXS.m_TexturedShaderPipelineState.Get());
	DXS.m_DirectCommandList->SetGraphicsRootSignature(DXS.m_TexturedShaderRootSignature.Get());
	DXS.m_DirectCommandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);
	DXS.m_DirectCommandList->SetGraphicsRoot32BitConstants(1, sizeof(XMMATRIX) / 4, &modelMatrix, 0);

	CD3DX12_GPU_DESCRIPTOR_HANDLE handle =
	{
		DXS.m_ResourceDescriptors->GetGPUDescriptorHandleForHeapStart(),
		(int)EResourceType::LIGHT_CONSTANT_BUFFER,
		DXS.m_CBVDescriptorSize
	};
	DXS.m_DirectCommandList->SetGraphicsRootDescriptorTable(2, handle);

	handle =
	{
		DXS.m_ResourceDescriptors->GetGPUDescriptorHandleForHeapStart(),
		(int)m_type,
		DXS.m_CBVDescriptorSize
	};
	DXS.m_DirectCommandList->SetGraphicsRootDescriptorTable(3, handle);

	DXS.m_DirectCommandList->IASetIndexBuffer(&m_indexBufferView);
	DXS.m_DirectCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	DXS.m_DirectCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DXS.m_DirectCommandList->RSSetScissorRects(1, &DXS.m_ScissorRect);
	DXS.m_DirectCommandList->RSSetViewports(1, &DXS.m_Viewport);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
	{
		DXS.m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		DXS.m_BackbufferIndex,
		DXS.m_CBVDescriptorSize
	};

	DXS.m_DirectCommandList->OMSetRenderTargets(1, &rtv, false, &DXS.m_StencilHeap->GetCPUDescriptorHandleForHeapStart());

	DXS.m_DirectCommandList->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
}
