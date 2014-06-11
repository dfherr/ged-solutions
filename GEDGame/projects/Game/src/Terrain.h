#pragma once

#include "DXUT.h"
#include "d3dx11effect.h"

class Terrain
{
public:
	Terrain(void);
	~Terrain(void);

	HRESULT create(ID3D11Device* device);
	void destroy();

	void render(ID3D11DeviceContext* context, ID3DX11EffectPass* pass);

private:
	Terrain(const Terrain&);
	Terrain(const Terrain&&);
	void operator=(const Terrain&);

	// general resources
	ID3D11ShaderResourceView* debugSRV;

	// terrain rendering resources
	// the terrain's vertices
	ID3D11Buffer* vertexBuffer;	

	// the terrain's triangulation
	ID3D11Buffer* indexBuffer;	

	// the terrain's material color for diffuse lighting
	ID3D11Texture2D* diffuseTexture; 

	// describes the structure of the diffuse texture to the shader stages
	ID3D11ShaderResourceView* diffuseTextureSRV; 
};