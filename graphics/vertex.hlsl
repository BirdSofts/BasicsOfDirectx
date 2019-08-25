// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,11.08.2019</created>
/// <changed>}Y{,25.08.2019</changed>
// ********************************************************************************


// global declarations
// matrix type
cbuffer MatrixBuffer
{
  matrix worldMatrix;
  matrix viewMatrix;
  matrix projectionMatrix;
};


// type declarations
// vertex input
struct Vertex
{
  float4 position : POSITION;
  float4 colour : COLOR;
};
// pixel output type
struct Pixel
{
  float4 position : SV_POSITION;
  float4 colour : COLOR;
};


// vertex shader
Pixel main ( Vertex input )
{
  
  // input: vertex position and colour defined by seven floats
  // indicated by the POSITION and COLOR semantic
  
  ////xx process:
  //-- transformation of position into homogenous coordinates (projective geometry),
  
  // process:
  // vertex position calculation: (against world, view, and projection matrices)
  
  // output:
  //-- passing the colour of each position to pixel shader
  
  Pixel output; // output vertex structure
  
  //float4 outputPos = { input.position.x, input.position.y, input.position.z, input.position.w };
  //output.position = outputPos;
  //float4 outputCol = { input.colour.x, input.colour.y, input.colour.z, input.colour.w };
  //output.colour = outputCol;
  
  // change the position vector to 4 units (proper matrix calculation)
  input.position.w = 1.0f;
  output.position = mul(input.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);
  // additionally store input colour (for pixel shader)
  output.colour = input.colour;

  return output;
  
};
