#pragma once

#include <cstdint>
#include <vector>
#include <DXUT.h>
#include <d3dx11effect.h>


//C++ struct for t3d vertex buffer
struct T3dVertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
};

// Corresponding struct for usage in your effect file
//
//struct T3dVertexVSIn
//{
//    float3 Pos : POSITION; //Position in object space
//    float2 Tex : TEXCOORD; //Texture coordinate
//    float3 Nor : NORMAL;   //Normal in object space
//	  float3 Tan : TANGENT;  //Tangent in object space
//};

class T3d
{
public:
	static HRESULT readFromFile(const std::string& filename, std::vector<T3dVertex>& vertexBufferData, 
                                                      std::vector<uint32_t>& indexBufferData);
													  
	static HRESULT readFromFile(const std::wstring& filename, std::vector<T3dVertex>& vertexBufferData, 
                                                      std::vector<uint32_t>& indexBufferData);

	static HRESULT createT3dInputLayout(ID3D11Device* pd3dDevice, 
		ID3DX11EffectPass* pass, ID3D11InputLayout** t3dInputLayout);
};