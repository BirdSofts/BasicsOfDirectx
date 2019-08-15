// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,11.08.2019</created>
/// <changed>}Y{,16.08.2019</changed>
// ********************************************************************************

// vertex shader
float4 main(float4 pos : POSITION) : SV_POSITION
{
  // input: vertex position defined by three floats
  // output: transformation of position into homogenous coordinates (projective geometry),
  // defined by the SV_POSITION semantic
  float4 homoPos =
  {
    pos.x, pos.y, pos.z, 1.0f
  };
  
  return homoPos;

}
