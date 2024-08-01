#include "Base.h"

// Constructor
Base::Base(int size) : baseSize(size) {
    GenerateBaseVertices();
    GenerateBaseIndices();
    // Calculate sizes in bytes
    baseVerticesSize = static_cast<GLsizei>(baseVertex.size() * sizeof(GLfloat));
    baseIndicesSize = static_cast<GLsizei>(baseIndex.size() * sizeof(GLushort));
}

// Destructor
Base::~Base() {}

// Getters for vertex and index sizes
GLsizei Base::GetBaseVerticesSize() const {
    return baseVerticesSize;
}

GLsizei Base::GetBaseIndicesSize() const {
    return baseIndicesSize;
}

// Getters for vertex and index arrays
GLfloat* Base::GetBaseVertices() {
    return baseVertex.data();
}

GLushort* Base::GetBaseIndices() {
    return baseIndex.data();
}

// Generate vertices
void Base::GenerateBaseVertices() {
    baseVertex = {
        // coordinates                                                              // color                    // tex coord    // normals
        static_cast<GLfloat>(baseSize), 0.0f,  static_cast<GLfloat>(baseSize),      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
       -static_cast<GLfloat>(baseSize), 0.0f,  static_cast<GLfloat>(baseSize),      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
        static_cast<GLfloat>(baseSize), 0.0f, -static_cast<GLfloat>(baseSize),      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
       -static_cast<GLfloat>(baseSize), 0.0f, -static_cast<GLfloat>(baseSize),      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f
    };
}

// Generate indices
void Base::GenerateBaseIndices() {
    baseIndex = {
        0, 1, 2,
        1, 2, 3
    };
}