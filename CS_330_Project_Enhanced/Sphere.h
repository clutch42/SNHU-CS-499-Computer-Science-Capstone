#ifndef SPHERE_H
#define SPHERE_H

#include <GL/glew.h> 
#include <vector>

// Created a class to hold all important variables
// and functions. To instantiate the class
// radius must be entered as a integer and a number of sides must be entered
// as an integer. Public getters for the arrays of vertices, indices,
// and size of both.
class Sphere {
public:
    // Constructor and Destructor
    Sphere(int rad, int sides);
    ~Sphere();

    // Getters for vertex and index arrays
    GLfloat* GetSphereVertices();
    GLushort* GetSphereIndices();

    // Getters for vertex and index sizes
    GLsizei GetSphereVerticesSize() const;
    GLsizei GetSphereIndicesSize() const;

private:
    // Private member functions for generating vertices and indices
    void GenerateSphereVertices();
    void GenerateSphereIndices();

    // Private member variables
    std::vector<GLfloat> sphereVertex;
    std::vector<GLushort> sphereIndex;

    GLsizei sphereVerticesSize;
    GLsizei sphereIndicesSize;

    int sphereRadius;
    int sphereSides;

};

#endif

