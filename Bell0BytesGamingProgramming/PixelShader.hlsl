float4 main() : SV_TARGET
{
	// Pick up only the red and green parts, making yellow (the SV_TARGET/COLOR is RGBA)
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}