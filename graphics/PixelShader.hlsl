// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,11.08.2019</created>
/// <changed>}Y{,17.08.2019</changed>
// ********************************************************************************

// pixel shader
float4 main ( float4 pos : SV_POSITION, float4 col : COLOR ) : SV_TARGET
{
  
  // SV_TARGET: the semantic indicates that the return value of pixel shader should match the render target format
  
  return col; // colour
}
