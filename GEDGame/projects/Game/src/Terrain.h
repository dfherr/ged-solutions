#pragma once

#include "DXUT.h"
#include "d3dx11effect.h"
#include "ConfigParser.h"

class Terrain
{
public:
	Terrain(void);
	~Terrain(void);

	HRESULT create(ID3D11Device* device);
	void destroy();

	void render(ID3D11DeviceContext* context, ID3DX11EffectPass* pass);


	int res;
	float cameraHeight;

private:
	Terrain(const Terrain&);
	Terrain(const Terrain&&);
	void operator=(const Terrain&);

	// general resources
	// ID3D11ShaderResourceView* debugSRV; //removed for assignment 4

	// terrain rendering resources
	// the terrain's vertices
	// ID3D11Buffer* vertexBuffer;	// removed for assignment 5

	// the terrain's triangulation
	ID3D11Buffer* indexBuffer;	

	// the terrain's material color for diffuse lighting
	ID3D11Texture2D* diffuseTexture; 

	// describes the structure of the diffuse texture to the shader stages
	ID3D11ShaderResourceView* diffuseTextureSRV; 

	// assignment 5 heightmap and normalmap recources
	ID3D11Texture2D* normalTexture;
	ID3D11ShaderResourceView* normalTextureSRV;

	ID3D11ShaderResourceView* heightBufferSRV;
	ID3D11Buffer* heightBuffer;
};