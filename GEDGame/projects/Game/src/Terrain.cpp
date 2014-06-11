#include "Terrain.h"

#include "GameEffect.h"
//#include "SimpleImage/SimpleImage.h"
#include <DDSTextureLoader.h>
#include "DirectXTex.h"

// You can use this macro to access your height field
#define IDX(X,Y,WIDTH) ((X) + (Y) * (WIDTH))

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

	// In our example, we load a debug texture
    V(DirectX::CreateDDSTextureFromFile(device, L"resources\\debug_green.dds", nullptr, &debugSRV));

	if (hr != S_OK) {
        MessageBoxA (NULL, "Could not load texture \"resources\\debug_green.dds\"", "Invalid texture", MB_ICONERROR | MB_OK);
		return hr;
	}

	// This buffer contains positions, normals and texture coordinates for one triangle
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

	// TODO: Replace this vertex array (triangle) with an array (or vector)
	// which contains the vertices of your terrain. Calculate position,
	// normal and texture coordinates according to your height field and
	// the dimensions of the terrain specified by the ConfigParser

	// Note 1: The normal map that you created last week will not be used
	// in this assignment (Assignment 4). It will be of use in later assignments

	// Note 2: For each vertex 10 floats are stored. Do not use another
	// layout.

    // Note 3: In the coordinate system of the vertex buffer (output):
    // x = east,    y = up,    z = south,          x,y,z in [0,1] (float)

    D3D11_SUBRESOURCE_DATA id;
    id.pSysMem = &triangle[0];
    id.SysMemPitch = 10 * sizeof(float); // Stride
    id.SysMemSlicePitch = 0;

    D3D11_BUFFER_DESC bd;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(triangle); //The size in bytes of the triangle array
									// TODO: Change this s.t. it fits the size
									// of your vertex buffer
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;

    V(device->CreateBuffer(&bd, &id, &vertexBuffer)); // http://msdn.microsoft.com/en-us/library/ff476899%28v=vs.85%29.aspx

	// Create index buffer
	// TODO: Insert your code to create the index buffer

	// Load color texture (color map)
	// TODO: Insert your code to load the color texture and create
	// the texture "diffuseTexture" as well as the shader resource view
	// "diffuseTextureSRV"

	return hr;
}


void Terrain::destroy()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(debugSRV);

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

    // Tell the input assembler stage which primitive topology to use
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);    

    // TODO: Bind the SRV of the terrain diffuse texture to the effect variable
    // (instead of the SRV of the debug texture)
	V(g_gameEffect.diffuseEV->SetResource( debugSRV));

    // Apply the rendering pass in order to submit the necessary render state changes to the device
    V(pass->Apply(0, context));

    // Draw
    // TODO: Use DrawIndexed to draw the terrain geometry using as shared vertex list
    // (instead of drawing only the vertex buffer)
    context->Draw(3, 0);
}
