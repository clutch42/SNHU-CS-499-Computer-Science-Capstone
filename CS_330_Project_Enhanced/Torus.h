#ifndef TORUS_H
#define TORUS_H

#include <GL/glew.h> 
#include <vector>

// Created a class to hold all important variables
// and functions. To instantiate the class
// a distance from center and radius must be 
// entered as a float and a number of sides must be entered
// as an integer. Public getters for the arrays of vertices, indices,
// and size of both.
class Torus {
public:
    // Constructor and Destructor
    Torus(float distFromCenter, float radius, int sides);
    ~Torus();

    // Getters for vertex and index arrays
    GLfloat* GetTorusVertices();
    GLushort* GetTorusIndices();

    // Getters for vertex and index sizes
    GLsizei GetTorusVerticesSize() const;
    GLsizei GetTorusIndicesSize() const;

private:
    // Private member functions for generating vertices and indices
    void GenerateTorusVertices();
    void GenerateTorusIndices();

    // Private member variables
    std::vector<GLfloat> torusVertex;
    std::vector<GLushort> torusIndex;

    GLsizei torusVerticesSize;
    GLsizei torusIndicesSize;
    

    float torusDistFromCenter;
    float torusRadius;
    int torusSides;

};

#endif
