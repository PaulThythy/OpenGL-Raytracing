#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D outputImage;

float radians(float deg)
{
    return deg * 3.14159265359 / 180.0;
}

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Camera {
    vec3 origin;
    vec3 lookAt;
    vec3 up;
    float fov;
    float aspectRatio;
};

struct Sphere {
    vec3 center;
    float radius;
};

Ray getCameraRay(Camera cam, float u, float v)
{
    vec3 forward = normalize(cam.lookAt - cam.origin);
    vec3 right   = normalize(cross(forward, cam.up));
    vec3 upVec   = cross(right, forward);

    float fovRad     = radians(cam.fov);
    float halfHeight = tan(fovRad * 0.5);
    float halfWidth  = cam.aspectRatio * halfHeight;  

    float x = 2.0 * u - 1.0;
    float y = 2.0 * v - 1.0;

    vec3 rayDir = normalize(forward 
                            + x * halfWidth  * right 
                            + y * halfHeight * upVec);

    Ray r;
    r.origin    = cam.origin;
    r.direction = rayDir;
    return r;
}

void main() 
{
    Sphere sphere;
    sphere.center = vec3(0.0, 0.0, 0.0);
    sphere.radius = 1.0;
    
    Camera cam;
    cam.origin = vec3(4, 4, 4);       
    cam.lookAt = vec3(0, 0, 0);     
    cam.up     = vec3(0, 1, 0);       
    cam.fov    = 60.0;           
    
    ivec2 pixelCoord        = ivec2(gl_GlobalInvocationID.xy);
    vec2 imgSize            = vec2(imageSize(outputImage));
    cam.aspectRatio         = imgSize.x / imgSize.y;
    
    vec2 uv        = (pixelCoord + 0.5) / imgSize;  

    Ray ray = getCameraRay(cam, uv.x, uv.y);
    
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b*b - 4.0*a*c;

    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    if (discriminant > 0.0) 
    {
        float t = (-b - sqrt(discriminant)) / (2.0 * a);
        if (t > 0.0) 
        {
            vec3 hitPoint = ray.origin + t * ray.direction;
            vec3 normal   = normalize(hitPoint - sphere.center);

            color = vec4(normal * 0.5 + 0.5, 1.0);
        }
    }

    imageStore(outputImage, pixelCoord, color);
}
