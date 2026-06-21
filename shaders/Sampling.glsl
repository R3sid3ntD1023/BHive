
vec3 Downsample13Tap(sampler2D tex, vec2 uv, vec2 texel)
{
	vec3 c = vec3(0.0);
    
    //center
    c += textureLod(tex, uv, 0.0).rgb * 0.125;

    //4 direct neighbors
    c += textureLod(tex, uv + texel * vec2(1,   0),     0).rgb * 0.125;
    c += textureLod(tex, uv + texel * vec2(-1,  0),     0).rgb * 0.125;
    c += textureLod(tex, uv + texel * vec2(0,   1),     0).rgb * 0.125;
    c += textureLod(tex, uv + texel * vec2(0,   -1),    0).rgb * 0.125;
       
    //4 diagonals
    c += textureLod(tex, uv + texel * vec2(1,   1),     0).rgb * 0.0625;
    c += textureLod(tex, uv + texel * vec2(-1,  1),     0).rgb * 0.0625;
    c += textureLod(tex, uv + texel * vec2(1,   -1),    0).rgb * 0.0625;
    c += textureLod(tex, uv + texel * vec2(-1,  -1),    0).rgb * 0.0625;
                        
    //4 far taps (UE5 trick)
    c += textureLod(tex, uv + texel * vec2(2,  0),      0).rgb * 0.03125;
    c += textureLod(tex, uv + texel * vec2(-2,  0),     0).rgb * 0.03125;
    c += textureLod(tex, uv + texel * vec2(0,   2),     0).rgb * 0.03125;
    c += textureLod(tex, uv + texel * vec2(0,   -2),    0).rgb * 0.03125;

	return c;
}

vec3 Downsample13TapShared(in vec3 tile[20][20], ivec2 id)
{
	vec3 c = vec3(0.0);
    
    //center
    c += tile[id.y + 2][id.x + 2] * 0.125;

    //4 direct neighbors
    c += tile[id.y + 2][id.x + 3] * 0.125;
    c += tile[id.y + 2][id.x + 1] * 0.125;
    c += tile[id.y + 3][id.x + 2] * 0.125;
    c += tile[id.y + 1][id.x + 2] * 0.125;
       
    //4 diagonals
    c += tile[id.y + 3][id.x + 3] * 0.0625;
    c += tile[id.y + 3][id.x + 1] * 0.0625;
    c += tile[id.y + 1][id.x + 3] * 0.0625;
    c += tile[id.y + 1][id.x + 1] * 0.0625;
                 
    //4 far taps (UE5 trick)
    c += tile[id.y + 2][id.x + 4] * 0.03125;
    c += tile[id.y + 2][id.x + 0] * 0.03125;
    c += tile[id.y + 4][id.x + 2] * 0.03125;
    c += tile[id.y + 0][id.x + 2] * 0.03125;

	return c;
}

float Cubic(float x)
{
    x = abs(x);
    float x2 = x * x;
    float x3 = x2 * x;

    if(x <= 1.0)
        return 1.0 - 2.0 * x2 + x3; // 1 - 2x3 + x3
    else if(x < 2.0)
        return 4.0 - 8.0 * x + 5.0 * x2 - x3; //4- 8x + 5x2 - x3
    return 0.0;
}

vec3 BicubicSample(sampler2D tex, vec2 uv, vec2 texel)
{
    vec2 coord = uv / texel;
    vec2 base = floor(coord - 0.5);
    vec3 result = vec3(0.0);

    for(int j = - 1; j <= 2; j++)
    {
        float wy = Cubic((coord.y - (base.y + float(j))));
        for(int i = - 1; i <= 2; i++)
        {
            float wx = Cubic((coord.x - (base.x + float(i))));
            vec2 offset = (base + vec2(i , j) + 0.5) * texel;
            result += textureLod(tex, offset, 0.0).rgb * (wx * wy);
        }
    }

    return result;
}
