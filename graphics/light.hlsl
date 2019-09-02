//// ********************************************************************************
///// <summary>
///// 
///// </summary>
///// <created>}Y{,02.09.2019</created>
///// <changed>}Y{,02.09.2019</changed>
//// ********************************************************************************


//// global declarations
//// buffer object type, containing three matrices, updated on each execution
//cbuffer MatrixBuffer
//{
//  matrix worldMatrix;
//  matrix viewMatrix;
//  matrix projectionMatrix;
//};


//// type declarations
//// texture vertex/pixel (input type)
//struct Vertex
//{
//  float4 position : POSITION; // vertex shaders
//  float2 tex : TEXCOORD0;
//};


//// texture vertex/pixel (output type)
//struct Pixel
//{
//  float4 position : SV_POSITION; // pixel shaders
//  float2 tex : TEXCOORD0;
//};


//Texture2D shaderTexture; // texture resource
//SamplerState samplerType; // how the pixels are written on the polygon face

//// texture vertex/pixel (input type)
//struct Pixel
//{
//  float4 position : SV_POSITION;
//  float2 tex : TEXCOORD0;
//};


//// texture vertex shader: calculate the vertex location by matrices and prepare the output for texture pixel shader
//Pixel main( Vertex input ) // called by GPU when processing data from vertex buffer
//{
  
//  // input: vertex position and texture coordinate defined by six floats
//  // indicated by the POSITION and TEXCOORD0 semantic

//  // TEXCOORD0 semantic: texture coordinate: float U (width) and float V (height) (texture dimension, each from 0.0f to 1.0f)
//  // note that since multiple texture coordinates are allowed,
//  // the possibility is there to change the zero to any number, indicating the set of coordinates.
  
//  // process:
//  // vertex position calculation: (against world, view, and projection matrices)
  
//  // output:
//  //-- passing the texture coordinate of each position to pixel shader
  
//  Pixel output; // output vertex structure
  
//  // change the position vector to 4 units (proper matrix calculation)
//  input.position.w = 1.0f;
//  // manipulation of input vertex through world, view and projection matrices,
//  // resulting to the correct vertex location for 3D rendering, and then onto the 2D screen
//  output.position = mul(input.position, worldMatrix);
//  output.position = mul(output.position, viewMatrix);
//  output.position = mul(output.position, projectionMatrix);
//  // additionally store input texture coordinate (for pixel shader)
//  output.tex = input.tex;

//  return output;
  
//};


//// texture pixel shader: sampling from the texture draws each pixel on the polygons that will be rendered to the screen.
//float4 main( Pixel input ) : SV_TARGET // called by GPU when the output of vertex shader is ready
//{
  
//  // SV_TARGET: the semantic indicates that the return value of texture pixel shader should match the render target format
  
//  float4 textureColour;

//  // using sampler, sample the pixel colour from the texture at this texture coordinate location.
//  textureColour = shaderTexture.Sample(samplerType, input.tex);
  
//  return textureColour; // from texture sampled colour
  
//}
