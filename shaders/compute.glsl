#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D outputImage;

struct Sphere {
    vec3 center;
    float radius;
};


void main() 
{
    Sphere sphere;
    sphere.center = vec3(0.0, 0.0, 0.0);
    sphere.radius = 1.0;
    
    vec3 cameraOrigin = vec3(1,1,1);

    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv = pixelCoord / vec2(imageSize(outputImage));
    uv = uv * 2.0 - 1.0;

    vec3 rayOrigin    = cameraOrigin;
    vec3 rayDirection = normalize(vec3(uv, -1.0));

    vec3 oc = rayOrigin - sphere.center;
    float a = dot(rayDirection, rayDirection);
    float b = 2.0 * dot(oc, rayDirection);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b*b - 4.0*a*c;

    vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

    if (discriminant > 0.0) 
    {
        float t = (-b - sqrt(discriminant)) / (2.0 * a);
        if (t > 0.0) 
        {
            vec3 hitPoint = rayOrigin + t * rayDirection;
            vec3 normal   = normalize(hitPoint - sphere.center);

            color = vec4(normal * 0.5 + 0.5, 1.0);
        }
    }

    imageStore(outputImage, pixelCoord, color);

    /*ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

    imageStore(outputImage, pixelCoord, color);*/
}
