#version 450

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D outputImage;

const float EPSILON = 1e-6;

float radians(float deg)
{
    return deg * 3.14159265359 / 180.0;
}

struct Material {
    vec3 albedo;
    vec3 emission;
    float emissionStrength;
    float roughness;
    float metallic;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

uniform struct Camera {
    vec3 lookFrom;
    vec3 lookAt;
    vec3 up;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    vec3 front;
    vec3 right;
} camera;

struct Light {
    vec3 position;
    vec3 color;
    float power;
};

layout(std430, binding = 3) buffer LightsBlock {
    Light lights[];
} LightsBuffer;

struct HitRecord {
    float t;
    vec3 position;
    vec3 normal;
    Material material;
};

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

layout(std430, binding = 1) buffer SpheresBlock {
    Sphere spheres[];
} SpheresBuffer;

bool intersectSphere(Ray ray, Sphere sphere, inout HitRecord rec)
{
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b*b - 4.0*a*c;

    if (discriminant < 0.0)
        return false;

    float tCandidate = (-b - sqrt(discriminant)) / (2.0 * a);
    if (tCandidate < 0.0)
        return false;

    if (tCandidate < rec.t)
    {
        rec.t        = tCandidate;
        rec.position = ray.origin + tCandidate * ray.direction;
        rec.normal   = normalize(rec.position - sphere.center);
        rec.material.albedo   = sphere.material.albedo;
        return true;
    }
    return false;
}


struct Triangle {
    vec3 v0;
    vec3 v1;
    vec3 v2;
    Material material;
};

layout(std430, binding = 2) buffer TrianglesBlock {
    Triangle triangles[];
} TrianglesBuffer;

bool intersectTriangle(Ray ray, Triangle tri, inout HitRecord rec)
{
    vec3 v0v1 = tri.v1 - tri.v0;
    vec3 v0v2 = tri.v2 - tri.v0;
    vec3 pvec = cross(ray.direction, v0v2);
    float det = dot(v0v1, pvec);

    if (abs(det) < EPSILON)
        return false;

    float invDet = 1.0 / det;
    vec3 tvec = ray.origin - tri.v0;
    float u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0)
        return false;

    vec3 qvec = cross(tvec, v0v1);
    float v = dot(ray.direction, qvec) * invDet;
    if (v < 0.0 || (u + v) > 1.0)
        return false;

    float tCandidate = dot(v0v2, qvec) * invDet;
    if (tCandidate < EPSILON)
        return false;

    if (tCandidate < rec.t)
    {
        rec.t        = tCandidate;
        rec.position = ray.origin + tCandidate * ray.direction;
        rec.normal   = normalize(cross(v0v1, v0v2));
        rec.material.albedo   = tri.material.albedo;
        return true;
    }
    return false;
}


Ray getCameraRay(Camera cam, float u, float v)
{
    vec3 forward = normalize(cam.lookAt - cam.lookFrom);
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
    r.origin    = cam.lookFrom;
    r.direction = rayDir;
    return r;
}

bool intersectScene(Ray ray, out HitRecord rec) {
    rec.t      = 1e30;
    rec.position = vec3(0.0);
    rec.normal = vec3(0.0);
    rec.material.albedo = vec3(0.0);

    bool hitAnything = false;

    for (int i = 0; i < SpheresBuffer.spheres.length(); i++) {
        if (intersectSphere(ray, SpheresBuffer.spheres[i], rec)) {
            hitAnything = true;
        }
    }

    for (int i = 0; i < TrianglesBuffer.triangles.length(); i++) {
        if (intersectTriangle(ray, TrianglesBuffer.triangles[i], rec)) {
            hitAnything = true;
        }
    }

    return hitAnything;
}

void main() 
{
    ivec2 pixelCoord    = ivec2(gl_GlobalInvocationID.xy);
    vec2 imgSize        = vec2(imageSize(outputImage));
    vec2 uv             = (pixelCoord + 0.5) / imgSize;  
    Ray ray             = getCameraRay(camera, uv.x, uv.y);

    HitRecord rec;
    bool hit = intersectScene(ray, rec);
    vec4 color = vec4(0.0);
    
    if (hit)
    {
        color = vec4(rec.material.albedo, 1.0);
    }
    else 
    {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }

    imageStore(outputImage, pixelCoord, color);
}
