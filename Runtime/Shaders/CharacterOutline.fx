struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
};

cbuffer PerObjectBuffer
{
	float4x4 WorldMatrix;
	float4x4 MVPMatrix;
};

cbuffer PerFrameBuffer
{
	float4 OutlineColor;
	float OutlineThickness;
};

Texture2D Texture;
SamplerState Sampler;

VS_OUTPUT VS(float4 InPosition : POSITION, float2 InTexCoord : TEXCOORD, float4 InNormal : NORMAL)
{
	VS_OUTPUT Output;
	float3 Normal = normalize(InNormal.rgb);
	float3 Offset = Normal * 0.005;
	float3 Position = ((InPosition.rgb) + Offset);
	Output.Position = mul(float4(Position, InPosition.a), MVPMatrix);
	Output.TexCoord = InTexCoord;
	Output.Normal = mul(InNormal, WorldMatrix);

	return Output;
}

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	return OutlineColor;
}
