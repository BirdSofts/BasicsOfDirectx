// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,11.08.2019</created>
/// <changed>}Y{,17.08.2019</changed>
// ********************************************************************************

struct vertex
{
  float4 position : SV_POSITION;
  float4 colour : COLOR;
};

// vertex shader
vertex main ( float3 pos : POSITION, float3 col : COLOR )
{
  
  // input: vertex position and colour defined by six floats
  // indicated by the SV_POSITION and COLOR semantic
  // outputs:
  //-- transformation of position into homogenous coordinates (projective geometry),
  //-- passing the colour of each position to pixel shader

  vertex output; // outputed vertex structure

  float4 outputPos = {
    pos.x, pos.y, pos.z, 1.0f
  };
  output.position = outputPos;

  float4 outputCol = {
    col.x, col.y, col.z, 1.0f
  };
  output.colour = outputCol;

  return output;
};
