#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D outputImage;

const float EPSILON = 1e-6;

float radians(float deg)
{
    return deg * 3.14159265359 / 180.0;
}

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

struct HitRecord {
    float t;
    vec3 position;
    vec3 normal;
    vec3 albedo;
    vec3 specular;
    float shininess;
};

struct Sphere {
    vec3 center;
    float radius;
    vec3 albedo;
    vec3 specular;
    float shininess;
};

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
        rec.albedo   = sphere.albedo;
        rec.specular = sphere.specular;
        rec.shininess= sphere.shininess;
        return true;
    }
    return false;
}


struct Triangle {
    vec3 v0;
    vec3 v1;
    vec3 v2;
    vec3 albedo;
    vec3 specular;
    float shininess;
};

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
        rec.albedo   = tri.albedo;
        rec.specular = tri.specular;
        rec.shininess= tri.shininess;
        return true;
    }
    return false;
}

bool intersectScene(Ray ray,
                    Sphere spheres[1], const int sphereCount,
                    Triangle tris[2], const int triCount,
                    out HitRecord rec)
{
    rec.t = 1e30;
    rec.position = vec3(0.0);
    rec.normal   = vec3(0.0);

    bool hitSomething = false;

    for(int i=0; i<sphereCount; i++)
    {
        if(intersectSphere(ray, spheres[i], rec))
        {
            hitSomething = true;
        }
    }

    for(int i=0; i<triCount; i++)
    {
        if(intersectTriangle(ray, tris[i], rec))
        {
            hitSomething = true;
        }
    }

    return hitSomething; 
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

bool inShadow(vec3 point, vec3 normal,
              Sphere spheres[1], const int sphereCount,
              Triangle tris[2], const int triCount,
              Light light)
{
    vec3 toLight  = light.position - point;
    float distL   = length(toLight);
    vec3 dirLight = normalize(toLight);

    Ray shadowRay;
    shadowRay.origin    = point + normal * 0.001;
    shadowRay.direction = dirLight;

    HitRecord shadowRec;
    bool hit = intersectScene(shadowRay,
                              spheres, sphereCount,
                              tris, triCount,
                              shadowRec);

    if (hit && shadowRec.t > 0.0 && shadowRec.t < distL)
        return true; 

    return false;
}

void main() 
{
    const int MAX_SPHERES = 1; Sphere spheres[MAX_SPHERES];
    spheres[0].center = vec3(0.0, 1.0, 0.0);
    spheres[0].radius = 1.0;
    spheres[0].albedo = vec3(1.0, 0.0, 0.0);
    spheres[0].specular = vec3(1.0, 1.0, 1.0);

    const int MAX_TRIANGLES = 2; Triangle triangles[MAX_TRIANGLES];
    triangles[0].v0     = vec3( 10.0, 0.0, 10.0);
    triangles[0].v1     = vec3( 10.0, 0.0, -10.0);
    triangles[0].v2     = vec3( -10.0, 0.0, 10.0);
    triangles[0].albedo = vec3(1.0, 1.0, 1.0);
    triangles[0].specular = vec3(1.0, 1.0, 1.0);
    triangles[0].shininess= 16.0;

    triangles[1].v0     = vec3( -10.0, 0.0, 10.0);
    triangles[1].v1     = vec3( 10.0, 0.0, -10.0);
    triangles[1].v2     = vec3( -10.0, 0.0, -10.0);
    triangles[1].albedo = vec3(1.0, 1.0, 1.0);
    triangles[1].specular = vec3(1.0, 1.0, 1.0);
    triangles[1].shininess= 16.0;

    const int MAX_LIGHTS = 1; Light lights[MAX_LIGHTS];
    lights[0].position = vec3(1.0, 3.0, 0.0);
    lights[0].color = vec3(1.0, 1.0, 1.0);
    lights[0].power = 1.0;        
    
    ivec2 pixelCoord    = ivec2(gl_GlobalInvocationID.xy);
    vec2 imgSize        = vec2(imageSize(outputImage));
    vec2 uv             = (pixelCoord + 0.5) / imgSize;  
    Ray ray             = getCameraRay(camera, uv.x, uv.y);

    HitRecord rec;
    bool hit = intersectScene(ray, spheres, MAX_SPHERES, triangles, MAX_TRIANGLES, rec);
    vec4 color = vec4(0.0);
    
    if (hit)
    {
        bool shadowed = inShadow(rec.position, rec.normal, 
                                spheres, MAX_SPHERES,
                                triangles, MAX_TRIANGLES,
                                lights[0]);

        if(!shadowed)
        {
            // Lambert
            vec3 L = normalize(lights[0].position - rec.position);
            float lambert = max(dot(rec.normal, L), 0.0);

            // Phong spec
            vec3 V = normalize(camera.lookFrom - rec.position);
            vec3 R = reflect(-L, rec.normal);
            float specAngle = max(dot(R, V), 0.0);
            float specular  = pow(specAngle, rec.shininess);

            vec3 diffuseColor  = rec.albedo   * lights[0].color * lambert;
            vec3 specularColor = rec.specular * lights[0].color * specular;

            vec3 finalColor = (diffuseColor + specularColor) * lights[0].power;
            color = vec4(finalColor, 1.0);
        }
        else
        {
            // shadowed
            color = vec4(0.1, 0.1, 0.1, 1.0);
        }
    }

    imageStore(outputImage, pixelCoord, color);
}
