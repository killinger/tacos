struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

cbuffer MatrixBuffer
{
	float4x4 ProjectionMatrix;
};

cbuffer ColorBuffer
{
	float4 TextColor;
};

Texture2D Texture;
SamplerState Sampler;

VS_OUTPUT VS(float4 InPosition : POSITION, float2 InTexCoord : TEXCOORD)
{
	VS_OUTPUT Output;
	Output.Position = mul(InPosition, ProjectionMatrix);
	Output.TexCoord = InTexCoord;
	
	return Output;
}

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	float4 Sample = { 1.0f, 1.0f, 1.0f, Texture.Sample(Sampler, Input.TexCoord).r };
	float4 Color = TextColor * Sample;
	return Color;

}
