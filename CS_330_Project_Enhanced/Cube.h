#ifndef CUBE_H
#define CUBE_H

#include <GL/glew.h> 
#include <vector>

// Created a class to hold all important variables
// and functions. When instantiate the class
// nothing is entered as a parameter as this has a fixed size. 
// Public getters for the arrays of vertices, indices,
// and size of both.
class Cube {
public:
    // Constructor and Destructor
    Cube();
    ~Cube();

    // Getters for vertex and index arrays
    GLfloat* GetCubeMainVertices();
    GLushort* GetCubeMainIndices();
    GLfloat* GetCubeSideVertices();
    GLushort* GetCubeSideIndices();

    // Getters for vertex and index sizes
    GLsizei GetCubeMainVerticesSize() const;
    GLsizei GetCubeMainIndicesSize() const;
    GLsizei GetCubeSideVerticesSize() const;
    GLsizei GetCubeSideIndicesSize() const;

private:
    // Private member functions for generating vertices and indices
    void GenerateCubeMainVertices();
    void GenerateCubeSideVertices();
    void GenerateCubeSideIndices();
    void GenerateCubeMainIndices();

    // Private member variables
    std::vector<GLfloat> cubeMainVertex;
    std::vector<GLfloat> cubeSideVertex;
    std::vector<GLushort> cubeSideIndex;
    std::vector<GLushort> cubeMainIndex;

    GLsizei cubeMainVerticesSize;
    GLsizei cubeMainIndicesSize;
    GLsizei cubeSideVerticesSize;
    GLsizei cubeSideIndicesSize;

    int prismRad;
    int prismHeight;
    int prismSides;

};

#endif
