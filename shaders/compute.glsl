#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D outputImage;

uniform vec3 sphereCenter;
uniform float sphereRadius;
uniform vec3 cameraOrigin;

void main() {
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = pixelCoord / vec2(imageSize(outputImage));
    uv = uv * 2.0 - 1.0; // Map to range [-1, 1]

    // Ray origin and direction
    vec3 rayOrigin = cameraOrigin;
    vec3 rayDirection = normalize(vec3(uv, -1.0));

    // Sphere intersection
    vec3 oc = rayOrigin - sphereCenter;
    float a = dot(rayDirection, rayDirection);
    float b = 2.0 * dot(oc, rayDirection);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4.0 * a * c;

    vec4 color = vec4(0.0); // Default: black
    if (discriminant > 0.0) {
        float t = (-b - sqrt(discriminant)) / (2.0 * a);
        if (t > 0.0) {
            vec3 hitPoint = rayOrigin + t * rayDirection;
            vec3 normal = normalize(hitPoint - sphereCenter);
            color = vec4(normal * 0.5 + 0.5, 1.0); // Normal as color
        }
    }
    imageStore(outputImage, pixelCoord, color);
}