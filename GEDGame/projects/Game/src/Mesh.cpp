#include "Mesh.h"

#include "T3d.h"
#include <DDSTextureLoader.h>

ID3D11InputLayout*	Mesh::inputLayout;

Mesh::Mesh(const std::string& filename_t3d,
           const std::string& filename_dds_diffuse,
           const std::string& filename_dds_specular,
           const std::string& filename_dds_glow)
  : 
	//Default values for all other member variables
    vertexBuffer(NULL), indexBuffer(NULL),
	indexCount(0),
	diffuseTex(NULL), diffuseSRV(NULL),
	specularTex(NULL), specularSRV(NULL),
	glowTex(NULL), glowSRV(NULL)
{
	filenameT3d = std::wstring(filename_t3d.begin(), filename_t3d.end());
	filenameDDSDiffuse = std::wstring(filename_dds_diffuse.begin(), filename_dds_diffuse.end());
	filenameDDSSpecular = std::wstring(filename_dds_specular.begin(), filename_dds_specular.end());
	filenameDDSGlow = std::wstring(filename_dds_glow.begin(), filename_dds_glow.end());
}


Mesh::Mesh(const std::wstring& filename_t3d,
           const std::wstring& filename_dds_diffuse,
           const std::wstring& filename_dds_specular,
           const std::wstring& filename_dds_glow)
  : filenameT3d        (filename_t3d),
	filenameDDSDiffuse (filename_dds_diffuse),
	filenameDDSSpecular(filename_dds_specular),
	filenameDDSGlow    (filename_dds_glow),
	//Default values for all other member variables
    vertexBuffer(NULL), indexBuffer(NULL),
	indexCount(0),
	diffuseTex(NULL), diffuseSRV(NULL),
	specularTex(NULL), specularSRV(NULL),
	glowTex(NULL), glowSRV(NULL)
{
}

Mesh::~Mesh(void)
{
}

HRESULT Mesh::create(ID3D11Device* device)
{	
	HRESULT hr;

	//Some variables that we will need
	D3D11_SUBRESOURCE_DATA id = {0};
	D3D11_BUFFER_DESC bd = {0};

	//Read mesh
	std::vector<T3dVertex> vertexBufferData;
	std::vector<uint32_t> indexBufferData;

	V(T3d::readFromFile(filenameT3d.c_str(), vertexBufferData, indexBufferData));

	id.pSysMem = &vertexBufferData[0];
	id.SysMemPitch = sizeof(T3dVertex); // Stride
    id.SysMemSlicePitch = 0;

    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = vertexBufferData.size() * sizeof(T3dVertex);
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;

	V(device->CreateBuffer(&bd, &id, &vertexBuffer));


	indexCount = indexBufferData.size();

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int) * indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	// Define initial data

	ZeroMemory(&id, sizeof(id));
	id.pSysMem = &indexBufferData[0];
	// Create Buffer
	V(device->CreateBuffer( &bd, &id, &indexBuffer ));



	// Create textures
	V(createTexture(device, filenameDDSDiffuse, &diffuseTex, &diffuseSRV)	);
	V(createTexture(device, filenameDDSSpecular, &specularTex, &specularSRV));
	V(createTexture(device, filenameDDSGlow, &glowTex, &glowSRV)			);


	return S_OK;
}

void Mesh::destroy()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer );
	SAFE_RELEASE(diffuseTex	 );
	SAFE_RELEASE(diffuseSRV	 );
	SAFE_RELEASE(specularTex );
	SAFE_RELEASE(specularSRV );
	SAFE_RELEASE(glowTex	 );
	SAFE_RELEASE(glowSRV	 );
}

HRESULT Mesh::createInputLayout(ID3D11Device* device, ID3DX11EffectPass* pass)
{
	HRESULT hr;
	V(T3d::createT3dInputLayout(device, pass, &inputLayout));
	return S_OK;
}

void Mesh::destroyInputLayout()
{
	SAFE_RELEASE(inputLayout);
}

HRESULT Mesh::render(ID3D11DeviceContext* context, ID3DX11EffectPass* pass, 
        ID3DX11EffectShaderResourceVariable* diffuseEffectVariable,
        ID3DX11EffectShaderResourceVariable* specularEffectVariable,
        ID3DX11EffectShaderResourceVariable* glowEffectVariable)
{
	HRESULT hr;

    if ((diffuseEffectVariable == nullptr) || !diffuseEffectVariable->IsValid())
    {
        throw std::exception("Diffuse EV is null or invalid");
    }
    if ((specularEffectVariable == nullptr) || !specularEffectVariable->IsValid())
    {
        throw std::exception("Diffuse EV is null or invalid");
    }
    if ((glowEffectVariable == nullptr) || !glowEffectVariable->IsValid())
    {
        throw std::exception("Diffuse EV is null or invalid");
    }

	V(diffuseEffectVariable->SetResource(diffuseSRV));
	V(specularEffectVariable->SetResource(specularSRV));
	V(glowEffectVariable->SetResource(glowSRV));

	// Bind the terrain vertex buffer to the input assembler stage 
	ID3D11Buffer* vbs[] = { vertexBuffer, };
    unsigned int strides[] = {sizeof(T3dVertex), }, offsets[] = { 0, };
    context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

    // Tell the input assembler stage which primitive topology to use
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout);

	V(pass->Apply(0, context));

	context->DrawIndexed(indexCount, 0, 0);

	return S_OK;
	
}

HRESULT Mesh::loadFile(const char * filename, std::vector<uint8_t>& data)
{
	FILE * filePointer = NULL;
    errno_t error = fopen_s(&filePointer, filename, "rb");
	if (error) 	{ return E_INVALIDARG; }
	fseek(filePointer, 0, SEEK_END);
	long bytesize = ftell(filePointer);
	fseek(filePointer, 0, SEEK_SET);
	data.resize(bytesize);
	fread(&data[0], 1, bytesize, filePointer);
	fclose(filePointer);
	return S_OK;
}

HRESULT Mesh::createTexture(ID3D11Device* device, const std::wstring& filename, ID3D11Texture2D** tex, 
					  ID3D11ShaderResourceView** srv)
{
	HRESULT hr;
    V_RETURN(DirectX::CreateDDSTextureFromFile(device, filename.c_str(), (ID3D11Resource**)tex, srv));

	return S_OK;

}