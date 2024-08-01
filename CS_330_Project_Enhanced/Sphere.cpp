#include "Sphere.h"
#include <cmath> // For M_PI
#include <glm/vec4.hpp> // For glm::vec4

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Constructor
Sphere::Sphere(int rad, int sides) : sphereRadius(rad), sphereSides(sides) {
    GenerateSphereVertices();
    GenerateSphereIndices();
    // Calculate sizes in bytes
    sphereVerticesSize = static_cast<GLsizei>(sphereVertex.size() * sizeof(GLfloat));
    sphereIndicesSize = static_cast<GLsizei>(sphereIndex.size() * sizeof(GLushort));
}

// Destructor
Sphere::~Sphere() {}

// Getters for vertex and index sizes
GLsizei Sphere::GetSphereVerticesSize() const {
    return sphereVerticesSize;
}

GLsizei Sphere::GetSphereIndicesSize() const {
    return sphereIndicesSize;
}

// Getters for vertex and index arrays
GLfloat* Sphere::GetSphereVertices() {
    return sphereVertex.data();
}

GLushort* Sphere::GetSphereIndices() {
    return sphereIndex.data();
}

// Generates vertices for torus using int sphereRadius and int sphereSides from the constructor
void Sphere::GenerateSphereVertices() {
    for (int i = 0; i < sphereSides + 1; ++i) {
        GLfloat theta = 2.0f * M_PI * static_cast<float>(i) / (sphereSides / 2);
        for (int j = 0; j < sphereSides + 1; ++j) {
            GLfloat phi = 2.0f * M_PI * static_cast<float>(j) / sphereSides;
            GLfloat x = sphereRadius * cos(theta) * cos(phi);
            GLfloat y = sphereRadius * sin(theta) + sphereRadius;
            GLfloat z = sphereRadius * cos(theta) * sin(phi);
            sphereVertex.push_back(x);
            sphereVertex.push_back(y);
            sphereVertex.push_back(z);

            // insert color (white)
            sphereVertex.insert(sphereVertex.end(), { 1.0f, 1.0f, 1.0f, 1.0f });

            // insert texture coord
            sphereVertex.insert(sphereVertex.end(), { static_cast<float>(j) / sphereSides, static_cast<float>(i) / sphereSides });

            // adding normals
            GLfloat normalX = cos(theta) * cos(phi);
            GLfloat normalY = sin(theta);
            GLfloat normalZ = cos(theta) * sin(phi);

            sphereVertex.insert(sphereVertex.end(), { normalX, normalY, normalZ });
        }
    }
}

// Generates indices for sphere using int sphereSides
void Sphere::GenerateSphereIndices() {
    for (int i = 0; i < sphereSides; ++i) {
        for (int j = 0; j < sphereSides; ++j) {
            int current = i * sphereSides + j;
            int next = current + sphereSides + 1;

            // Triangle 1
            sphereIndex.push_back(current);
            sphereIndex.push_back(next);
            sphereIndex.push_back(next + 1);


            // Triangle 2
            sphereIndex.push_back(current);
            sphereIndex.push_back(next + 1);
            sphereIndex.push_back(current + 1);
        }
    }
}