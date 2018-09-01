struct VertexOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VertexOut main(float3 pos : POSITION, float3 col : COLOR)
{
	VertexOut vertexOutput;

	// Transform {x,y,z} input into homogeneous coordinates {wx, wy, wz, w}
	float4 outputPos = { pos.x, pos.y, pos.z, 1.0f };
	vertexOutput.position = outputPos;

	float4 outputCol = { col.x, col.y, col.z, 1.0f };
	vertexOutput.color = outputCol;

	return vertexOutput;
}