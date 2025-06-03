
struct PSInput
{
	float4 pos : SV_POSITION;		// 頂点座標
	float4 color : COLOR;			// 頂点カラー
};

float4 main(PSInput _input) : SV_TARGET
{
	float4 outColor = _input.color;

	return outColor;
}
