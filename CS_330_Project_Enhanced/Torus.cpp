#include "Torus.h"
#include <cmath> // For M_PI
#include <glm/vec4.hpp> // For glm::vec4

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Constructor
Torus::Torus(float distFromCenter, float radius, int sides) : torusDistFromCenter(distFromCenter), torusRadius(radius), torusSides(sides) {
    GenerateTorusVertices();
    GenerateTorusIndices();
    // Calculate sizes in bytes
    torusVerticesSize = static_cast<GLsizei>(torusVertex.size() * sizeof(GLfloat));
    torusIndicesSize = static_cast<GLsizei>(torusIndex.size() * sizeof(GLushort));
}

// Destructor
Torus::~Torus() {}

// Getters for vertex and index sizes
GLsizei Torus::GetTorusVerticesSize() const {
    return torusVerticesSize;
}

GLsizei Torus::GetTorusIndicesSize() const {
    return torusIndicesSize;
}

// Getters for vertex and index arrays
GLfloat* Torus::GetTorusVertices() {
    return torusVertex.data();
}

GLushort* Torus::GetTorusIndices() {
    return torusIndex.data();
}

// Generates vertices for torus using float torusDistFromCenter,
// float torusRadius, int torusSides  from the constructor
void Torus::GenerateTorusVertices() {
    for (int i = 0; i < torusSides + 1; ++i) {
        GLfloat theta = 2.0f * M_PI * static_cast<float>(i) / torusSides;
        for (int j = 0; j < torusSides + 1; ++j) {
            GLfloat phi = 2.0f * M_PI * static_cast<float>(j) / torusSides;

            GLfloat x = (torusDistFromCenter + torusRadius + torusRadius * cos(phi)) * cos(theta);
            GLfloat y = torusRadius * sin(phi) + torusRadius;
            GLfloat z = (torusDistFromCenter + torusRadius + torusRadius * cos(phi)) * sin(theta);

            torusVertex.push_back(x);
            torusVertex.push_back(y);
            torusVertex.push_back(z);

            // insert color (white)
            torusVertex.insert(torusVertex.end(), { 1.0f, 1.0f, 1.0f, 1.0f });

            // insert texture coord
            torusVertex.insert(torusVertex.end(), { static_cast<float>(j) / torusSides, static_cast<float>(i) / torusSides });

            // adding normals
            GLfloat normalX = cos(theta) * cos(phi);
            GLfloat normalY = sin(theta);
            GLfloat normalZ = sin(theta) * cos(phi);

            torusVertex.insert(torusVertex.end(), { normalX, normalY, normalZ });
        }
    }
}

// Generates indices for torus using int torusSides
void Torus::GenerateTorusIndices() {
    for (int i = 0; i < torusSides; ++i) {
        for (int j = 0; j < torusSides; ++j) {
            // Calculate indices for the quad
            GLuint index0 = i * (torusSides + 1) + j;
            GLuint index1 = index0 + 1;
            GLuint index2 = (i + 1) * (torusSides + 1) + j;
            GLuint index3 = index2 + 1;

            // Form two triangles from the quad
            torusIndex.push_back(index0);
            torusIndex.push_back(index1);
            torusIndex.push_back(index2);

            torusIndex.push_back(index1);
            torusIndex.push_back(index3);
            torusIndex.push_back(index2);
        }
    }
}