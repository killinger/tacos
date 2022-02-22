struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

cbuffer MVPBuffer
{
	float4x4 MVPMatrix;
};

Texture2D Texture;
SamplerState Sampler;

VS_OUTPUT VS(float4 InPosition : POSITION, float2 InTexCoord : TEXCOORD, float4 InNormal : NORMAL)
{
	VS_OUTPUT Output;
	Output.Position = mul(InPosition, MVPMatrix);
	Output.TexCoord = InTexCoord;
	
	return Output;
}

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	return Texture.Sample(Sampler, Input.TexCoord);
}
