// used the module 4 tutorial as a base and made changes as necessary

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
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Brian Engel 3D Bottle"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh
        size_t vertsSize;
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // vector of all meshs
    vector<GLMesh> meshs;
    // Shader program
    GLuint gProgramId;
    // Texture
    GLuint gTextureId0;
    GLuint gTextureId1;
    GLuint gTextureId2;
    GLuint gTextureId3;
    GLuint gTextureId4;
    GLuint gTextureId5;
    GLuint gTextureId6;
    glm::vec2 gUVScale(2.0f, 10.0f);
    glm::vec2 gUVScale2(10.0f, 10.0f);
    GLint gTexWrapMode = GL_REPEAT;

    // variables for prism
    const int prismRad = 1;
    const int prismHeight = 1;
    const int prismSides = 30;
    const double torusDistFromCenter = 2.5;
    const double torusRadius = 0.5;
    const int torusSides = 30;
    const double sphereRadius = 1;
    const int sphereSides = 30;
    GLfloat prismTopVertex[(prismSides + 1) * 2 * 12];
    GLfloat prismSideVertex[(prismSides + 1) * 2 * 12];
    GLushort prismSideIndex[prismSides * 6];
    GLushort prismTopIndex[prismSides * 6];

    // variables for cube
    GLfloat cubeSideVertex[4 * 4 * 12];
    GLfloat cubeMainVertex[4 * 2 * 12];
    GLushort cubeSideIndex[6 * 4];
    GLushort cubeMainIndex[6 * 2];

    // variables for plane
    GLfloat baseVertex[4*12];
    GLushort baseIndex[6];

    // variables for torus
    GLfloat torusVertex[(torusSides + 1) * (torusSides + 1) * 12];
    GLushort torusIndex[6*(torusSides)* (torusSides)];

    // variables for sphere
    GLfloat sphereVertex[12 * (sphereSides + 1) * (sphereSides + 1)];
    GLushort sphereIndex[6 * sphereSides/2 * sphereSides];

    // camera positions
    glm::vec3 gCameraPos = glm::vec3(0.0f, 3.0f, 15.0f);
    glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f;  // Initial yaw angle (facing to the left)
    float pitch = 0.0f;  // Initial pitch angle (level with the horizon)
    float mouseSensitivity = 0.1f;  // Adjust as needed

    // time
    float gDeltaTime = 0.0f; // Time between current frame and last frame
    float gLastFrame = 0.0f;

    // camera speed
    float cameraSpeed = 2.5f;

    // Initial projection type
    bool perspectiveProjection = true;

    // light color
    glm::vec3 gLightColor(1.0f, 0.839f, 0.667f); // sunlight
    glm::vec3 gLightColor2(1.0f, 1.0f, 1.0f); // led light bulb

    // Light position
    glm::vec3 gLightPosition(0.0f, 5.0f, 40.0f);
    glm::vec3 gLightPosition2(2.0f, 16.0f, 4.0f);

    // diffuse lamp intensity
    float diffuseIntensityValue = 0.5;
    float diffuseIntensityValue2 = 0.5;
    float specularIntensity = 0.8f;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh& mesh, const GLfloat *vertArray, const GLushort *indexArray);
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void GeneratePrismTopVertices();
void GenerateBaseVertices();
void GenerateBaseIndices();
void GenerateCubeSideVertices();
void GenerateCubeMainVertices();
void GenerateCubeSideIndices();
void GenerateCubeMainIndices();
void GeneratePrismSideVertices();
void GeneratePrismSideIndices();
void GeneratePrismTopIndices();
void GenerateTorusVertices();
void GenerateTorusIndices();
void GenerateSphereVertices();
void GenerateSphereIndices();

void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}

/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(330,
    layout(location = 0) in vec3 position;     // Position attribute
    layout(location = 1) in vec4 color;        // Color attribute
    layout(location = 2) in vec2 texCoord;     // Texture coordinate attribute
    layout(location = 3) in vec3 normal;       // normal attribute

    out vec4 vertexColor;                          // Output color to fragment shader
    out vec2 fragTexCoord;                       // Output texture coordinate to fragment shader
    out vec3 vertexNormal; // For outgoing normals to fragment shader
    out vec3 vertexFragmentPos; // For outgoing color or pixels to fragment shader

    uniform mat4 model;                          // Model matrix
    uniform mat4 view;                           // View matrix
    uniform mat4 projection;                     // Projection matrix

    void main()
    {
        mat4 mvp = projection * view * model;    // Model-View-Projection matrix
        gl_Position = mvp * vec4(position, 1.0);

        vertexColor = color;                      // Pass color to fragment shader
        fragTexCoord = texCoord;                // Pass texture coordinate to fragment shader

        vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment or pixel position in world space only (excludes view and projection)

        vertexNormal = mat3(transpose(inverse(model))) * normal; // Gets normal vectors in world space only and excludes normal translation properties

    }
);

/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(330,
    in vec4 vertexColor;                           // Input color from vertex shader
    in vec2 fragTexCoord;                        // Input texture coordinate from vertex shader
    in vec3 vertexNormal; // For incoming normals
    in vec3 vertexFragmentPos; // For incoming fragment position

    out vec4 finalColor;                         // Final output color

    uniform sampler2D textureSampler;            // Texture sampler
    uniform sampler2D textureSampler2;
    uniform sampler2D textureSampler3;
    uniform sampler2D textureSampler4;
    uniform sampler2D textureSampler5;
    uniform sampler2D textureSampler6;
    uniform sampler2D textureSampler7;
    uniform vec2 uvScale;
    uniform vec2 uvScale2;

    uniform vec4 overrideColor;         // Override color

    uniform vec3 lightColor;
    uniform vec3 lightColor2;
    uniform vec3 lightPos;
    uniform vec3 lightPos2;
    uniform vec3 viewPosition;
    uniform float diffuseIntensity;
    uniform float diffuseIntensity2;
    uniform float specularIntensity;

    void main()
    {
        //Calculate Ambient lighting*/
        float ambientStrength = 0.5f; // Set ambient or global lighting strength.
        vec3 ambient = ambientStrength * lightColor; // Generate ambient light color.

        //Calculate Diffuse lighting*/
        vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit.
        vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube.
        float impact = max(dot(norm, lightDirection), 0.0) * diffuseIntensity;// Calculate diffuse impact by generating dot product of normal and light.
        vec3 diffuse = impact * lightColor; // Generate diffuse light color.

        vec3 norm2 = normalize(vertexNormal); // Normalize vectors to 1 unit.
        vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube.
        float impact2 = max(dot(norm, lightDirection2), 0.0) * diffuseIntensity2;// Calculate diffuse impact by generating dot product of normal and light.
        vec3 diffuse2 = impact2 * lightColor2; // Generate diffuse light color.

        //Calculate Specular lighting*/
        //float specularIntensity = 0.8f; // Set specular light strength.
        float highlightSize = 16.0f; // Set specular highlight size.
        vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction.

        // Calculate Specular lighting for light 1
        vec3 reflectDir = reflect(-lightDirection, norm);
        float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
        vec3 specular = specularIntensity * specularComponent * lightColor;

        // Calculate Specular lighting for light 2
        vec3 reflectDir2 = reflect(-lightDirection2, norm2);
        float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize);
        vec3 specular2 = specularIntensity * specularComponent2 * lightColor2;

        // Sum up the specular components
        vec3 specularSum = specular + specular2;

        // Calculate Phong result.
        vec3 phong = (ambient + diffuse + diffuse2 + specularSum);

        // Use override color if provided, otherwise use base color
        finalColor.rgb = (overrideColor.a > 0.0) ? overrideColor.rgb * phong : vertexColor.rgb * phong;
        //finalColor.a = (overrideColor.a > 0.0) ? overrideColor.a : 1.0;

        // Sample texture using texture coordinates if textureSampler is set
        vec4 texColor = texture(textureSampler, fragTexCoord * uvScale2);
        vec4 texColor2 = texture(textureSampler2, fragTexCoord);
        vec4 texColor3 = texture(textureSampler3, fragTexCoord);
        vec4 texColor4 = texture(textureSampler4, fragTexCoord);
        vec4 texColor5 = texture(textureSampler5, fragTexCoord);
        vec4 texColor6 = texture(textureSampler6, fragTexCoord);
        vec4 texColor7 = texture(textureSampler7, fragTexCoord * uvScale);

        // If texture is available, use it; otherwise, use the vertex color
        finalColor.rgb = (texColor.rgb == vec3(0.0)) ? finalColor.rgb : texColor.rgb * phong;
        finalColor.rgb = (texColor2.rgb == vec3(0.0)) ? finalColor.rgb : texColor2.rgb * phong;
        finalColor.rgb = (texColor3.rgb == vec3(0.0)) ? finalColor.rgb : texColor3.rgb * phong;
        finalColor.rgb = (texColor4.rgb == vec3(0.0)) ? finalColor.rgb : texColor4.rgb * phong;
        finalColor.rgb = (texColor5.rgb == vec3(0.0)) ? finalColor.rgb : texColor5.rgb * phong;
        finalColor.rgb = (texColor6.rgb == vec3(0.0)) ? finalColor.rgb : texColor6.rgb * phong;
        finalColor.rgb = (texColor7.rgb == vec3(0.0)) ? finalColor.rgb : texColor7.rgb * phong;

        // Set the alpha value
        //finalColor.a = 1.0; // Or use a different alpha value if needed
        finalColor.a = (overrideColor.a > 0.0) ? overrideColor.a : 1.0;
    }
);

int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    GeneratePrismSideVertices();
    GeneratePrismSideIndices();
    GeneratePrismTopVertices();
    GeneratePrismTopIndices();
    
    GenerateBaseVertices();
    GenerateBaseIndices();

    GenerateCubeSideVertices();
    GenerateCubeSideIndices();
    GenerateCubeMainVertices();
    GenerateCubeMainIndices();

    GenerateTorusVertices();
    GenerateTorusIndices();

    GenerateSphereVertices();
    GenerateSphereIndices();

    GLMesh cylinderTopMesh;
    cylinderTopMesh.vertsSize = sizeof(prismTopVertex);
    cylinderTopMesh.nIndices = sizeof(prismTopIndex) / sizeof(prismTopIndex[0]);
    UCreateMesh(cylinderTopMesh, prismTopVertex, prismTopIndex); // Calls the function to create the Vertex Buffer Object
    meshs.push_back(cylinderTopMesh);

    GLMesh cylinderSideMesh;
    cylinderSideMesh.vertsSize = sizeof(prismSideVertex);
    cylinderSideMesh.nIndices = sizeof(prismSideIndex) / sizeof(prismSideIndex[0]);
    UCreateMesh(cylinderSideMesh, prismSideVertex, prismSideIndex); // Calls the function to create the Vertex Buffer Object
    meshs.push_back(cylinderSideMesh);

    GLMesh baseMesh;
    baseMesh.vertsSize = sizeof(baseVertex);
    baseMesh.nIndices = sizeof(baseIndex) / sizeof(baseIndex[0]);
    UCreateMesh(baseMesh, baseVertex, baseIndex);
    meshs.push_back(baseMesh);

    GLMesh cubeSideMesh;
    cubeSideMesh.vertsSize = sizeof(cubeSideVertex);
    cubeSideMesh.nIndices = sizeof(cubeSideIndex) / sizeof(cubeSideIndex[0]);
    UCreateMesh(cubeSideMesh, cubeSideVertex, cubeSideIndex);
    meshs.push_back(cubeSideMesh);

    GLMesh cubeMainMesh;
    cubeMainMesh.vertsSize = sizeof(cubeMainVertex);
    cubeMainMesh.nIndices = sizeof(cubeMainIndex) / sizeof(cubeMainIndex[0]);
    UCreateMesh(cubeMainMesh, cubeMainVertex, cubeMainIndex);
    meshs.push_back(cubeMainMesh);

    GLMesh torusMesh;
    torusMesh.vertsSize = sizeof(torusVertex);
    torusMesh.nIndices = sizeof(torusIndex) / sizeof(torusIndex[0]);
    UCreateMesh(torusMesh, torusVertex, torusIndex);
    meshs.push_back(torusMesh);

    GLMesh sphereMesh;
    sphereMesh.vertsSize = sizeof(sphereVertex);
    sphereMesh.nIndices = sizeof(sphereIndex) / sizeof(sphereIndex[0]);
    UCreateMesh(sphereMesh, sphereVertex, sphereIndex);
    meshs.push_back(sphereMesh);

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Load textures
    const char* texFilename = "stb/gray_fabric3.jpg";
    if (!UCreateTexture(texFilename, gTextureId0))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename2 = "stb/combinedBottle.jpg";
    if (!UCreateTexture(texFilename2, gTextureId1))
    {
        cout << "Failed to load texture " << texFilename2 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename3 = "stb/offwhite.jpg";
    if (!UCreateTexture(texFilename3, gTextureId2))
    {
        cout << "Failed to load texture " << texFilename3 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename4 = "stb/green_felt.jpg";
    if (!UCreateTexture(texFilename4, gTextureId3))
    {
        cout << "Failed to load texture " << texFilename4 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename5 = "stb/dog_toy.jpg";
    if (!UCreateTexture(texFilename5, gTextureId4))
    {
        cout << "Failed to load texture " << texFilename5 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename6 = "stb/tennis_ball2.jpg";
    if (!UCreateTexture(texFilename6, gTextureId5))
    {
        cout << "Failed to load texture " << texFilename6 << endl;
        return EXIT_FAILURE;
    }

    const char* texFilename7 = "stb/toy_pattern.jpg";
    if (!UCreateTexture(texFilename7, gTextureId6))
    {
        cout << "Failed to load texture " << texFilename7 << endl;
        return EXIT_FAILURE;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler"), 0);

    // We set the texture as texture unit 1
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler2"), 1);

    // We set the texture as texture unit 2
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler3"), 2);

    // We set the texture as texture unit 3
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler4"), 3);

    // We set the texture as texture unit 4
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler5"), 4);

    // We set the texture as texture unit 5
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler6"), 5);

    // We set the texture as texture unit 6
    glUniform1i(glGetUniformLocation(gProgramId, "textureSampler7"), 6);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // set time variables
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }
    for (GLMesh mesh : meshs) {
        // Release mesh data
        UDestroyMesh(mesh);
    }

    // Release texture
    UDestroyTexture(gTextureId0);
    UDestroyTexture(gTextureId1);
    UDestroyTexture(gTextureId2);
    UDestroyTexture(gTextureId3);
    UDestroyTexture(gTextureId4);
    UDestroyTexture(gTextureId5);
    UDestroyTexture(gTextureId6);

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

    
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

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
    //static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool keypress = false;

    float cameraOffset = cameraSpeed * gDeltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        gCameraPos += cameraOffset * gCameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        gCameraPos -= cameraOffset * gCameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        gCameraPos -= glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraOffset;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        gCameraPos += glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraOffset;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        gCameraPos += gCameraUp * cameraOffset; // Move up
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        gCameraPos -= gCameraUp * cameraOffset; // Move down
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        perspectiveProjection = !perspectiveProjection;
    }
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

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");
    GLint overrideColorLoc = glGetUniformLocation(gProgramId, "overrideColor");
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    GLint UVScaleLoc2 = glGetUniformLocation(gProgramId, "uvScale2");

    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightColorLoc2 = glGetUniformLocation(gProgramId, "lightColor2");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint lightPositionLoc2 = glGetUniformLocation(gProgramId, "lightPos2");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
    GLint diffuseIntensityLocation = glGetUniformLocation(gProgramId, "diffuseIntensity");
    GLint diffuseIntensityLocation2 = glGetUniformLocation(gProgramId, "diffuseIntensity2");
    GLint specularIntensityLocation = glGetUniformLocation(gProgramId, "specularIntensity");

    // Pass color, light, and camera data to the Cube Shader program's corresponding uniforms
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
    const glm::vec3 cameraPosition = gCameraPos;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
    glUniform1f(diffuseIntensityLocation, diffuseIntensityValue);
    glUniform1f(diffuseIntensityLocation2, diffuseIntensityValue2);

    // I scaled, then translated, then rotated each of the cylinders seperately
    glm::mat4 model = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
    
    // updated to use the wasd keys to move
    glm::mat4 view = glm::lookAt(gCameraPos, gCameraPos + gCameraFront, gCameraUp);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // Creates a perspective projection
    glm::mat4 projection;
    if (perspectiveProjection) {
        projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 100.0f);
    }
   
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // draw body of shampoo bottle with texture
    specularIntensity = 0.8f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextureId1);
    // glUniform4f(glGetUniformLocation(gProgramId, "overrideColor"), 1.0f, 1.0f, 1.0f, 0.1f);
    model = glm::rotate(-1.57f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(3.14f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(1.0f, 1.0f, 6.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(1).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(1).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw top and bottom of body of bottle
    specularIntensity = 0.8f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gTextureId2);
    //glUniform4f(glGetUniformLocation(gProgramId, "overrideColor"), 1.0f, 0.0f, 0.0f, 1.0f);
    model = glm::rotate(-1.57f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(1.0f, 1.0f, 6.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(0).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(0).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw top (cap) of shampoo bottle
    specularIntensity = 0.8f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gTextureId2);
    //glUniform4f(glGetUniformLocation(gProgramId, "overrideColor"), 1.0f, 0.0f, 0.0f, 1.0f);
    model = glm::rotate(-1.57f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, 6.0f)) * glm::scale(glm::vec3(0.33f, 0.33f, 0.8f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(0).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(0).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw sides (cap) of shampoo bottle
    specularIntensity = 0.8f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gTextureId2);
    //glUniform4f(glGetUniformLocation(gProgramId, "overrideColor"), 0.0f, 1.0f, 0.0f, 1.0f);
    model = glm::rotate(-1.57f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, 6.0f)) * glm::scale(glm::vec3(0.33f, 0.33f, 0.8f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(1).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(1).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw base
    specularIntensity = 0.4f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glUniform2fv(UVScaleLoc2, 1, glm::value_ptr(gUVScale2));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId0);
    model = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(2).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(2).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // draw sides of dog toy
    specularIntensity = 0.2f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gTextureId3);
    model = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(-0.5f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(-0.4f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(4.5f, 3.0f, 0.2f)) * glm::scale(glm::vec3(5.33f, 1.17f, 2.83f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(3).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(3).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw main faces of dog toy
    specularIntensity = 0.2f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, gTextureId4);
    model = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(-0.5f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(-0.4f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(4.5f, 3.0f, 0.2f)) * glm::scale(glm::vec3(5.33f, 1.17f, 2.83f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(4).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(4).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw torus toy
    specularIntensity = 0.2f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, gTextureId6);
    model = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(5.0f, 0.0f, 3.0f)) * glm::scale(glm::vec3(1.0f, 1.833f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(5).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(5).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw tennis ball by itself
    specularIntensity = 0.0f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, gTextureId5);
    model = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(-2.5f, 0.0f, 1.5f)) * glm::scale(glm::vec3(1.1f, 1.1f, 1.1f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(6).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(6).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw tennis ball 1 for can
    specularIntensity = 0.0f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, gTextureId5);
    model = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(-0.5f, 0.0f, 3.5f)) * glm::scale(glm::vec3(1.1f, 1.1f, 1.1f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(6).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(6).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw tennis ball 2 for can
    specularIntensity = 0.0f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, gTextureId5);
    //glUniform4f(glGetUniformLocation(gProgramId, "overrideColor"), 0.0f, 1.0f, 0.0f, 0.2f);
    model = glm::rotate(0.0f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(-0.5f, 2.2f, 3.5f)) * glm::scale(glm::vec3(1.1f, 1.1f, 1.1f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(6).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(6).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw red top of tennis ball can
    specularIntensity = 0.8f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glUniform4f(glGetUniformLocation(gProgramId, "overrideColor"), 1.0f, 0.0f, 0.0f, 1.0f);
    model = glm::rotate(-1.57f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(-0.5f, -3.5f, 7.17f)) * glm::scale(glm::vec3(1.27f, 1.2f, 0.2f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(1).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(1).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw red top of tennis ball can
    specularIntensity = 0.8f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glUniform4f(glGetUniformLocation(gProgramId, "overrideColor"), 1.0f, 0.0f, 0.0f, 1.0f);
    model = glm::rotate(-1.57f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(-0.5f, -3.5f, 7.17f)) * glm::scale(glm::vec3(1.27f, 1.2f, 0.2f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(0).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(0).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw transparent body of tennis ball can
    specularIntensity = 0.8f;
    glUniform1f(specularIntensityLocation, specularIntensity);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform4f(glGetUniformLocation(gProgramId, "overrideColor"), 1.0f, 1.0f, 1.0f, 0.3f);
    model = glm::rotate(-1.57f, glm::vec3(1.0, 0.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f)) * glm::rotate(0.0f, glm::vec3(0.0, 0.0f, 1.0f)) * glm::translate(glm::vec3(-0.5f, -3.5f, 0.0f)) * glm::scale(glm::vec3(1.27f, 1.2f, 7.17f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(meshs.at(1).vao);
    glDrawElements(GL_TRIANGLES, meshs.at(1).nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

void UCreateMesh(GLMesh& mesh, const GLfloat *vertArray, const GLushort *indexArray) {
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerCoord = 2;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertsSize, vertArray, GL_STATIC_DRAW);


    glGenBuffers(1, &mesh.vbos[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mesh.nIndices, indexArray, GL_STATIC_DRAW);


    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerCoord + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(GLfloat) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerCoord, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(GLfloat) * (floatsPerVertex+floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(GLfloat) * (floatsPerVertex + floatsPerColor + floatsPerCoord)));
    glEnableVertexAttribArray(3);
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}

bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Set the texture wrapping parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture.

        return true;
    }

    // Error loading the image
    return false;
}

void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
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
void GeneratePrismTopVertices() {
    
    vector<GLfloat> prismData;
    const int numColors = 6;
    prismData.insert(prismData.end(), { 0.0f, 0.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f, 0.5f, 0.5, 0.0f, 0.0f, -1.0f});
    prismData.insert(prismData.end(), { 0.0f, 0.0f, prismHeight, 0.0f,0.0f,0.0f,1.0f,0.5f,0.5f, 0.0f, 0.0f, 1.0f});
    for (int i = 0; i < prismSides; ++i) {
        GLfloat theta = 2.0f * M_PI * static_cast<float>(i) / prismSides;
        GLfloat x = prismRad * cos(theta);
        GLfloat y = prismRad * sin(theta);
        prismData.push_back((x));
        prismData.push_back((y));
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
        GLfloat textureX = cos(theta);
        GLfloat textureY = sin(theta);
        prismData.insert(prismData.end(), { (textureX + 1)/2, (textureY + 1)/2, 0.0f,0.0f, -1.0f });


        prismData.push_back(x);
        prismData.push_back(y);
        prismData.push_back(static_cast<GLfloat>(prismHeight));
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
        textureX = cos(theta);
        textureY = sin(theta);
        prismData.insert(prismData.end(), { (textureX + 1)/2, (textureY + 1)/2, 0.0f,0.0f, 1.0f });
    }

    for (int i = 0; i < prismData.size(); i++) {
        prismTopVertex[i] = prismData[i];
    }
}

void GeneratePrismSideVertices() {

    vector<GLfloat> prismData;
    const int numColors = 6;
    for (int i = 0; i < prismSides + 1; ++i) {
        GLfloat theta = 2.0f * M_PI * static_cast<float>(i) / prismSides;
        GLfloat x = prismRad * cos(theta);
        GLfloat y = prismRad * sin(theta);
        prismData.push_back((x));
        prismData.push_back((y));
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

        prismData.insert(prismData.end(), { 1.0f, static_cast<float>(i) / prismSides });
        // adding normals
        GLfloat normalX = cos(theta);
        GLfloat normalY = sin(theta);
        GLfloat normalZ = 0.0f;
        prismData.insert(prismData.end(), { normalX, normalY, normalZ });
        prismData.push_back(x);
        prismData.push_back(y);
        prismData.push_back(static_cast<GLfloat>(prismHeight));
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

        prismData.insert(prismData.end(), { 0.0f, static_cast<float>(i) / prismSides });
        // adding normals
        normalZ = 0.0f;
        prismData.insert(prismData.end(), { normalX, normalY, normalZ });
    }

    for (int i = 0; i < prismData.size(); i++) {
        prismSideVertex[i] = prismData[i];
    }
}

void GeneratePrismSideIndices() {
    std::vector<GLushort> prismData;
    // create top and bottom
    int vert = (prismSides + 1) * 2;
    
    // create sides
    for (int i = 0; i < vert; i++) {
        prismData.insert(prismData.end(), { static_cast<GLushort>(i), static_cast<GLushort>(i + 1), static_cast<GLushort>(i + 2) });
    }

    for (int i = 0; i < prismData.size(); i++) {
        prismSideIndex[i] = prismData[i];
    }
}

// I created this function to get indices for the prism
void GeneratePrismTopIndices() {
    std::vector<GLushort> prismData;
    // create top and bottom
    int vert = (prismSides + 1) * 2;
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
    for (int i = 0; i < prismData.size(); i++) {
        prismTopIndex[i] = prismData[i];
    }
}

void GenerateBaseVertices() {
    GLfloat values[] = {
        // coordinates          // color                    // tex coord    // normals
        10.0f, 0.0f,  10.0f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
       -10.0f, 0.0f,  10.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
        10.0f, 0.0f, -10.0f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
       -10.0f, 0.0f, -10.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f
    };
    for (int i = 0; i < 4 * 12; i++) {
        baseVertex[i] = values[i];
    }
}

void GenerateBaseIndices() {
    GLushort values[] = {
        0, 1, 2,
        1, 2, 3
    };
    for (int i = 0; i < 6; i++) {
        baseIndex[i] = values[i];
    }
}

void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    // variables for camera position
    static double lastX = xpos;
    static double lastY = ypos;
    static bool firstMouse = true;

    // sets up initial position
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // difference between current and last position
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    // set current position to last position
    lastX = xpos;
    lastY = ypos;

    // scale the offset
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    // add the offset to the current yaw and pitch
    yaw += xoffset;
    pitch += yoffset;

    // so camera doesn't flip over
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // convert the new pitch and yaw to a new matrix
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    gCameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// updated to control the camera speed. has minimum of 0.0f and maximum of 10.0f
// updated to control mouse sensitivity as well. has minimum of 0.0f and maximun of 1.0f
// when the values are both 0.0f the object is locked in place
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset < 0) {
        cameraSpeed -= 0.5f;
    }
    else if (yoffset > 0) {
        cameraSpeed += 0.5f;
    }
    if (cameraSpeed < 1.0f) {
        cameraSpeed = 1.0f;
    }
    if (cameraSpeed > 10.0f) {
        cameraSpeed = 10.0f;
    }


    if (yoffset < 0) {
        mouseSensitivity -= 0.05f;
    }
    else if (yoffset > 0) {
        mouseSensitivity += 0.05f;
    }
    if (mouseSensitivity < 0.1f) {
        mouseSensitivity = 0.1f;
    }
    if (mouseSensitivity > 1.0f) {
        mouseSensitivity = 1.0f;
    }
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS) {
            //cout << "Left mouse button pressed" << endl;
        }
        else {
            //cout << "Left mouse button released" << endl;
        }
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS) {
            //cout << "Middle mouse button pressed" << endl;
        }
        else {
            //cout << "Middle mouse button released" << endl;
        }
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS) {
            //cout << "Right mouse button pressed" << endl;
        }
        else {
            //cout << "Right mouse button released" << endl;
        }
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

void GenerateCubeSideVertices() {
    GLfloat values[] = {
        // coordinates          // color                    // tex coord    // normals
        0.5f,  1.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
        0.5f,  1.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
        0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f, 0.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     1.0f, 0.0f, 0.0f,

       -0.5f,  1.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
       -0.5f,  1.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
       -0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
       -0.5f, 0.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,

        0.5f,  1.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, 0.0f, 1.0f,
       -0.5f,  1.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
        0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
       -0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, 0.0f, 1.0f,

        0.5f,  1.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
       -0.5f,  1.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, 0.0f, -1.0f,
        0.5f, 0.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, 0.0f, -1.0f,
       -0.5f, 0.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, -1.0f
    };
    for (int i = 0; i < 4 * 4 * 12; i++) {
        cubeSideVertex[i] = values[i];
    }
}

void GenerateCubeMainVertices() {
    GLfloat values[] = {
        // coordinates          // color                    // tex coord    // normals
        0.5f,  1.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
       -0.5f,  1.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
        0.5f,  1.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
       -0.5f,  1.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f,

        0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
       -0.5f, 0.0f,  0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     0.0f, -1.0f, 0.0f,
        0.5f, 0.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
       -0.5f, 0.0f,  -0.5f,      1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     0.0f, -1.0f, 0.0f
    };

    for (int i = 0; i < 4 * 2 * 12; i++) {
        cubeMainVertex[i] = values[i];
    }
}

void GenerateCubeSideIndices() {
    GLushort values[] = {
        0, 1, 2,
        1, 2, 3,

        4, 5, 6,
        5, 6, 7,
        
        8, 9, 10, 
        9, 10, 11,

        12, 13, 14,
        13, 14, 15
    };
    for (int i = 0; i < 6 * 4; i++) {
        cubeSideIndex[i] = values[i];
    }
}

void GenerateCubeMainIndices() {
    GLushort values[] = {
        0, 1, 2,
        1, 2, 3, 

        4, 5, 6,
        5, 6, 7
    };
    for (int i = 0; i < 6 * 2; i++) {
        cubeMainIndex[i] = values[i];
    }
}

void GenerateTorusVertices() {
    vector<GLfloat> torusData;
    for (int i = 0; i < torusSides + 1; ++i) {
        GLfloat theta = 2.0f * M_PI * static_cast<float>(i) / torusSides;
        for (int j = 0; j < torusSides + 1; ++j) {
            GLfloat phi = 2.0f * M_PI * static_cast<float>(j) / torusSides;

            GLfloat x = (torusDistFromCenter + torusRadius + torusRadius * cos(phi)) * cos(theta);
            GLfloat y = torusRadius * sin(phi) + torusRadius;
            GLfloat z = (torusDistFromCenter + torusRadius + torusRadius * cos(phi)) * sin(theta);

            // cout << "(" << x << ", " << y << ", " << z << ")\n";

            torusData.push_back(x);
            torusData.push_back(y);
            torusData.push_back(z);

            // insert color (white)
            torusData.insert(torusData.end(), { 1.0f, 1.0f, 1.0f, 1.0f });

            // insert texture coord
            torusData.insert(torusData.end(), { static_cast<float>(j) / torusSides, static_cast<float>(i) / torusSides });

            // adding normals

            GLfloat normalX = cos(theta) * cos(phi);
            GLfloat normalY = sin(theta);
            GLfloat normalZ = sin(theta)* cos(phi);

            torusData.insert(torusData.end(), { normalX, normalY, normalZ });

        }
    }

    for (int i = 0; i < torusData.size(); i++) {
        torusVertex[i] = torusData[i];
    }
}

void GenerateTorusIndices() {
    std::vector<GLushort> torusData;

    for (int i = 0; i < torusSides; ++i) {
        for (int j = 0; j < torusSides; ++j) {
            // Calculate indices for the quad
            GLuint index0 = i * (torusSides + 1) + j;
            GLuint index1 = index0 + 1;
            GLuint index2 = (i + 1) * (torusSides + 1) + j;
            GLuint index3 = index2 + 1;

            // Form two triangles from the quad
            torusData.push_back(index0);
            torusData.push_back(index1);
            torusData.push_back(index2);

            //cout << index0 << " " << index1 << " " << index2 << endl;

            torusData.push_back(index1);
            torusData.push_back(index3);
            torusData.push_back(index2);
            
            //cout << index1 << " " << index3 << " " << index2 << endl;
        }
    }

    for (int i = 0; i < torusData.size(); i++) {
        torusIndex[i] = torusData[i];
    }
}

void GenerateSphereVertices() {
    vector<GLfloat> sphereData;
    for (int i = 0; i < sphereSides + 1; ++i) {
        GLfloat theta = 2.0f * M_PI * static_cast<float>(i) / (sphereSides/2);
        for (int j = 0; j < sphereSides + 1; ++j) {
            GLfloat phi =2.0f * M_PI * static_cast<float>(j) / sphereSides;

            GLfloat x = sphereRadius * cos(theta) * cos(phi);
            GLfloat y = sphereRadius * sin(theta) + sphereRadius;
            GLfloat z = sphereRadius * cos(theta) * sin(phi);

            // cout << "(" << x << ", " << y << ", " << z << ")\n";

            sphereData.push_back(x);
            sphereData.push_back(y);
            sphereData.push_back(z);

            // insert color (white)
            sphereData.insert(sphereData.end(), { 1.0f, 1.0f, 1.0f, 1.0f });

            // insert texture coord
            sphereData.insert(sphereData.end(), { static_cast<float>(j) / sphereSides, static_cast<float>(i) / sphereSides });

            // adding normals

            GLfloat normalX = cos(theta) * cos(phi);
            GLfloat normalY = sin(theta);
            GLfloat normalZ = cos(theta) * sin(phi);

            sphereData.insert(sphereData.end(), { normalX, normalY, normalZ });
        }
    }

    for (int i = 0; i < sphereData.size(); i++) {
        sphereVertex[i] = sphereData[i];
    }
}

void GenerateSphereIndices() {
    vector<GLfloat> sphereData;

    for (int i = 0; i < sphereSides; ++i) {
        for (int j = 0; j < sphereSides; ++j) {
            int current = i * sphereSides + j;
            int next = current + sphereSides + 1;

            // Triangle 1
            sphereData.push_back(current);
            sphereData.push_back(next);
            sphereData.push_back(next + 1);


            // Triangle 2
            sphereData.push_back(current);
            sphereData.push_back(next + 1);
            sphereData.push_back(current + 1);
        }
    }

    for (int i = 0; i < sphereData.size(); i++) {
        sphereIndex[i] = sphereData[i];
    }
}