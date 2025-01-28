#version 450

#define PI 3.141592653589793238462643
#define MAX_BOUNCES 10

uniform int SAMPLES;
uniform int BOUNCES;

uniform int uFrameCount;

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D outputImage;
layout (rgba32f, binding = 1) uniform image2D accumImage;

const float EPSILON = 1e-6;

float radians(float deg) {
    return deg * PI / 180.0;
}

// From https://github.com/asc-community/MxEngine
float rand(vec2 co, float seed) {
    return fract(sin(dot(co.xy + seed, vec2(12.9898, 78.233))) * 43758.5453123);
}

struct Vector3 {
    vec3 position;
    vec3 normal; 
};

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

layout(std430, binding = 4) buffer LightsBlock {
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

layout(std430, binding = 2) buffer SpheresBlock {
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

    float sqrtDisc = sqrt(discriminant);
    float tCandidate = (-b - sqrtDisc) / (2.0 * a);
    if (tCandidate < EPSILON)
        tCandidate = (-b + sqrtDisc) / (2.0 * a);
    if (tCandidate < EPSILON)
        return false;

    if (tCandidate < rec.t)
    {
        rec.t        = tCandidate;
        rec.position = ray.origin + tCandidate * ray.direction;
        rec.normal   = normalize(rec.position - sphere.center);
        rec.material = sphere.material;
        return true;
    }
    return false;
}

struct Triangle {
    Vector3 v0;
    Vector3 v1;
    Vector3 v2;
    Material material;
};

layout(std430, binding = 3) buffer TrianglesBlock {
    Triangle triangles[];
} TrianglesBuffer;

bool intersectTriangle(Ray ray, Triangle tri, inout HitRecord rec) {
    vec3 v0 = tri.v0.position;
    vec3 v1 = tri.v1.position;
    vec3 v2 = tri.v2.position;

    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    vec3 pvec = cross(ray.direction, edge2);

    float det = dot(edge1, pvec);

    if (abs(det) < EPSILON)
        return false;

    float invDet = 1.0 / det;

    vec3 tvec = ray.origin - v0;

    float u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0)
        return false;

    vec3 qvec = cross(tvec, edge1);
    float v = dot(ray.direction, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0)
        return false;

    float t = dot(edge2, qvec) * invDet;
    if (t < EPSILON || t > rec.t)
        return false;

    rec.t = t;
    rec.position = ray.origin + t * ray.direction;

    vec3 n0 = tri.v0.normal;
    vec3 n1 = tri.v1.normal;
    vec3 n2 = tri.v2.normal;

    vec3 interpolatedNormal = normalize((1.0 - u - v) * n0 + u * n1 + v * n2);
    rec.normal = interpolatedNormal;

    rec.material = tri.material;

    return true;
}

Ray getCameraRay(float u, float v)
{
    vec3 forward = normalize(camera.lookAt - camera.lookFrom);
    vec3 right   = normalize(cross(forward, camera.up));
    vec3 upVec   = cross(right, forward);

    float fovRad     = radians(camera.fov);
    float halfHeight = tan(fovRad * 0.5);
    float halfWidth  = camera.aspectRatio * halfHeight;  

    float x = 2.0 * u - 1.0;
    float y = 2.0 * v - 1.0;

    vec3 rayDir = normalize(forward 
                            + x * halfWidth  * right 
                            + y * halfHeight * upVec);

    Ray r;
    r.origin    = camera.lookFrom;
    r.direction = rayDir;
    return r;
}

vec3 sampleHemisphere(vec3 N, float seed, vec2 uv) {
    float Xi1 = rand(uv * vec2(12.9898, 78.233) + vec2(sin(seed), cos(seed)), seed);
    float Xi2 = rand(uv * vec2(78.233, 12.9898) + vec2(cos(seed), sin(seed)), seed + 1.0);

    float theta = acos(sqrt(1.0 - Xi1));
    float phi = 2.0 * PI * Xi2;

    float xs = sin(theta) * cos(phi);
    float ys = cos(theta);
    float zs = sin(theta) * sin(phi);

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangentX = normalize(cross(up, N));
    vec3 tangentY = cross(N, tangentX);

    vec3 direction = tangentX * xs + tangentY * zs + N * ys;
    return normalize(direction);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;

    return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 computeBRDF(Material material, vec3 N, vec3 V, vec3 L) {
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float VdotH = max(dot(V, H), 0.0);

    vec3 F0 = mix(vec3(0.04), material.albedo, material.metallic);
    vec3 F = fresnelSchlick(VdotH, F0);

    float D = DistributionGGX(N, H, material.roughness);
    float G = GeometrySmith(N, V, L, material.roughness);

    vec3 numerator = D * F * G;
    float denominator = 4.0 * NdotV * NdotL + 0.001;
    vec3 specular = numerator / denominator;

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - material.metallic;

    vec3 diffuse = kD * material.albedo / PI;

    return diffuse + specular;
}

struct AABB {
    vec3 min;
    vec3 max;
};

struct BVHNode {
    AABB bounds;
    int primitiveIndices[16];        //same size as the number of primitives in the node
    int leftChild;
    int rightChild;
};

layout(std430, binding = 6) buffer BVHNodesBlock {
    BVHNode nodes[];
} BVHBuffer;

bool intersectAABB(Ray ray, AABB aabb) {
    vec3 invDir = 1.0 / ray.direction;
    
    vec3 t0 = (aabb.min - ray.origin) * invDir;
    vec3 t1 = (aabb.max - ray.origin) * invDir;
    
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);
    
    float tNear = max(max(tmin.x, tmin.y), tmin.z);
    float tFar = min(min(tmax.x, tmax.y), tmax.z);
    
    return tNear <= tFar && tFar > 0;
}

bool intersectBVH(Ray ray, inout HitRecord hitRecord) {
    const int MAX_STACK = 64;
    int stack[MAX_STACK];
    int stackPtr = 0;
    
    stack[stackPtr++] = 0; // Push root node
    bool hit = false;
    
    while (stackPtr > 0 && stackPtr < MAX_STACK) {
        int nodeIdx = stack[--stackPtr];
        BVHNode node = BVHBuffer.nodes[nodeIdx];
        
        if (!intersectAABB(ray, node.bounds))
            continue;
            
        if (node.leftChild == -1) { // Leaf node
            // Iterate through all primitives in the node
            for (int i = 0; i < 16; i++) {
                int triIdx = node.primitiveIndices[i];
                if (triIdx == -1) break; // Stop if we hit an unused index
                
                Triangle tri = TrianglesBuffer.triangles[triIdx];
                
                HitRecord tempHit = hitRecord;
                if (intersectTriangle(ray, tri, tempHit)) {
                    if (tempHit.t < hitRecord.t) {
                        hitRecord = tempHit;
                        hit = true;
                    }
                }
            }
        } else {
            stack[stackPtr++] = node.leftChild;
            stack[stackPtr++] = node.rightChild;
        }
    }
    
    return hit;
}

bool traceRay(Ray ray, out HitRecord hitRecord) {
    hitRecord.t      = 1e20;
    hitRecord.position = vec3(0.0);
    hitRecord.normal = vec3(0.0);
    hitRecord.material.albedo = vec3(0.0);
    hitRecord.material.emission = vec3(0.0);
    hitRecord.material.emissionStrength = 0.0;
    hitRecord.material.roughness = 0.0;
    hitRecord.material.metallic = 0.0;

    bool hitSomething = false;

    // Test spheres
    for (int i = 0; i < SpheresBuffer.spheres.length(); ++i) {
        if (intersectSphere(ray, SpheresBuffer.spheres[i], hitRecord)) {
            hitSomething = true;
        }
    }

    // Test all triangles using BVH
    if (intersectBVH(ray, hitRecord)) {
        hitSomething = true;
    }

    return hitSomething;
}

void main() 
{
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 imgSize = vec2(imageSize(outputImage));
    vec3 color = vec3(0.0);

    for(int sampleIndex = 0; sampleIndex < SAMPLES; ++sampleIndex) {
        float seed = float(uFrameCount) + float(pixelCoord.x) / imgSize.x + float(pixelCoord.y) / imgSize.y;
        float offsetX = rand(vec2(pixelCoord), seed);
        float offsetY = rand(vec2(pixelCoord), seed+1.0);
        vec2 randOffset = vec2(offsetX, offsetY);
        vec2 uv = (vec2(pixelCoord) + randOffset) / imgSize;

        Ray ray = getCameraRay(uv.x, uv.y); 

        vec3 throughput = vec3(1.0);

        for(int bounce = 0; bounce < BOUNCES && bounce < MAX_BOUNCES; ++bounce) {
            HitRecord hitRecord;

            if(traceRay(ray, hitRecord)) {
                color += throughput * hitRecord.material.emission * hitRecord.material.emissionStrength;

                vec3 N = normalize(hitRecord.normal);
                vec3 V = normalize(-ray.direction);

                int numLights = LightsBuffer.lights.length();
                for(int i = 0; i < numLights; ++i) {
                    Light light = LightsBuffer.lights[i];
                    vec3 L = normalize(light.position - hitRecord.position);
                    float distance = length(light.position - hitRecord.position);
                    float attenuation = 1.0 / (distance * distance);

                    Ray shadowRay;
                    shadowRay.origin = hitRecord.position + N * 0.001;
                    shadowRay.direction = L;

                    HitRecord shadowHit;
                    if(!traceRay(shadowRay, shadowHit) || length(shadowHit.position - hitRecord.position) > distance) {
                        vec3 BRDF = computeBRDF(hitRecord.material, N, V, L);

                        float NdotL = max(dot(N, L), 0.0);

                        vec3 radiance = light.color * light.power * attenuation;
                        color += throughput * BRDF * radiance * NdotL;
                    }
                }

                vec3 scatterDir = sampleHemisphere(N, float(sampleIndex * BOUNCES + bounce), uv);

                vec3 BRDF = computeBRDF(hitRecord.material, N, V, scatterDir);

                float NdotScatterDir = max(dot(N, scatterDir), 0.0);
                float pdf = NdotScatterDir / PI;

                throughput *= BRDF * NdotScatterDir / pdf;

                ray.origin = hitRecord.position + N * EPSILON;
                ray.direction = scatterDir;

                if(length(throughput) < 0.001)
                    break;

            } else {
                break;
            }
        }
    }

    color /= float(SAMPLES);
    
    //  gamma correction
    color = pow(color, vec3(1.0 / 2.6));

    vec4 accumColor = imageLoad(accumImage, pixelCoord);

    // new accumulated color
    accumColor.rgb = (accumColor.rgb * float(uFrameCount - 1) + color.rgb) / float(uFrameCount);
    accumColor.a = 1.0;

    imageStore(accumImage, pixelCoord, accumColor);
}
