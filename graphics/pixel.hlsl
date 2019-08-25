// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,11.08.2019</created>
/// <changed>}Y{,25.08.2019</changed>
// ********************************************************************************


// vertex input type
struct Vertex
{
  float4 position : SV_POSITION;
  float4 colour : COLOR;
};


// pixel shader
float4 main ( Vertex input ) : SV_TARGET
//float4 main ( float4 pos : SV_POSITION, float4 col : COLOR  ) : SV_TARGET
{
  
  // SV_TARGET: the semantic indicates that the return value of pixel shader should match the render target format
  
  return input.colour; // colour
  
}
