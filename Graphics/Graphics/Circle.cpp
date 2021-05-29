#include "PCH.h"
#include "Circle.h"
#include "Engine.h"
#include "Helpers.h"

Circle::Circle(int _resolution, float _radius, XMFLOAT3 _pos)
	: CEntity(_pos)
{
	m_radius = _radius;
	if (_resolution < 3)
	{
		_resolution = 3;
	}

	if (_radius <= 0)
	{
		_radius = 1.0f;
	}


	XMFLOAT4 color = XMFLOAT4(1, 0, 1, 0);
	XMFLOAT3 normal = XMFLOAT3(0, 0, -1);

	m_vertices.push_back({ XMFLOAT3(0, 0, 0), color, normal });

	float radiansPerSlice = (2 * XM_PI) / _resolution;

	float x;
	float y;

	for (int i = 0; i < _resolution; i++)
	{
		x = ::sinf(radiansPerSlice * i);
		y = ::cosf(radiansPerSlice * i);

		m_vertices.push_back({ XMFLOAT3(x * _radius, y * _radius, 0), color, normal });
	}

	for (int i = 0; i < _resolution - 1; i++)
	{
		m_indices.push_back(0);
		m_indices.push_back(1 + i);
		m_indices.push_back(2 + i);
	}

	m_indices.push_back(0);
	m_indices.push_back(_resolution);
	m_indices.push_back(1);
}

//void Circle::Update(float _deltaTime)
//{
//	if (IPM.GetKey(DIK_SPACE))
//	{
//		m_position = m_position + (up * _deltaTime * 7);
//
//		//m_position = m_position + (forward * _deltaTime * 20);
//	}
//
//	if (m_rotation.z >= 360)
//		m_rotation.z -= 360;
//
//	//if(!inBlackHole)
//	
//	if(!inBlackHole)
//		m_position = m_position - (up * _deltaTime * 2.5);
//
//	
//	/*if (IPM.GetKey(DIK_S))
//	{
//		m_position = m_position - (forward * _deltaTime * 20);
//	}
//	if (IPM.GetKey(DIK_A))
//	{
//		m_position = m_position - (right * _deltaTime * 20);
//	}
//	if (IPM.GetKey(DIK_D))
//	{
//		m_position = m_position + (right * _deltaTime * 20);
//	}
//	if (IPM.GetKey(DIK_Q))
//	{
//		m_position = m_position + (up * _deltaTime * 200);
//	}
//	if (IPM.GetKey(DIK_E))
//	{
//		m_position = m_position - (up * _deltaTime * 20);
//	}*/
//
//
//	if (IPM.GetKey(DIK_A))
//		m_rotation.z += _deltaTime * 40;
//	
//	if (IPM.GetKey(DIK_D))
//		m_rotation.z -= _deltaTime * 40;
//
//	if(IPM.GetKey(DIK_W))
//		m_rotation.x += _deltaTime * 40;
//
//	if(IPM.GetKey(DIK_W))
//		m_rotation.x -= _deltaTime * 40;
//	 
//
//	XMVECTOR x = { 1, 0, 0, 0 };
//	XMVECTOR y = { 0, 1, 0, 0 };
//	XMVECTOR z = { 0, 0, 1, 0 };
//
//	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw
//	(
//		XMConvertToRadians(m_rotation.x),
//		XMConvertToRadians(m_rotation.y),
//		XMConvertToRadians(m_rotation.z)
//	);
//
//	x = XMVector3Transform(x, rotationMatrix);
//	y = XMVector3Transform(y, rotationMatrix);
//	z = XMVector3Transform(z, rotationMatrix);
//
//	forward = XMFLOAT3(z.m128_f32[0], z.m128_f32[1], z.m128_f32[2]);
//	right = XMFLOAT3(x.m128_f32[0], x.m128_f32[1], x.m128_f32[2]);
//	up = XMFLOAT3(y.m128_f32[0], y.m128_f32[1], y.m128_f32[2]);
//}

//void Circle::Render()
//{
//
//	XMMATRIX modelMatrix;
//	XMMATRIX scale = XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z);
//	XMMATRIX rotation = XMMatrixRotationRollPitchYaw
//	(
//		XMConvertToRadians(m_rotation.x),
//		XMConvertToRadians(m_rotation.y),
//		XMConvertToRadians(m_rotation.z)
//	);
//	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
//
//	modelMatrix = scale * rotation * translation;
//	XMMATRIX view = DXS.m_ViewMatrix;
//	XMMATRIX proj = DXS.m_ProjectionMatrix;
//	XMMATRIX mvpMatrix = XMMatrixMultiply(modelMatrix, view);
//	mvpMatrix = XMMatrixMultiply(mvpMatrix, proj);
//
//	DXS.m_DirectCommandList->SetPipelineState(DXS.m_CellShadingPipelineState.Get());
//	DXS.m_DirectCommandList->SetGraphicsRootSignature(DXS.m_CellShadingRootSignature.Get());
//	DXS.m_DirectCommandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);
//	DXS.m_DirectCommandList->SetGraphicsRoot32BitConstants(1, sizeof(XMMATRIX) / 4, &modelMatrix, 0);
//
//	CD3DX12_GPU_DESCRIPTOR_HANDLE handle =
//	{
//		DXS.m_ResourceDescriptors->GetGPUDescriptorHandleForHeapStart(),
//		(int)EResourceType::LIGHT_CONSTANT_BUFFER,
//		DXS.m_DescriptorSize
//	};
//	DXS.m_DirectCommandList->SetGraphicsRootDescriptorTable(2, handle);
//
//	DXS.m_DirectCommandList->IASetIndexBuffer(&m_indexBufferView);
//	DXS.m_DirectCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
//	DXS.m_DirectCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	DXS.m_DirectCommandList->RSSetScissorRects(2, &DXS.m_ScissorRect);
//	DXS.m_DirectCommandList->RSSetViewports(1, &DXS.m_Viewport);
//
//	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
//	{
//		DXS.m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
//		DXS.m_BackbufferIndex,
//		DXS.m_CBVDescriptorSize
//	};
//
//	DXS.m_DirectCommandList->OMSetRenderTargets(1, &rtv, false, &DXS.m_StencilHeap->GetCPUDescriptorHandleForHeapStart());
//
//	DXS.m_DirectCommandList->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
//}

