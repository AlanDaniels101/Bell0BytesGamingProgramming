float4 main( float3 pos : POSITION ) : SV_POSITION
{
	// Transform {x,y,z} input into homogeneous coordinates {wx, wy, wz, w}
	float4 homoPos = {pos.x, pos.y, pos.z, 1.0f};
	return homoPos;
}