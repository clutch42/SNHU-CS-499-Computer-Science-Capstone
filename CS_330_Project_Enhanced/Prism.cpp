#include "Prism.h"
#include <cmath> // For M_PI
#include <glm/vec4.hpp> // For glm::vec4

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Constructor
Prism::Prism(int rad, int height, int sides) : prismRad(rad), prismHeight(height), prismSides(sides) {
    GeneratePrismTopVertices();
    GeneratePrismSideVertices();
    GeneratePrismSideIndices();
    GeneratePrismTopIndices();
    // Calculate sizes in bytes
    topVerticesSize = static_cast<GLsizei>(prismTopVertex.size() * sizeof(GLfloat));
    topIndicesSize = static_cast<GLsizei>(prismTopIndex.size() * sizeof(GLushort));
    sideVerticesSize = static_cast<GLsizei>(prismSideVertex.size() * sizeof(GLfloat));
    sideIndicesSize = static_cast<GLsizei>(prismSideIndex.size() * sizeof(GLushort));
}

// Destructor
Prism::~Prism() {}

// Getters for vertex and index sizes
GLsizei Prism::GetTopVerticesSize() const {
    return topVerticesSize;
}

GLsizei Prism::GetTopIndicesSize() const {
    return topIndicesSize;
}

GLsizei Prism::GetSideVerticesSize() const {
    return sideVerticesSize;
}

GLsizei Prism::GetSideIndicesSize() const {
    return sideIndicesSize;
}

// Getters for vertex and index arrays
GLfloat* Prism::GetTopVertices() {
    return prismTopVertex.data();
}

GLushort* Prism::GetTopIndices() {
    return prismTopIndex.data();
}

GLfloat* Prism::GetSideVertices() {
    return prismSideVertex.data();
}

GLushort* Prism::GetSideIndices() {
    return prismSideIndex.data();
}

// I created this function to automate the vertex positions for a prism with differing radius, height, and sides
// and assign it one of 6 colors unless it is the center top which is white, or center botttom which is black
void Prism::GeneratePrismTopVertices() {
    const int numColors = 6;
    prismTopVertex.insert(prismTopVertex.end(), { 0.0f, 0.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f, 0.5f, 0.5, 0.0f, 0.0f, -1.0f });
    prismTopVertex.insert(prismTopVertex.end(), { 0.0f, 0.0f, static_cast<GLfloat>(prismHeight), 0.0f,0.0f,0.0f,1.0f,0.5f,0.5f, 0.0f, 0.0f, 1.0f });
    for (int i = 0; i < prismSides; ++i) {
        GLfloat theta = 2.0f * M_PI * static_cast<float>(i) / prismSides;
        GLfloat x = prismRad * cos(theta);
        GLfloat y = prismRad * sin(theta);
        prismTopVertex.push_back((x));
        prismTopVertex.push_back((y));
        prismTopVertex.push_back(0.0f);

        // Assign colors with alpha to vertices based on the segment
        glm::vec4 color;
        int colorIndex = i % numColors;
        if (colorIndex == 0) {
            prismTopVertex.insert(prismTopVertex.end(), { 1.0f, 0.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 1) {
            prismTopVertex.insert(prismTopVertex.end(), { 0.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 2) {
            prismTopVertex.insert(prismTopVertex.end(), { 0.0f, 0.0f, 1.0f, 1.0f });
        }
        else if (colorIndex == 3) {
            prismTopVertex.insert(prismTopVertex.end(), { 1.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 4) {
            prismTopVertex.insert(prismTopVertex.end(), { 0.0f, 1.0f, 1.0f, 1.0f });
        }
        else {
            prismTopVertex.insert(prismTopVertex.end(), { 1.0f, 0.0f, 1.0f, 1.0f });
        }
        GLfloat textureX = cos(theta);
        GLfloat textureY = sin(theta);
        prismTopVertex.insert(prismTopVertex.end(), { (textureX + 1) / 2, (textureY + 1) / 2, 0.0f,0.0f, -1.0f });


        prismTopVertex.push_back(x);
        prismTopVertex.push_back(y);
        prismTopVertex.push_back(static_cast<GLfloat>(prismHeight));
        if (colorIndex == 0) {
            prismTopVertex.insert(prismTopVertex.end(), { 1.0f, 0.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 1) {
            prismTopVertex.insert(prismTopVertex.end(), { 0.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 2) {
            prismTopVertex.insert(prismTopVertex.end(), { 0.0f, 0.0f, 1.0f, 1.0f });
        }
        else if (colorIndex == 3) {
            prismTopVertex.insert(prismTopVertex.end(), { 1.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 4) {
            prismTopVertex.insert(prismTopVertex.end(), { 0.0f, 1.0f, 1.0f, 1.0f });
        }
        else {
            prismTopVertex.insert(prismTopVertex.end(), { 1.0f, 0.0f, 1.0f, 1.0f });
        }
        textureX = cos(theta);
        textureY = sin(theta);
        prismTopVertex.insert(prismTopVertex.end(), { (textureX + 1) / 2, (textureY + 1) / 2, 0.0f,0.0f, 1.0f });
    }
}

// I created this function to automate the vertex positions for a prism with differing radius, height, and sides
// and assign it one of 6 colors unless it is the center top which is white, or center botttom which is black
void Prism::GeneratePrismSideVertices() {
    const int numColors = 6;
    for (int i = 0; i < prismSides + 1; ++i) {
        GLfloat theta = 2.0f * M_PI * static_cast<float>(i) / prismSides;
        GLfloat x = prismRad * cos(theta);
        GLfloat y = prismRad * sin(theta);
        prismSideVertex.push_back((x));
        prismSideVertex.push_back((y));
        prismSideVertex.push_back(0.0f);

        // Assign colors with alpha to vertices based on the segment
        glm::vec4 color;
        int colorIndex = i % numColors;
        if (colorIndex == 0) {
            prismSideVertex.insert(prismSideVertex.end(), { 1.0f, 0.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 1) {
            prismSideVertex.insert(prismSideVertex.end(), { 0.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 2) {
            prismSideVertex.insert(prismSideVertex.end(), { 0.0f, 0.0f, 1.0f, 1.0f });
        }
        else if (colorIndex == 3) {
            prismSideVertex.insert(prismSideVertex.end(), { 1.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 4) {
            prismSideVertex.insert(prismSideVertex.end(), { 0.0f, 1.0f, 1.0f, 1.0f });
        }
        else {
            prismSideVertex.insert(prismSideVertex.end(), { 1.0f, 0.0f, 1.0f, 1.0f });
        }

        prismSideVertex.insert(prismSideVertex.end(), { 1.0f, static_cast<float>(i) / prismSides });
        // adding normals
        GLfloat normalX = cos(theta);
        GLfloat normalY = sin(theta);
        GLfloat normalZ = 0.0f;
        prismSideVertex.insert(prismSideVertex.end(), { normalX, normalY, normalZ });
        prismSideVertex.push_back(x);
        prismSideVertex.push_back(y);
        prismSideVertex.push_back(static_cast<GLfloat>(prismHeight));
        if (colorIndex == 0) {
            prismSideVertex.insert(prismSideVertex.end(), { 1.0f, 0.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 1) {
            prismSideVertex.insert(prismSideVertex.end(), { 0.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 2) {
            prismSideVertex.insert(prismSideVertex.end(), { 0.0f, 0.0f, 1.0f, 1.0f });
        }
        else if (colorIndex == 3) {
            prismSideVertex.insert(prismSideVertex.end(), { 1.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 4) {
            prismSideVertex.insert(prismSideVertex.end(), { 0.0f, 1.0f, 1.0f, 1.0f });
        }
        else {
            prismSideVertex.insert(prismSideVertex.end(), { 1.0f, 0.0f, 1.0f, 1.0f });
        }

        prismSideVertex.insert(prismSideVertex.end(), { 0.0f, static_cast<float>(i) / prismSides });
        // adding normals
        normalZ = 0.0f;
        prismSideVertex.insert(prismSideVertex.end(), { normalX, normalY, normalZ });
    } 
}

// I created this function to get indices for the prism
void Prism::GeneratePrismSideIndices() {
    // create top and bottom
    int vert = (prismSides + 1) * 2;

    // create sides
    for (int i = 0; i < vert; i++) {
        prismSideIndex.insert(prismSideIndex.end(), { static_cast<GLushort>(i), static_cast<GLushort>(i + 1), static_cast<GLushort>(i + 2) });
    }
}

// I created this function to get indices for the prism
void Prism::GeneratePrismTopIndices() {
    // create top and bottom
    int vert = (prismSides + 1) * 2;
    for (int i = 2; i < vert; i++) {
        if (i == 2) {
            prismTopIndex.insert(prismTopIndex.end(), { 0, static_cast<GLushort>(i), static_cast<GLushort>(vert - 2) });
        }
        else if (i == 3) {
            prismTopIndex.insert(prismTopIndex.end(), { 1, static_cast<GLushort>(i), static_cast<GLushort>(vert - 1) });
        }
        else if (i % 2 == 0) {
            prismTopIndex.insert(prismTopIndex.end(), { 0, static_cast<GLushort>(i), static_cast<GLushort>(i - 2) });
        }
        else {
            prismTopIndex.insert(prismTopIndex.end(), { 1, static_cast<GLushort>(i), static_cast<GLushort>(i - 2) });
        }
    }
}
