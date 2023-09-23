// used the module 3.4 tutorial as a base and made changes as necessary

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#include <vector>
#include <cmath>
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Brian Engel 3D Triangle"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    //GLMesh gMesh;
    // vector of all meshs
    vector<GLMesh> meshs;
    // Shader program
    GLuint gProgramId;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void GeneratePrismVertices(GLfloat verts[], GLfloat rad, GLfloat height, int sides);
void GeneratePrismIndices(GLushort* indices, int sides);

/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1

out vec4 vertexColor; // variable to transfer color data to the fragment shader

//Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
    vertexColor = color; // references incoming color data
}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader

out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(vertexColor);
}
);


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    GLMesh tempMesh;

    // Create the mesh
    UCreateMesh(tempMesh); // Calls the function to create the Vertex Buffer Object
    meshs.push_back(tempMesh);


    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }
    for (auto mesh : meshs) {
        // Release mesh data
        UDestroyMesh(mesh);
    }
    // Release shader program
    UDestroyShaderProgram(gProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// Functioned called to render a frame
void URender()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // I scaled, then translated, then rotated each of the cylinders seperately
    glm::mat4 largerPrismModel = glm::rotate(1.8f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(1.0f, 1.0f, 6.0f));
    glm::mat4 smallerPrismModel = glm::rotate(1.8f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, -0.82f)) * glm::scale(glm::vec3(0.41f, 0.41f, 0.82f));
    // playing around creating an extra cylinder
    //glm::mat4 mediumPrismModel = glm::rotate(1.8f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(2.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(0.7f, 0.7f, 3.0f));
    // Transforms the camera: move the camera back (z axis) and up (y axis)
    glm::mat4 view = glm::translate(glm::vec3(0.0f, 1.0f, -12.0f));

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(largerPrismModel));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    for (GLMesh mesh : meshs) {
        // Activate the VBOs contained within the mesh's VAO
        glBindVertexArray(mesh.vao);

        // Draws the triangles
        glDrawElements(GL_TRIANGLES, mesh.nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    }
    // Render the smaller prism
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(smallerPrismModel));

    // Draws the smaller prism
    //glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, nullptr);

    // Render the medium prism
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mediumPrismModel));
    // playing around creating a third cylinder
    // Draws the medium prism
    //glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, nullptr);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    // Define vertices and indices for the prism
    const GLfloat prismHeight = 1.0f;
    const GLfloat prismRad = 1.0f;
    const int prismSides = 60;
    GLfloat prismVerts[(prismSides + 1) * 7 * 2];
    GLushort prismIndices[prismSides * 12];
    // created my own functions for the vertices and indices
    GeneratePrismVertices(prismVerts, prismRad, prismHeight, prismSides);
    GeneratePrismIndices(prismIndices, prismSides);

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    size_t prismVertsSize = (prismSides + 1) * 7 * 2 * sizeof(GLfloat);
    // Create a single buffer for the combined vertex data
    glGenBuffers(1, &mesh.vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, prismVertsSize, prismVerts, GL_STATIC_DRAW);

    // Create a single buffer for the combined index data
    size_t numIndices = sizeof(prismIndices) / sizeof(prismIndices[0]);
    mesh.nIndices = sizeof(prismIndices);
    glGenBuffers(1, &mesh.vbos[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLushort), prismIndices, GL_STATIC_DRAW);


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    // Strides between vertex coordinates is 7 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

// I created this function to automate the vertex positions for a prism with differing radius, height, and sides
// and assign it one of 6 colors unless it is the center top which is white, or center botttom which is black
void GeneratePrismVertices(GLfloat verts[], GLfloat rad, GLfloat height, int sides) {
    std::vector<GLfloat> prismData;
    const int numColors = 6;
    prismData.insert(prismData.end(), { 0.0f, 0.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f });
    prismData.insert(prismData.end(), { 0.0f, 0.0f, height, 0.0f,0.0f,0.0f,1.0f });
    for (int i = 0; i < sides; ++i) {
        float theta = 2.0f * M_PI * static_cast<float>(i) / sides;
        float x = rad * cos(theta);
        float y = rad * sin(theta);
        prismData.push_back(x);
        prismData.push_back(y);
        prismData.push_back(0.0f);

        // Assign colors with alpha to vertices based on the segment
        glm::vec4 color;
        int colorIndex = i % numColors;
        if (colorIndex == 0) {
            prismData.insert(prismData.end(), { 1.0f, 0.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 1) {
            prismData.insert(prismData.end(), { 0.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 2) {
            prismData.insert(prismData.end(), { 0.0f, 0.0f, 1.0f, 1.0f });
        }
        else if (colorIndex == 3) {
            prismData.insert(prismData.end(), { 1.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 4) {
            prismData.insert(prismData.end(), { 0.0f, 1.0f, 1.0f, 1.0f });
        }
        else {
            prismData.insert(prismData.end(), { 1.0f, 0.0f, 1.0f, 1.0f });
        }
        prismData.push_back(x);
        prismData.push_back(y);
        prismData.push_back(static_cast<GLfloat>(height));
        if (colorIndex == 0) {
            prismData.insert(prismData.end(), { 1.0f, 0.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 1) {
            prismData.insert(prismData.end(), { 0.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 2) {
            prismData.insert(prismData.end(), { 0.0f, 0.0f, 1.0f, 1.0f });
        }
        else if (colorIndex == 3) {
            prismData.insert(prismData.end(), { 1.0f, 1.0f, 0.0f, 1.0f });
        }
        else if (colorIndex == 4) {
            prismData.insert(prismData.end(), { 0.0f, 1.0f, 1.0f, 1.0f });
        }
        else {
            prismData.insert(prismData.end(), { 1.0f, 0.0f, 1.0f, 1.0f });
        }
    }
    for (size_t i = 0; i < prismData.size(); i++)
    {
        verts[i] = prismData[i];
    }
}

// I created this function to get indices for the prism
void GeneratePrismIndices(GLushort* indices, int sides) {
    std::vector<GLushort> prismData;
    // create top and bottom
    int vert = (sides + 1) * 2;
    for (int i = 2; i < vert; i++) {
        if (i == 2) {
            prismData.insert(prismData.end(), { 0, static_cast<GLushort>(i), static_cast<GLushort>(vert - 2) });
        }
        else if (i == 3) {
            prismData.insert(prismData.end(), { 1, static_cast<GLushort>(i), static_cast<GLushort>(vert - 1) });
        }
        else if (i % 2 == 0) {
            prismData.insert(prismData.end(), { 0, static_cast<GLushort>(i), static_cast<GLushort>(i - 2) });
        }
        else {
            prismData.insert(prismData.end(), { 1, static_cast<GLushort>(i), static_cast<GLushort>(i - 2) });
        }
    }
    // create sides
    for (int i = 2; i < vert; i++) {
        if (i == 2) {
            prismData.insert(prismData.end(), { 2, static_cast<GLushort>(vert - 2), static_cast<GLushort>(vert - 1) });
        }
        else if (i == 3) {
            prismData.insert(prismData.end(), { 2, 3, static_cast<GLushort>(vert - 1) });
        }
        else {
            prismData.insert(prismData.end(), { static_cast<GLushort>(i), static_cast<GLushort>(i - 1), static_cast<GLushort>(i - 2) });
        }
    }
    for (size_t i = 0; i < prismData.size(); i++) {
        indices[i] = prismData[i];
    }
}
