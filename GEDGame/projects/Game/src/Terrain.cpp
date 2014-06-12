#include <DDSTextureLoader.h>

//#include "SimpleImage/SimpleImage.h"
#include "Terrain.h"
#include "GameEffect.h"
#include "DirectXTex.h"
#include "ConfigParser.h"
#include "SimpleImage.h"
#include "DirectXMath.h"
#include <iostream>

using namespace DirectX;

// you can use this macro to access your height field
#define IDX(X,Y,WIDTH) ((X) + (Y) * (WIDTH))

int indices_size;
/*
* Create the terrain and let everything point to null.
*/
Terrain::Terrain(void):
	indexBuffer(nullptr),
	vertexBuffer(nullptr),
	diffuseTexture(nullptr),
	diffuseTextureSRV(nullptr),
	debugSRV(nullptr)
{
}

Terrain::~Terrain(void)
{
}

HRESULT Terrain::create(ID3D11Device* device)
{
	HRESULT hr;

	// in our example, we load a debug texture
	V(DirectX::CreateDDSTextureFromFile(device, L"resources\\debug_green.dds", nullptr, &debugSRV));

	if (hr != S_OK) {
		MessageBoxA (NULL, "Could not load texture \"resources\\debug_green.dds\"", "Invalid texture", MB_ICONERROR | MB_OK);
		return hr;
	}
	/* cut out triangle code
	// this buffer contains positions, normals and texture coordinates for one triangle
	float triangle[] = {
	// Vertex 0
	-400.0f, 0.0f, -400.0f,  1.0f, // Position
	0.0f,    1.0f,    0.0f,  0.0f, // Normal
	0.0f,    0.0f,                 // Texcoords

	// Vertex 1
	400.0f,   0.0f, -400.0f, 1.0f, // Position
	0.0f,     1.0f,    0.0f, 0.0f, // Normal
	0.0f,     1.0f,                // Texcoords

	// Vertex 2
	-400.0f, 0.0f,  400.0f,  1.0f, // Position
	0.0f,    1.0f,    0.0f,  0.0f, // Normal
	1.0f,    0.0f,                 // Texcoords
	};
	*/

	// TODO: Replace this vertex array (triangle) with an array (or vector)
	// which contains the vertices of your terrain. Calculate position,
	// normal and texture coordinates according to your height field and
	// the dimensions of the terrain specified by the ConfigParser

	// load heightfield with ConfigParser
	std::string heightPath = g_configParser.getTerrainHeightPath(); 
	WCHAR path[MAX_PATH];
	std::wstring w_heightPath(heightPath.begin(), heightPath.end());
	V(DXUTFindDXSDKMediaFileCch(path, MAX_PATH, w_heightPath.c_str()));
	GEDUtils::SimpleImage heightfield(path);
	// load other config parameters
	float width = g_configParser.getTerrainWidth();
	float height = g_configParser.getTerrainHeight();
	float depth = g_configParser.getTerrainDepth();
	// Note 1: The normal map that you created last week will not be used
	// in this assignment (Assignment 4). It will be of use in later assignments

	// Note 2: For each vertex 10 floats are stored. Do not use another
	// layout.

	// Note 3: In the coordinate system of the vertex buffer (output):
	// x = east,    y = up,    z = south,          x,y,z in [0,1] (float)

	// create Vertex buffer
	struct Vertex {
		XMFLOAT4 p; // Position 
		XMFLOAT4 n; // Normal 
		XMFLOAT2 u; // Texture coordinates
	};

	int res = heightfield.getWidth() - 1;
	int vertex_buff_size = res*res;
	Vertex *vertex_buffer = new Vertex[vertex_buff_size];

	// iterate through the heightfield
	 // vertex buffer iterator

	int it = 0;			
	double x_dv;
	double y_dv;
	double z;
	double normal_length;
	for (int y=0; y<res; ++y)
	{
		for (int x=0; x<res; ++x, ++it)
		{
			vertex_buffer[it].p.x = (x / (float)res -0.5f) * width;
			vertex_buffer[it].p.y = heightfield.getPixel(x, y) * height;
			vertex_buffer[it].p.z = (y / (float)res - 0.5f) * depth;
			vertex_buffer[it].p.w = 1.0f;
			//boarder wraping with ((x+1) + resolution) % resolution, + resolution can be skipped for +1, only necessary for -1
			// therefor resolution maps to = 0 and -1 maps to resolution-1
			// don't forget to scale x,y according to height/width of heightfield
			if(x == 0 || x == (res - 1)){
				x_dv = (heightfield.getPixel((x+1)%res, y) - heightfield.getPixel((x-1 + res)%res, y)) / 2.0 * res;
			}else{
				x_dv = (heightfield.getPixel(x+1, y) - heightfield.getPixel(x-1, y)) / 2.0 * res;
			}
			if(y == 0 || y == (res - 1)){
				y_dv = (heightfield.getPixel(x, (y+1)%res) - heightfield.getPixel(x, (y-1+res) % res)) / 2.0 * res;
			}
			else{
				y_dv = (heightfield.getPixel(x, y+1) - heightfield.getPixel(x, y-1)) / 2.0 * res;
			}

			z = 1.0;
			normal_length = sqrt((x_dv*x_dv)+(y_dv*y_dv)+1);

			// normalize normal (-x_dv, -y_dv, z)
			x_dv = -(x_dv /normal_length);
			y_dv = -(y_dv /normal_length);
			z = (z / normal_length);

			// map to [0,1]
		//	x_dv = x_dv/2.0 + 0.5f;
		//	y_dv = y_dv/2.0 + 0.5f;
		//	z = z/2.0 + 0.5f;	

			XMMATRIX matNormalScaling = XMMatrixScaling(width, height, depth);
			matNormalScaling = XMMatrixTranspose(XMMatrixInverse(nullptr, matNormalScaling));
			XMVECTOR vNormal = XMVectorSet(x_dv, y_dv, z, 0.0f);
			vNormal = XMVector4Transform(vNormal, matNormalScaling);
			vNormal = XMVector3Normalize(vNormal);
			XMFLOAT4 vectorFloats;
			XMStoreFloat4(&vectorFloats, vNormal);
			// normals calculated

			vertex_buffer[it].n = vectorFloats;
			vertex_buffer[it].u.x = x / (float)res;
			vertex_buffer[it].u.y = y / (float)res;
		}
	}


	D3D11_SUBRESOURCE_DATA id;
	id.pSysMem = &vertex_buffer[0];
	id.SysMemPitch = 10 * sizeof(float); // Stride
	id.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(Vertex) * vertex_buff_size; //The size in bytes of the vertex array
	// TODO: Change this s.t. it fits the size
	// of your vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	V(device->CreateBuffer(&bd, &id, &vertexBuffer)); // http://msdn.microsoft.com/en-us/library/ff476899%28v=vs.85%29.aspx
	// Create index buffer
	// TODO: Insert your code to create the index buffer
	// Create and fill description
	indices_size = (res-1) * (res-1) * 2 * 3; // each square has 2 triangles with 3 vertices
	unsigned int *indices = new unsigned int[indices_size];
	
	// fill index buffer
	it = 0; // index buffer iterator
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
/**
	for(int i = 0; i < indices_size; i += 3){
	std::cout << "Triangle: v1: " << indices[i] << " v2: "<< indices[i+1] << " v3: " << indices[i+2] << std::endl;	
	}
*/
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

	// Load color texture (color map)
	// TODO: Insert your code to load the color texture and create
	// the texture "diffuseTexture" as well as the shader resource view
	// "diffuseTextureSRV"
	// load colorMap with ConfigParser
	std::string colorPath = g_configParser.getTerrainColorPath();
	path[MAX_PATH];
	std::wstring w_colorPath(colorPath.begin(), colorPath.end());
	V(DXUTFindDXSDKMediaFileCch(path, MAX_PATH, w_colorPath.c_str()));
	CreateDDSTextureFromFile(device, path, nullptr, &diffuseTextureSRV);
	

	return hr;
}


void Terrain::destroy()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(debugSRV);
	SAFE_RELEASE(diffuseTextureSRV);

	// TODO: Release the index buffer
	// TODO: Release the terrain's shader resource view and texture
}


void Terrain::render(ID3D11DeviceContext* context, ID3DX11EffectPass* pass)
{
	HRESULT hr;

	// Bind the terrain vertex buffer to the input assembler stage 
	ID3D11Buffer* vbs[] = { vertexBuffer, };
	unsigned int strides[] = { 10 * sizeof(float), }, offsets[] = { 0, };
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	// TODO: Bind the terrain index buffer to the input assembler stage
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Tell the input assembler stage which primitive topology to use
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);    

	// TODO: Bind the SRV of the terrain diffuse texture to the effect variable
	// (instead of the SRV of the debug texture)
	V(g_gameEffect.diffuseEV->SetResource( diffuseTextureSRV));

	// Apply the rendering pass in order to submit the necessary render state changes to the device
	V(pass->Apply(0, context));

	// Draw
	// TODO: Use DrawIndexed to draw the terrain geometry using as shared vertex list
	// (instead of drawing only the vertex buffer)
	context->DrawIndexed(indices_size, 0, 0);
}
