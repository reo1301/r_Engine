
struct VSInput
{
	float3 pos : POSITION;		// 頂点座標
	float4 color : COLOR;		// 頂点カラー
};

struct VSOutput
{
	float4 pos : SV_POSITION;		// 頂点座標
	float4 color : COLOR;			// 頂点カラー
};

VSOutput main(VSInput _input)
{
	VSOutput vsOut;
	
	float4 localPos = float4(_input.pos, 1.0);

	vsOut.pos = localPos;
	vsOut.color = _input.color;

	return vsOut;
}
