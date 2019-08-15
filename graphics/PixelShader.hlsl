// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,11.08.2019</created>
/// <changed>}Y{,16.08.2019</changed>
// ********************************************************************************

// pixel shader
float4 main () : SV_TARGET
{
  // SV_TARGET: the semantic indicates that the return value of pixel shader should match the render target format
  return float4 ( 0.13f, 0.13f, 0.13f, 1.0f ); // colour
}
