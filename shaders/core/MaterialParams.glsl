struct MaterialParams
{
	vec3 diffuse;
	vec3 emissive;
	float opacity;
	Material material;//BDRF specific struct
};

//Foward declarations
MaterialParams GetLambertParams(VS_OUT vs);
MaterialParams GetStandardParams(VS_OUT vs);
MaterialParams GetEmissiveParams(VS_OUT vs);