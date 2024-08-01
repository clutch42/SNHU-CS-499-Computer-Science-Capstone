#ifndef PRISM_H
#define PRISM_H

#include <GL/glew.h> 
#include <vector>

// Created a class to hold all important variables
// and functions. To instantiate the class
// a radius, height, and a number of sides must be entered
// as an integer. Public getters for the arrays of vertices, indices,
// and size of both.
class Prism {
public:
    // Constructor and Destructor
    Prism(int rad, int height, int sides);
    ~Prism();

    // Getters for vertex and index arrays
    GLfloat* GetTopVertices();
    GLushort* GetTopIndices();
    GLfloat* GetSideVertices();
    GLushort* GetSideIndices();

    // Getters for vertex and index sizes
    GLsizei GetTopVerticesSize() const;
    GLsizei GetTopIndicesSize() const;
    GLsizei GetSideVerticesSize() const;
    GLsizei GetSideIndicesSize() const;

private:
    // Private member functions for generating vertices and indices
    void GeneratePrismTopVertices();
    void GeneratePrismSideVertices();
    void GeneratePrismSideIndices();
    void GeneratePrismTopIndices();

    // Private member variables
    std::vector<GLfloat> prismTopVertex;
    std::vector<GLfloat> prismSideVertex;
    std::vector<GLushort> prismSideIndex;
    std::vector<GLushort> prismTopIndex;

    GLsizei topVerticesSize;
    GLsizei topIndicesSize;
    GLsizei sideVerticesSize;
    GLsizei sideIndicesSize;

    int prismRad;
    int prismHeight;
    int prismSides;

};

#endif