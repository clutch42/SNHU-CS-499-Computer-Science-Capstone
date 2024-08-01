#include "Cube.h"
#include <cmath> // For M_PI
#include <glm/vec4.hpp> // For glm::vec4

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Constructor
Cube::Cube() {
    GenerateCubeMainVertices();
    GenerateCubeSideVertices();
    GenerateCubeSideIndices();
    GenerateCubeMainIndices();
    // Calculate sizes in bytes
    cubeMainVerticesSize = static_cast<GLsizei>(cubeMainVertex.size() * sizeof(GLfloat));
    cubeMainIndicesSize = static_cast<GLsizei>(cubeMainIndex.size() * sizeof(GLushort));
    cubeSideVerticesSize = static_cast<GLsizei>(cubeSideVertex.size() * sizeof(GLfloat));
    cubeSideIndicesSize = static_cast<GLsizei>(cubeSideIndex.size() * sizeof(GLushort));
}

// Destructor
Cube::~Cube() {}


// Getters for vertex and index sizes
GLsizei Cube::GetCubeMainVerticesSize() const {
    return cubeMainVerticesSize;
}

GLsizei Cube::GetCubeMainIndicesSize() const {
    return cubeMainIndicesSize;
}

GLsizei Cube::GetCubeSideVerticesSize() const {
    return cubeSideVerticesSize;
}

GLsizei Cube::GetCubeSideIndicesSize() const {
    return cubeSideIndicesSize;
}

// Getters for vertex and index arrays
GLfloat* Cube::GetCubeMainVertices() {
    return cubeMainVertex.data();
}

GLushort* Cube::GetCubeMainIndices() {
    return cubeMainIndex.data();
}

GLfloat* Cube::GetCubeSideVertices() {
    return cubeSideVertex.data();
}

GLushort* Cube::GetCubeSideIndices() {
    return cubeSideIndex.data();
}

// Generates cube of size 1 x 1 x 1. Have to use OpenGL to change shape.
void Cube::GenerateCubeSideVertices() {
    cubeSideVertex = {
        // coordinates          // color                    // tex coord    // normals
        0.5f,  1.0f,  0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
        0.5f,  1.0f,  -0.5f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
        0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f, 0.0f,  -0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     1.0f, 0.0f, 0.0f,

       -0.5f,  1.0f,  0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
       -0.5f,  1.0f,  -0.5f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
       -0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
       -0.5f, 0.0f,  -0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,

        0.5f,  1.0f,  0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
       -0.5f,  1.0f,  0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
       -0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, 0.0f, 1.0f,

        0.5f,  1.0f,  -0.5f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
       -0.5f,  1.0f,  -0.5f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
        0.5f, 0.0f,  -0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
       -0.5f, 0.0f,  -0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, -1.0f
    };
}

// Generates cube of size 1 x 1 x 1. Have to use OpenGL to change shape.
void Cube::GenerateCubeMainVertices() {
    cubeMainVertex = {
        // coordinates          // color                    // tex coord    // normals
        0.5f,  1.0f,  0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
       -0.5f,  1.0f,  0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
        0.5f,  1.0f,  -0.5f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
       -0.5f,  1.0f,  -0.5f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f,

        0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
       -0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, -1.0f, 0.0f,
        0.5f, 0.0f,  -0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
       -0.5f, 0.0f,  -0.5f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, -1.0f, 0.0f
    };
}

// Generate indices for the cube
void Cube::GenerateCubeSideIndices() {
    cubeSideIndex = {
        0, 1, 2,
        1, 2, 3,

        4, 5, 6,
        5, 6, 7,

        8, 9, 10,
        9, 10, 11,

        12, 13, 14,
        13, 14, 15
    };
}

// Generate indices for the cube
void Cube::GenerateCubeMainIndices() {
    cubeMainIndex = {
        0, 1, 2,
        1, 2, 3,

        4, 5, 6,
        5, 6, 7
    };
}
