struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

struct directional_light
{
	float3 Direction;
	float4 Ambient;
	float4 Diffuse;
};

struct point_light
{
	float4 Ambient;
	float4 Diffuse;
	float3 Position;
	float Attenuation;
};

cbuffer PerObjectBuffer
{
	float4x4 WorldMatrix;
	float4x4 MVPMatrix;
};

cbuffer PerFrameBuffer
{
	directional_light DirectionalLight;
	point_light PointLight;
};

Texture2D Texture;
SamplerState Sampler;

VS_OUTPUT VS(float4 InPosition : POSITION, float2 InTexCoord : TEXCOORD, float4 InNormal : NORMAL)
{
	VS_OUTPUT Output;
	Output.Position = mul(InPosition, MVPMatrix);
	Output.TexCoord = InTexCoord;
	Output.Normal = mul(InNormal, WorldMatrix);
	return Output;
}

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	Input.Normal = normalize(Input.Normal);
	float4 Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	float3 Result = Diffuse * Light.Ambient;
	Result += saturate(dot(Light.Direction, Input.Normal) * Light.Diffuse * Diffuse);
	return float4(Result, Diffuse.a);
}