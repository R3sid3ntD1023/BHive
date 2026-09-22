
bool SphereSphereIntersection(Sphere a, Sphere b)
{
    float d = length(a.Center - b.Center);
    return d <= (a.Radius + b.Radius) ? true : false;
}

bool SphereFrustumIntersection(Frustum frustum, Sphere s)
{
    for(int i = 0; i < 6; i++)
    {
        vec4 plane = frustum.planes[i];
        float dist = dot(s.Center, plane.xyz) + plane.w; 

        if(dist > s.Radius)
            return false;
    }

    return true;
}