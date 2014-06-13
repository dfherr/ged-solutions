//--------------------------------------------------------------------------------------
// Shader resources
//--------------------------------------------------------------------------------------

Texture2D   g_Diffuse;		// Material albedo for diffuse lighting
Texture2D	g_NormalMap;	// added, see assignment 5
Buffer<float> g_HeightMap;	// added, see assignment 5
Texture2D	g_Specular;	// added assignment 06
Texture2D	g_Glow;		// added assignment 06


//--------------------------------------------------------------------------------------
// Constant buffers
//--------------------------------------------------------------------------------------

cbuffer cbConstant
{
    float4  g_LightDir; // Object space
	int		g_TerrainRes; // added, see assignment 5
};

cbuffer cbChangesEveryFrame
{
    matrix  g_World;
    matrix  g_WorldViewProjection;
	matrix	g_WorldNormals; // added, see assignment 5
    float   g_Time;
	float4	g_CameraPosWorld; // added assignment 6
};

cbuffer cbUserChanges
{
};


//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------

struct PosNorTex
{
    float4 Pos : SV_POSITION;
    float4 Nor : NORMAL;
    float2 Tex : TEXCOORD;
};

struct PosTexLi
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
    float   Li : LIGHT_INTENSITY;
	float3 normal: NORMAL;
};

struct PosTex // added, see assignment 5
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

 // added assignment 6
struct T3dVertexVSIn { 
	float3 Pos : POSITION; //Position in object space 
	float2 Tex : TEXCOORD; //Texture coordinate 
	float3 Nor : NORMAL; //Normal in object space 
	float3 Tan : TANGENT; //Tangent in object space (not used in Ass. 5) 
}; 

 // added assignment 6
struct T3dVertexPSIn { 
	float4 Pos : SV_POSITION; //Position in clip space 
	float2 Tex : TEXCOORD; //Texture coordinate 
	float3 PosWorld : POSITION; //Position in world space 
	float3 NorWorld : NORMAL; //Normal in world space 
	float3 TanWorld : TANGENT; //Tangent in world space (not used in Ass. 5) 
};

//--------------------------------------------------------------------------------------
// Samplers
//--------------------------------------------------------------------------------------

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samLinearClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

//--------------------------------------------------------------------------------------
// Rasterizer states
//--------------------------------------------------------------------------------------

RasterizerState rsDefault {
};

RasterizerState rsCullFront {
    CullMode = Front;
};

RasterizerState rsCullBack {
    CullMode = Back;
};

RasterizerState rsCullNone {
	CullMode = None; 
};

RasterizerState rsLineAA {
	CullMode = None; 
	AntialiasedLineEnable = true;
};


//--------------------------------------------------------------------------------------
// DepthStates
//--------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};


//--------------------------------------------------------------------------------------
// Shaders
//--------------------------------------------------------------------------------------
/* No longer used render calls
PosTexLi SimpleVS(PosNorTex Input) {
    PosTexLi output = (PosTexLi) 0;

    // Transform position from object space to homogenious clip space
    output.Pos = mul(Input.Pos, g_WorldViewProjection);

    // Pass trough normal and texture coordinates
    output.Tex = Input.Tex;

    // Calculate light intensity
    output.normal = normalize(mul(Input.Nor, g_World).xyz); // Assume orthogonal world matrix
    output.Li = saturate(dot(output.normal, g_LightDir.xyz));
        
    return output;
}

float4 SimplePS(PosTexLi Input) : SV_Target0 {
    // Perform lighting in object space, so that we can use the input normal "as it is"
    //float4 matDiffuse = g_Diffuse.Sample(samAnisotropic, Input.Tex);
    float4 matDiffuse = g_Diffuse.Sample(samLinearClamp, Input.Tex);
    return float4(matDiffuse.rgb * Input.Li, 1);
	//return float4(Input.normal, 1);
}
*/
PosTex TerrainVS(uint VertexID : SV_VertexID ){
	PosTex output = (PosTex) 0;
	float x = (VertexID % g_TerrainRes) / (float)g_TerrainRes; // matrix column
	float y = g_HeightMap[VertexID];
	float z = (VertexID / g_TerrainRes) / (float)g_TerrainRes;	// matrix row

	// -0.5f to center at [0,0,0]
	output.Pos = float4(x-0.5f,y, z-0.5f, 1.0f);
	output.Pos = mul(output.Pos, g_WorldViewProjection);
	output.Tex.x = x;
	output.Tex.y = z;

	return output;
}

float4 TerrainPS(PosTex input) : SV_Target0 {
	float3 n;
	n.xz = g_NormalMap.Sample(samAnisotropic, input.Tex).xy;
	// map back to range -1, 1
	n.x = (n.x * 2.0f) - 1.0f;
	n.z = (n.z * 2.0f) - 1.0f;
	n.y = sqrt(1.0 - (n.x*n.x) - (n.z * n.z));
	n = normalize(mul(float4(n,0), g_WorldNormals).xyz);

	float3 matDiffuse = g_Diffuse.Sample(samLinearClamp, input.Tex).xyz;
	float i = saturate(dot(n, normalize(g_LightDir.xyz)));
	return float4(matDiffuse * i, 1);
}

T3dVertexPSIn MeshVS(T3dVertexVSIn input){
	T3dVertexPSIn output = (T3dVertexPSIn) 0;

	output.Pos = mul(float4(input.Pos,1),g_WorldViewProjection);
	output.Tex = input.Tex;
	float4 worldpos = mul(float4(input.Pos,1), g_World);
	output.PosWorld = worldpos.xyz/worldpos.w; 
	output.NorWorld = normalize(mul(float4(input.Nor,0), g_WorldNormals).xyz);
	output.TanWorld = normalize(mul(float4(input.Tan,0), g_WorldNormals).xyz);

	return output;
}


float4 MeshPS(T3dVertexPSIn input) : SV_Target0 {
	float4 matDiffuse, matSpecular, matGlow, colLight, colLightAmbient;
	float3 n, r, v;
	float cd = 0.5f;
	float cs = 0.4f;
	float ca = 0.15f;
	float cg = 0.5f;
	colLight = float4(1.0f, 1.0f, 1.0f, 1.0f);
	colLightAmbient = float4(1.0f, 1.0f, 1.0f, 1.0f);
	matDiffuse = g_Diffuse.Sample(samAnisotropic, input.Tex);
	matSpecular = g_Specular.Sample(samAnisotropic, input.Tex);
	matGlow = g_Glow.Sample(samAnisotropic, input.Tex);
	n = normalize(input.NorWorld);
	r = reflect(-(g_LightDir.xyz), n);
	v = normalize(g_CameraPosWorld.xyz - input.PosWorld);

	return (cd * matDiffuse * saturate(dot(n, g_LightDir.xyz)) * colLight + 
	 cs * matSpecular * pow(saturate(dot(r, v)),10) * colLight +
	 ca * matDiffuse * colLightAmbient + cg * matGlow);
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique11 Render
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, TerrainVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TerrainPS()));
        
        SetRasterizerState(rsCullNone);
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
	
	pass P1_Mesh
    {
        SetVertexShader(CompileShader(vs_4_0, MeshVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, MeshPS()));
        
        SetRasterizerState(rsCullBack);
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}
