Texture2D texture2d;
SamplerState samplerstate;

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 Texture : TEXCOORD0;
};

float4 main(VS_OUTPUT IN) : SV_TARGET
{
	return texture2d.Sample(samplerstate, IN.Texture);
}