struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

cbuffer MatrixBuffer
{
	float4x4 Transform;
};

cbuffer ColorBuffer
{
	float4 RectColor;
};

VS_OUTPUT VS(float4 InPosition : POSITION)
{
	VS_OUTPUT Output;
	Output.Position = mul(InPosition, Transform);
	return Output;
}

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	return RectColor;
}
