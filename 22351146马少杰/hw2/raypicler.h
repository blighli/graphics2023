#pragma once
#include "planet.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

struct Ray {
    vec3 origin;
    vec3 direction;
};

static Ray getRayFromMouse(int mouseX, int mouseY, int screenWidth, int screenHeight, Camera& camera) {
    float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / screenHeight;

    vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);
    vec4 rayEye = inverse(camera.getMatrix()) * rayClip;
    rayEye = vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    vec4 rayWorld = inverse(mat4(camera.getMatrix())) * rayEye;
    vec3 rayDirection = normalize(vec3(rayWorld));

    return Ray{ camera.pos, rayDirection };
}

static bool rayIntersectsSphere(const Ray& ray, const Planet& planet, float& t) {
    vec3 v1 = planet.center - ray.origin;
    float tCenter = dot(v1, ray.direction);
    cross(v1, ray.direction);
    vec3 v2 = tCenter * ray.direction;
    vec3 closestPoint = ray.origin + tCenter * ray.direction;
    float distance = sqrt(distance2(closestPoint, planet.center));
    return distance <= planet.radius * tCenter + 1e-3 && tCenter >= 0;
}