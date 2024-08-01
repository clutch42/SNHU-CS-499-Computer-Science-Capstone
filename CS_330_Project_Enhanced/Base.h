#ifndef BASE_H
#define BASE_H

#include <GL/glew.h> 
#include <vector>

// Created a class to hold all important variables
// and functions. To instantiate the class
// a size must be entered
// as an integer. Public getters for the arrays of vertices, indices,
// and size of both.
class Base {
public:
    // Constructor and Destructor
    Base(int size);
    ~Base();

    // Getters for vertex and index arrays
    GLfloat* GetBaseVertices();
    GLushort* GetBaseIndices();
    
    // Getters for vertex and index sizes
    GLsizei GetBaseVerticesSize() const;
    GLsizei GetBaseIndicesSize() const;

private:
    // Private member functions for generating vertices and indices
    void GenerateBaseVertices();
    void GenerateBaseIndices();

    // Private member variables
    std::vector<GLfloat> baseVertex;
    std::vector<GLushort> baseIndex;

    GLsizei baseVerticesSize;
    GLsizei baseIndicesSize;

    int baseSize;
};

#endif
