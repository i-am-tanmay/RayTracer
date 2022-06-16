struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Texture : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float2 Texture : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	OUT.Position = IN.Position;
	OUT.Texture = IN.Texture;

	return OUT;
}