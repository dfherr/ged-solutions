#pragma once

#include <DXUT.h>
#include <d3dx11effect.h>

#include <vector>
#include <cstdint>


//This class ecapsulates the D3D11 resources needed for a mesh
class Mesh
{
public:
	//Create mesh object and set filenames, but don't create D3D11 resources
	//This constructor should be called from within InitApp().
	Mesh(const std::string& filename_t3d,           //filename of t3d file for mesh geometry
         const std::string& filename_dds_diffuse,   //filename of dds file for diffuse texture
         const std::string& filename_dds_specular,  //filename of dds file for specular texture
         const std::string& filename_dds_glow);     //filename of dds file for glow texture
		 
	Mesh(const std::wstring& filename_t3d,           //filename of t3d file for mesh geometry
		const std::wstring& filename_dds_diffuse,   //filename of dds file for diffuse texture
		const std::wstring& filename_dds_specular,  //filename of dds file for specular texture
		const std::wstring& filename_dds_glow);     //filename of dds file for glow texture

	//Currently does nothing.
	//This destructor should be called from within DeinitApp().
	~Mesh(void);

	//Creates the required D3D11 resources from the given input files.
	//This function should be called from within OnD3D11CreateDevice().
	HRESULT create(ID3D11Device* device);

	//Releases all D3D11 resources of the mesh.
	//This destructor should be called from within OnD3D11DestroyDevice().
	void destroy();

	// Creates an input layout which is used for meshes
	static HRESULT createInputLayout(ID3D11Device* device, 
		ID3DX11EffectPass* pass);

	// Releases the input layout
	static void destroyInputLayout();

	// Render the mesh
	HRESULT render(ID3D11DeviceContext* context, ID3DX11EffectPass* pass,
        ID3DX11EffectShaderResourceVariable* diffuseEffectVariable,
        ID3DX11EffectShaderResourceVariable* specularEffectVariable,
        ID3DX11EffectShaderResourceVariable* glowEffectVariable);

private:
	//Reads the complete file given by "path" byte-wise into "data".
	static HRESULT loadFile(const char * filename, std::vector<uint8_t>& data);

	// Creates DX Texture Resources from File
	static HRESULT createTexture(ID3D11Device* device, const std::wstring& filename, 
		ID3D11Texture2D** tex, ID3D11ShaderResourceView** srv);
	
private:
	//Filenames
	std::wstring				filenameT3d;
	std::wstring				filenameDDSDiffuse;
	std::wstring				filenameDDSSpecular;
	std::wstring				filenameDDSGlow;

	//Mesh geometry information
	ID3D11Buffer*               vertexBuffer;
	ID3D11Buffer*               indexBuffer;
	int                         indexCount; //number of single indices in indexBuffer (needed for DrawIndexed())

	//Mesh textures and corresponding shader resource views
	ID3D11Texture2D*            diffuseTex;
	ID3D11ShaderResourceView*   diffuseSRV;
	ID3D11Texture2D*            specularTex;
	ID3D11ShaderResourceView*   specularSRV;
	ID3D11Texture2D*	        glowTex;
	ID3D11ShaderResourceView*   glowSRV;

	//Mesh Input layout
	static ID3D11InputLayout*	inputLayout;
};