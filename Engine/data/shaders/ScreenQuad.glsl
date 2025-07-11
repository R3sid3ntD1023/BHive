#type vertex
#version 460 core

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTexCoord;


layout(location = 0) out struct vertex_output
{
	vec2 texcoord;
} vs_out;

void main()
{
	gl_Position = vec4(vPosition, 0.0, 1.0);

	vs_out.texcoord = vTexCoord;
}

#type fragment
#version 460 core

//https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
/*
=================================================================================================

  Baking Lab
  by MJP and David Neubelt
  http://mynameismjp.wordpress.com/

  All code licensed under the MIT license

=================================================================================================
 The code in this file was originally written by Stephen Hill (@self_shadow), who deserves all
 credit for coming up with this fit and implementing it. Buy him a beer next time you see him. :)
*/

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
mat3 ACESInputMat = mat3(
  0.59719, 0.35358, 0.04823,
  0.07600, 0.90834, 0.01566,
  0.02840, 0.13383, 0.83777
);

// ODT_SAT => XYZ => D60_2_D65 => sRGB
mat3 ACESOutputMat = mat3
(
  1.60475, -0.53108, -0.07367,
  -0.10208, 1.10813, -0.00605,
  -0.00327, -0.07276, 1.07602
);

vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

vec3 ACESFitted(vec3 color)
{
    color = transpose(ACESInputMat) * color;

    //Apply RTT and ODT
    color = RRTAndODTFit(color);
    color = transpose(ACESOutputMat) * color;
    color = clamp(color, 0, 1);
    return color;
}

vec3 ACES(vec3 color) {
  const float a = 2.51f;
  const float b = 0.03f;
  const float c = 2.43f;
  const float d = 0.59f;
  const float e = 0.14f;
  return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}


vec3 filmic(vec3 x) {
  vec3 X = max(vec3(0.0), x - 0.004);
  vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
  return pow(result, vec3(2.2));
}

layout(location = 0) in struct vertex_output
{
	vec2 texcoord;
} vs_in;

#define PP_ACES 1 << 0

layout(binding = 0) uniform sampler2D u_hdrtexture;
layout(binding = 1) uniform sampler2D u_bloomtexture;

#ifdef VULKAN
    layout(push_constant) uniform PushConstants
    {
        float u_BloomStrength;
        uint u_PostProcessMode;
    } constants;

#else

    layout(location = 0) uniform struct PushConstants
    {
        float u_BloomStrength;
        uint u_PostProcessMode;
    } constants;

#endif

layout(location = 0) out vec4 fs_out;

//const float gamma = 2.2;
//float exposure = 0.1;

void main()
{	
	
	vec4 hdr = texture(u_hdrtexture, vs_in.texcoord );
	vec4 bloom = texture(u_bloomtexture, vs_in.texcoord);

  switch(constants.u_PostProcessMode)
  {
    case PP_ACES:
      hdr.rgb = ACES(hdr.rgb);
      break;
    default:
      break;
  }

	vec3 final = mix(hdr.rgb, hdr.rgb + bloom.rgb, vec3(constants.u_BloomStrength));
  
	
	fs_out = vec4(final, 1);
}