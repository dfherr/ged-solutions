#include <DDSTextureLoader.h>

#include "Terrain.h"
#include "GameEffect.h"
#include "DirectXTex.h"
#include "ConfigParser.h"
#include "SimpleImage.h"
#include "DirectXMath.h"
#include "debug.h"

using namespace DirectX;

// you can use this macro to access your height field
#define IDX(X,Y,WIDTH) ((X) + (Y) * (WIDTH))

int indices_size;
/*
* Create the terrain and let everything point to null.
*/
Terrain::Terrain(void):
	indexBuffer(nullptr),
	diffuseTexture(nullptr),
	diffuseTextureSRV(nullptr),
	normalTexture(nullptr),
	normalTextureSRV(nullptr),
	heightBuffer(nullptr),
	heightBufferSRV(nullptr)
{
}

Terrain::~Terrain(void)
{
}

HRESULT Terrain::create(ID3D11Device* device)
{
	HRESULT hr;

	// Create an array (or vector) which contains the vertices of your terrain. 
	// Calculate position, normal and texture coordinates according to your 
	// height field and the dimensions of the terrain specified by the ConfigParser

	// load heightfield with ConfigParser
	std::string heightPath = g_configParser.getTerrainHeightPath(); 
	WCHAR path[MAX_PATH];
	std::wstring w_heightPath(heightPath.begin(), heightPath.end());
	V(DXUTFindDXSDKMediaFileCch(path, MAX_PATH, w_heightPath.c_str()));
	GEDUtils::SimpleImage heightfield(path);
	// load widht/depth/height config parameters
	float width = g_configParser.getTerrainWidth();
	float height = g_configParser.getTerrainHeight();
	float depth = g_configParser.getTerrainDepth();

	res = heightfield.getWidth();
	float *heightArr = new float[res*res];
	// load height data from image into array
	for(int y = 0; y < res; ++y){
		for(int x = 0; x < res; ++x){
				heightArr[IDX(x,y, res)] = heightfield.getPixel(x, y);
		}
	}
	

	D3D11_SUBRESOURCE_DATA id;
	id.pSysMem = &heightArr[0];
	id.SysMemPitch = sizeof(float); // Stride
	id.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.ByteWidth = sizeof(float) * (res * res); //The size in bytes of the vertex array
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	// V(device->CreateBuffer(&bd, &id, &vertexBuffer)); // http://msdn.microsoft.com/en-us/library/ff476899%28v=vs.85%29.aspx 	// removed for assignment 5
	V(device->CreateBuffer(&bd, &id, &heightBuffer)); 

	D3D11_BUFFER_SRV bsrv;
	bsrv.FirstElement = 0;
	bsrv.NumElements = res * res;

	// Create heightBufferSRV desc
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.Buffer = bsrv;

	V(device->CreateShaderResourceView(heightBuffer, &srvd, &heightBufferSRV));
	delete[] heightArr;
	// Create index buffer
	// Create and fill description
	indices_size = (res - 1) * (res - 1) * 2 * 3; // each square has 2 triangles with 3 vertices, we don't need the last vertices as there are no squares right/bottom of it
	unsigned int *indices = new unsigned int[indices_size];

	// fill index buffer
	int it = 0; // index buffer iterator
	for(int y = 0; y < (res-1); ++y){
		for(int x = 0; x < (res-1); ++x){
			// top left triangle
			indices[it++] = (res * y) + x;
			indices[it++] = (res * y) + (x+1);
			indices[it++] = (res * (y+1)) + x;
			// bottom right triangle
			indices[it++] = (res * (y+1)) + x;
			indices[it++] = (res * y) + (x+1);
			indices[it++] = (res * (y+1)) + (x+1);
		}
	}

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int) * indices_size;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	// Define initial data
	ZeroMemory(&id, sizeof(id));
	id.pSysMem = &indices[0];
	// Create Buffer
	V(device->CreateBuffer( &bd, &id, &indexBuffer ));
	delete[] indices;
	// load colorMap with ConfigParser in diffuseTextureSRV
	std::string colorPath = g_configParser.getTerrainColorPath();
	path[MAX_PATH];
	std::wstring w_colorPath(colorPath.begin(), colorPath.end());
	V(DXUTFindDXSDKMediaFileCch(path, MAX_PATH, w_colorPath.c_str()));
	CreateDDSTextureFromFile(device, path, nullptr, &diffuseTextureSRV);

	// Load normal texture (normal map)
	std::string normalPath = g_configParser.getTerrainNormalPath();
	path[MAX_PATH];
	std::wstring w_normalPath(normalPath.begin(), normalPath.end());
	V(DXUTFindDXSDKMediaFileCch(path, MAX_PATH, w_normalPath.c_str()));
	CreateDDSTextureFromFile(device, path, nullptr, &normalTextureSRV);


	return hr;
}


void Terrain::destroy()
{
	// SAFE RELEASE all GPU variables. 
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(diffuseTextureSRV);
	SAFE_RELEASE(diffuseTexture);
	SAFE_RELEASE(normalTexture);
	SAFE_RELEASE(normalTextureSRV);
	SAFE_RELEASE(heightBuffer);
	SAFE_RELEASE(heightBufferSRV);
}


void Terrain::render(ID3D11DeviceContext* context, ID3DX11EffectPass* pass)
{
	HRESULT hr;

	// Bind the terrain vertex buffer to the input assembler stage 
	ID3D11Buffer* vbs[] = { nullptr, };
	unsigned int strides[] = { 0, }, offsets[] = { 0, };
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	// Bind the terrain index buffer to the input assembler stage
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Tell the input assembler stage which primitive topology to use
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);    

	// Bind the SRV of the terrain diffuse texture to the effect variable
	// (instead of the SRV of the debug texture)
	V(g_gameEffect.diffuseEV->SetResource( diffuseTextureSRV));
	V(g_gameEffect.heightmap->SetResource( heightBufferSRV));
	V(g_gameEffect.normalmap->SetResource( normalTextureSRV));
	V(g_gameEffect.resolution->SetInt(res));

	// Apply the rendering pass in order to submit the necessary render state changes to the device
	V(pass->Apply(0, context));

	// Draw
	// Use DrawIndexed to draw the terrain geometry using as shared vertex list
	// (instead of drawing only the vertex buffer)
	context->DrawIndexed(indices_size, 0, 0);
}
