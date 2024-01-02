#pragma once
#include <bits/stdc++.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
using namespace std;
using namespace glm;
const float PI = atan(1) * 4;
struct Planet {
    string name;
    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> uvs;
    vector<unsigned int> indices;
    vec3 center;
    bool light;

    float radius;
    int divisions;

    float rot;
    float orb;

    GLuint texture;

    Planet() : center(vec3(0, 0, 0)), radius(1), divisions(0) {}

    Planet(vec3 _center, float _radius, int _divisions) : center(_center), radius(_radius), divisions(_divisions) {}

    Planet(vec3 _center, float _radius, int _divisions, float _rot, float _orb) : center(_center), radius(_radius), divisions(_divisions), rot(_rot), orb(_orb) {
        // radius = glm::max(5.0f, glm::min(radius, 20.0f));
    }

    void generateSphere() {
        float step = 1.f / (float)(divisions - 1);

        for (int i = 0; i < divisions; i++) {
            float u = i * step;

            for (int j = 0; j < divisions; j++) {
                float v = j * step;
                vec3 pos = vec3(radius * cos(2.f * PI * u) * sin(PI * v),
                    radius * sin(2.f * PI * u) * sin(PI * v),
                    radius * cos(PI * v)) + center;

                vec3 normal = normalize(pos - center);

                positions.push_back(pos);
                normals.push_back(normal);
                uvs.push_back(vec2(u, v));
            }
        }

        for (int i = 0; i < divisions - 1; i++)
        {
            for (int j = 0; j < divisions - 1; j++)
            {
                unsigned int p00 = i * divisions + j;
                unsigned int p01 = i * divisions + j + 1;
                unsigned int p10 = (i + 1) * divisions + j;
                unsigned int p11 = (i + 1) * divisions + j + 1;

                indices.push_back(p00);
                indices.push_back(p10);
                indices.push_back(p01);

                indices.push_back(p01);
                indices.push_back(p10);
                indices.push_back(p11);
            }
        }
    }

    void rotatePlanet(vec3 axis, float theta) {
        axis = normalize(axis);
        float x = axis.x;
        float y = axis.y;
        float z = axis.z;
        float x2 = x * x;
        float y2 = y * y;
        float z2 = z * z;

        mat3 rMat = mat3(cos(theta) + x2 * (1 - cos(theta)), x * y * (1 - cos(theta)) - z * sin(theta), x * z * (1 - cos(theta)) + y * sin(theta),
            y * x * (1 - cos(theta)) + z * sin(theta), cos(theta) + y2 * (1 - cos(theta)), y * z * (1 - cos(theta)) - x * sin(theta),
            z * x * (1 - cos(theta)) - y * sin(theta), z * y * (1 - cos(theta)) + x * sin(theta), cos(theta) + z2 * (1 - cos(theta)));

        for (size_t i = 0; i < positions.size(); i++) {
            positions[i] = (rMat * (positions[i] - center)) + center;
            normals[i] = normalize(positions[i] - center);
        }
    }

    void orbitPlanet(vec3 parentCenter, vec3 axis, float theta) {
        axis = normalize(axis);
        float x = axis.x;
        float y = axis.y;
        float z = axis.z;
        float x2 = x * x;
        float y2 = y * y;
        float z2 = z * z;

        rotatePlanet(axis, -theta);

        mat3 rMat = mat3(cos(theta) + x2 * (1 - cos(theta)), x * y * (1 - cos(theta)) - z * sin(theta), x * z * (1 - cos(theta)) + y * sin(theta),
            y * x * (1 - cos(theta)) + z * sin(theta), cos(theta) + y2 * (1 - cos(theta)), y * z * (1 - cos(theta)) - x * sin(theta),
            z * x * (1 - cos(theta)) - y * sin(theta), z * y * (1 - cos(theta)) + x * sin(theta), cos(theta) + z2 * (1 - cos(theta)));

        center = (rMat * (center - parentCenter)) + parentCenter;

        for (size_t i = 0; i < positions.size(); i++) {
            positions[i] = (rMat * (positions[i] - parentCenter)) + parentCenter;
            normals[i] = normalize(positions[i] - center);
        }
    }

};
