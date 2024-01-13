#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h" // Camera Class

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Final Project - 'Choose Your Class' - Quest McClure"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 1600;
    const int WINDOW_HEIGHT = 1200;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbo;     // Handles for the vertex buffer objects
        GLuint nVertices;    // Number of indices of the mesh
        GLuint nIndices;
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    // Hammer Mesh Data
    GLMesh gHammer;
    GLMesh gHammerHandle;  
    GLMesh gHammerHandleMetal;
    // Scythe Mesh Data
    GLMesh gScythe;
    GLMesh gScytheBlade;
    GLMesh gScytheSupport;
    // Sword Mesh Data
    GLMesh gSwordHandle;
    GLMesh gSwordPommel;
    GLMesh gSwordBlade;
    // Staff Mesh Data
    GLMesh gStaff;
    GLMesh gStaffOrb;
    GLMesh gStaffSupport;
    // Plane Mesh Data
    GLMesh gPlane;
    // Texture ids
    GLuint blankTexture;
    GLuint gTextureIdMetal;
    GLuint gTextureIdConcrete;
    GLuint gTextureIdLeather;
    GLuint gTextureIdHammerHandleMetal; 
    GLuint gTextureIdWood;
    GLuint gTextureIdBladeMetal;
    GLuint gTextureIdCrystal;
    GLuint gTextureIdMagicOrb;
    GLuint gTextureIdStaffWood;
    // Texture scale var
    glm::vec2 gUVScale(1.0f, 1.0f);
    // Shader program
    GLuint gProgramId;

    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 3.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;
    bool projMode = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    /* Lighting Variables */
    // Positions
    glm::vec3 gLightPosition(-3.5f, 0.5f, -0.5f);   // 1
    glm::vec3 gLightPosition2(-3.17f, 1.0f, -0.2f); // 2
    glm::vec3 gLightPosition3(0.0f, 1.0f, 4.0f);    // 3
    glm::vec3 gLightPosition4(3.17f, 1.0f, -0.2f);  // 4
    glm::vec3 gLightPosition5(0.0f, 1.5f, -1.0f);   // 5
   
    // Point Light Array
    glm::vec3 pointLightPositions[] = {
        gLightPosition,     // point light pos 1 Rotating blue
        gLightPosition2,    // point light pos 2 scythe purple
        gLightPosition3,    // point light pos 3 sword blue
        gLightPosition4,    // point light pos 4 staff orb red
        gLightPosition5     // point light pos 5 hammer gold
    };

    // LIGHT SWITCH
    bool lightSwitch = true;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UKeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void UCreateHammer(GLMesh& mesh);
void UCreateHammerHandle(GLMesh& mesh);
void UCreateHammerHandleMetal(GLMesh& mesh);

void UCreateCylinder(GLfloat verts[], int numSides, float radius, float halfLen, float texId);
void UCreateSphere(GLfloat verts[], float radius, float texId);

void UCreatePlane(GLMesh& mesh);

void UCreateScythe(GLMesh& mesh);
void UCreateScytheBlade(GLMesh& mesh);
void UCreateScytheSupport(GLMesh& mesh);

void UCreateSwordHandle(GLMesh& mesh);
void UCreateSwordPommel(GLMesh& mesh);
void UCreateSwordBlade(GLMesh& mesh);

void UCreateStaff(GLMesh& mesh);
void UCreateStaffOrb(GLMesh& mesh);
void UCreateStaffSupport(GLMesh& mesh);

void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();

void DrawHammer();
void DrawHammerHandle();
void DrawHammerHandleMetal();

void DrawScythe();
void DrawScytheBlade();
void DrawScytheSupport();

void DrawSwordHandle();
void DrawSwordPommel();
void DrawSwordBlade();

void DrawStaff();
void DrawStaffOrb();
void DrawStaffSupport();

void DrawPlane();

bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
    layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1
    layout(location = 2) in vec2 textureCoordinate;  // Texture data from Vertex Attrib Pointer 2
    layout(location = 3) in float textureIndex; // Texture array index data 
    layout(location = 4) in vec3 normal;

    out vec4 vertexColor; // variable to transfer color data to the fragment shader
    out vec2 vertexTextureCoordinate;
    flat out int v_TexIndex;
    out vec3 vertexFragmentPos;
    out vec3 vertexNormal;

    //Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
    vertexColor = color; // references incoming color data
    vertexTextureCoordinate = textureCoordinate; // reference tex coord
    v_TexIndex = int(textureIndex); // reference texture array location
    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)
    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties

}
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,

    // define material var
    struct Material {
         sampler2D diffuse;
        sampler2D specular;
        float shininess;
    };
    // define pointlight var
    struct PointLight {
        vec3 position;

        float constant;
        float linear;
        float quadratic;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };
    // define directional light var
    struct DirLight {
        vec3 direction;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    in vec2 vertexTextureCoordinate;
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader
    flat in int v_TexIndex; // holds incoming tex index
    in vec3 vertexFragmentPos; // For incoming fragment position
    in vec3 vertexNormal; // For incoming normals
    
    uniform vec3 viewPosition;
    uniform vec2 uvScale;
    uniform sampler2D u_Textures[10]; // uniform texture array to hold textures. To change texture amount, this needs to be changed.
    layout (location = 0) out vec4 fragmentColor;

    const int NR_POINT_LIGHTS = 5; // Define number of point lights
    uniform PointLight pointLights[NR_POINT_LIGHTS];
    uniform DirLight dirLight;
    uniform Material material;

    // Function prototypes
    vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
    vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);


    void main()
    {
        vec3 norm = normalize(vertexNormal); // normalize normals
        vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // normalize view dir
        // == =====================================================
        // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
        // For each phase, a calculate function is defined that calculates the corresponding color
        // per lamp. In the main() function we take all the calculated colors and sum them up for
        // this fragment's final color.
        // == =====================================================
        // phase 1: directional lighting
        vec3 result = CalcDirLight(dirLight, norm, viewDir);;

        // loop to assign point lights in array to vec3 result which gets sent out as fragmentColor
        for (int i = 0; i < NR_POINT_LIGHTS; i++)
            // phase 2: point lights
            result += CalcPointLight(pointLights[i], norm, vertexFragmentPos, viewDir);

        // Allows transparency to render invisible
        fragmentColor = vec4(result, 1.0);
        if (result.r == 0 && result.g == 0 && result.b == 0) {
            fragmentColor = vec4(0, 0, 0, 0);
        }
    }

    // calculates the color when using a directional light.
    vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
    {
        vec3 lightDir = normalize(-light.direction);
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        // combine results
        vec3 ambient = light.ambient * vec3(texture(u_Textures[v_TexIndex], vertexTextureCoordinate * uvScale));
        vec3 diffuse = light.diffuse * diff * vec3(texture(u_Textures[v_TexIndex], vertexTextureCoordinate * uvScale));
        vec3 specular = light.specular * spec * vec3(texture(u_Textures[v_TexIndex], vertexTextureCoordinate * uvScale));
        return (ambient + diffuse + specular);
    }

    // Calculate point light shader function
    vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
    {
        vec3 lightDir = normalize(light.position - fragPos);
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        // attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        // combine results
        vec3 ambient = light.ambient * vec3(texture(u_Textures[v_TexIndex], vertexTextureCoordinate * uvScale));
        vec3 diffuse = light.diffuse * diff * vec3(texture(u_Textures[v_TexIndex], vertexTextureCoordinate * uvScale));
        vec3 specular = light.specular * spec * vec3(texture(u_Textures[v_TexIndex], vertexTextureCoordinate * uvScale));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular);
    }
);

// Image flip function
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

int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the Hammer mesh
    UCreateHammer(gHammer); // Calls the function to create the Vertex Buffer Object
    UCreateHammerHandle(gHammerHandle);
    UCreateHammerHandleMetal(gHammerHandleMetal);
    // Create the Scythe mesh
    UCreateScythe(gScythe);
    UCreateScytheBlade(gScytheBlade);
    UCreateScytheSupport(gScytheSupport);
    // Create the Sword mesh
    UCreateSwordHandle(gSwordHandle);
    UCreateSwordPommel(gSwordPommel);
    UCreateSwordBlade(gSwordBlade);
    // Create the Staff mesh
    UCreateStaff(gStaff);
    UCreateStaffOrb(gStaffOrb);
    UCreateStaffSupport(gStaffSupport);
    // Create the Plane mesh
    UCreatePlane(gPlane);

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;
    
    glUseProgram(gProgramId);

    // Load textures. Each additional texture needs to scale from the previous one as well as a new GLuint texture id
    // TexRef 0 :: EMPTY (Set as completely see through texture)
    const char* texFilename = "../resources/textures/blank.png";
    if (!UCreateTexture(texFilename, blankTexture))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // TexRef 1 :: CONCRETE
    texFilename = "../resources/textures/concrete.jpg";
    if (!UCreateTexture(texFilename, gTextureIdConcrete))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // TexRef 2 :: METAL
    texFilename = "../resources/textures/metal.jpg";
    if (!UCreateTexture(texFilename, gTextureIdMetal))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // TexRef 3 :: LEATHER
    texFilename = "../resources/textures/leather.png";
    if (!UCreateTexture(texFilename, gTextureIdLeather))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // TexRef 4 :: HANDLE METAL
    texFilename = "../resources/textures/hammerhandlemetal.jpg";
    if (!UCreateTexture(texFilename, gTextureIdHammerHandleMetal))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // TexRef 5 :: WOOD
    texFilename = "../resources/textures/wood.png";
    if (!UCreateTexture(texFilename, gTextureIdWood))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // TexRef 6 :: BLADE METAL
    texFilename = "../resources/textures/blademetal.png";
    if (!UCreateTexture(texFilename, gTextureIdBladeMetal))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // TexRef 7 :: CRYSTAL
    texFilename = "../resources/textures/crystal.png";
    if (!UCreateTexture(texFilename, gTextureIdCrystal))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // TexRef 8 :: MAGIC ORB
    texFilename = "../resources/textures/magic.png";
    if (!UCreateTexture(texFilename, gTextureIdMagicOrb))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // TexRef 9 :: STAFF WOOD
    texFilename = "../resources/textures/staffwood.png";
    if (!UCreateTexture(texFilename, gTextureIdStaffWood))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Controls output to console
    std::cout << ("::::: CONTROLS :::::\nWASD         :: Move Camera\nQ/E-Spc/Ctrl :: Up / Down\nMouse        :: Pitch / Yaw\n") <<
        ("Scroll       :: Move Speed\n1 / 2        :: Increase / Decrease Camera Speed\nP            :: Persp/Ortho Toggle\n") << 
        ("L            :: Lightswitch Toggle") << endl;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
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

    // Release mesh data
    UDestroyMesh(gHammer); 
    UDestroyMesh(gHammerHandle);
    UDestroyMesh(gHammerHandleMetal);

    UDestroyMesh(gScythe);
    UDestroyMesh(gScytheBlade);
    UDestroyMesh(gScytheSupport);

    UDestroyMesh(gSwordHandle);
    UDestroyMesh(gSwordPommel);
    UDestroyMesh(gSwordBlade);

    UDestroyMesh(gStaff);
    UDestroyMesh(gStaffOrb);
    UDestroyMesh(gStaffSupport);

    UDestroyMesh(gPlane);

    // Release texture
    UDestroyTexture(gTextureIdMetal);
    UDestroyTexture(gTextureIdConcrete);
    UDestroyTexture(gTextureIdLeather);
    UDestroyTexture(gTextureIdHammerHandleMetal);
    UDestroyTexture(gTextureIdWood);
    UDestroyTexture(gTextureIdBladeMetal);
    UDestroyTexture(gTextureIdCrystal);
    UDestroyTexture(gTextureIdMagicOrb);
    UDestroyTexture(gTextureIdStaffWood);

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
    glfwSetKeyCallback(*window, UKeyboardKeyCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    // Escape key exits program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // WASD movement controls, W = forward, S = back, A = left, D = right
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);

    // Up and Down controls, added space and left control as an option as it is more comfortable for me to navigate with.
    if ((glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS))
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS))
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
}

void UKeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Mouse Sensitivity Controls, performed with 1 to decrease and 2 to increase. This is performed in a key callback function
    // so that the sensitivity is changed exactly once at key press and cannot be held down. Every press adjusts sensitivity by +-0.01.
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        gCamera.MouseSensitivity -= 0.01f;
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        gCamera.MouseSensitivity += 0.01f;
    }

    // Ortho or Perspective toggle, simply sets a bool value to true or false
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        if (projMode) {
            projMode = false;
            std::cout << "orthographic" << endl;
            return;
        }
        projMode = true;
        std::cout << "perspective" << endl;
    }

    // Ortho or Perspective toggle, simply sets a bool value to true or false
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        if (lightSwitch) {
            lightSwitch = false;
            std::cout << "lights off" << endl;
            return;
        }
        lightSwitch = true;
        std::cout << "lights on" << endl;
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
    // Allows movement speed adjustments with scroll wheel. Speed's tick set to 0.20 to allow smoother changes.
    gCamera.MovementSpeed += yoffset * 0.20f;
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS && lightSwitch == true)
        {
            cout << "Left mouse button pressed" << endl;
        }
        else
        {
            cout << "Left mouse button released" << endl;
        }
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Function to switch projection matrix modes based on the projMode bool, returning a perspective or ortho view
glm::mat4 UGetProjectionMatrix()
{
    if (projMode)
        return glm::perspective(70.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    else 
        return glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -10.0f, 100.0f);   
}

// Functioned called to render a frame
void URender()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Clear the frame and z buffers
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(gProgramId);


    /*  LIGHTING    :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::   */

    const glm::vec3 cameraPosition = gCamera.Position;
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
    
    // Lighting Controls (press L)
    if(lightSwitch)
    { 
    // directional lights on
    GLint dirLightDirection = glGetUniformLocation(gProgramId, "dirLight.direction"); // direction of light, back side as it is the sun in photo
    glUniform3f(dirLightDirection, 0.0f, -1.0f, 0.5f);
    GLint dirLightAmbient = glGetUniformLocation(gProgramId, "dirLight.ambient"); // ambient a bit yellowish
    glUniform3f(dirLightAmbient, 0.4f, 0.4f, 0.49f);
    GLint dirLightDiffuse = glGetUniformLocation(gProgramId, "dirLight.diffuse"); // yellowish diffuse
    glUniform3f(dirLightDiffuse, 0.8f, 0.8f, 0.87f);
    GLint dirLightSpecular = glGetUniformLocation(gProgramId, "dirLight.specular"); // white specular
    glUniform3f(dirLightSpecular, 0.8f, 0.8f, 0.8f);
    }
    else {
        // directional lights off
        GLint dirLightDirection = glGetUniformLocation(gProgramId, "dirLight.direction"); // direction of light, back side as it is the sun in photo
        glUniform3f(dirLightDirection, 0.0f, -1.0f, 0.5f);
        GLint dirLightAmbient = glGetUniformLocation(gProgramId, "dirLight.ambient"); // ambient a bit yellowish
        glUniform3f(dirLightAmbient, 0.01f, 0.01f, 0.009f);
        GLint dirLightDiffuse = glGetUniformLocation(gProgramId, "dirLight.diffuse"); // yellowish diffuse
        glUniform3f(dirLightDiffuse, 0.1f, 0.1f, 0.07f);
        GLint dirLightSpecular = glGetUniformLocation(gProgramId, "dirLight.specular"); // white specular
        glUniform3f(dirLightSpecular, 0.8f, 0.8f, 0.8f);
    }
    
    // rotating point light #1
    bool gIsLampOrbiting = true;
    const float angularVelocity = glm::radians(45.0f);
    if (gIsLampOrbiting)
    {
        float angle = angularVelocity * gDeltaTime;
        glm::vec3 rotationAxis(0.0f, 0.5f, 0.0f);
        glm::vec4 newPosition = glm::rotate(angle, rotationAxis) * glm::vec4(gLightPosition, 1.0f);
        gLightPosition.x = newPosition.x;
        gLightPosition.y = newPosition.y;
        gLightPosition.z = newPosition.z;
    }

    // point light 1 - rotating white
    GLint pointLightPosition0 = glGetUniformLocation(gProgramId, "pointLights[0].position");
    glUniform3f(pointLightPosition0, gLightPosition.x, gLightPosition.y, gLightPosition.z);     // Assigns point light position
    pointLightPosition0 = glGetUniformLocation(gProgramId, "pointLights[0].ambient");
    glUniform3f(pointLightPosition0, 0.1f, 0.1f, 0.1f);                                       // Set ambient color / intensity
    pointLightPosition0 = glGetUniformLocation(gProgramId, "pointLights[0].diffuse");
    glUniform3f(pointLightPosition0, 0.9f, 0.9f, 0.9f);                                        // Set diffuse color / intensity
    pointLightPosition0 = glGetUniformLocation(gProgramId, "pointLights[0].specular");
    glUniform3f(pointLightPosition0, 1.0f, 1.0f, 1.0f);                                        // Set specular color / intensity
    pointLightPosition0 = glGetUniformLocation(gProgramId, "pointLights[0].constant");
    glUniform1f(pointLightPosition0, 1.0f);                                                     // Set constant variable (for attenuation)
    pointLightPosition0 = glGetUniformLocation(gProgramId, "pointLights[0].linear");
    glUniform1f(pointLightPosition0, 0.022);                                                     // Set linear variable (for attenuation)
    pointLightPosition0 = glGetUniformLocation(gProgramId, "pointLights[0].quadratic");
    glUniform1f(pointLightPosition0, 0.0019);                                                    // Set quadratic variable (for attenuation)
    
    // point light 2 - Scythe Light Green
    GLint pointLightPosition1 = glGetUniformLocation(gProgramId, "pointLights[1].position");
    glUniform3f(pointLightPosition1, pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);     // Assigns point light position
    pointLightPosition1 = glGetUniformLocation(gProgramId, "pointLights[1].ambient");
    glUniform3f(pointLightPosition1, 0.0f, 0.1f, 0.0f);                                       // Set ambient color / intensity
    pointLightPosition1 = glGetUniformLocation(gProgramId, "pointLights[1].diffuse");
    glUniform3f(pointLightPosition1, 0.0f, 1.0f, 0.0f);                                        // Set diffuse color / intensity
    pointLightPosition1 = glGetUniformLocation(gProgramId, "pointLights[1].specular");
    glUniform3f(pointLightPosition1, 0.5f, 1.0f, 0.5f);                                        // Set specular color / intensity
    pointLightPosition1 = glGetUniformLocation(gProgramId, "pointLights[1].constant");
    glUniform1f(pointLightPosition1, 1.0f);                                                     // Set constant variable (for attenuation)
    pointLightPosition1 = glGetUniformLocation(gProgramId, "pointLights[1].linear");
    glUniform1f(pointLightPosition1, 0.14);                                                     // Set linear variable (for attenuation)
    pointLightPosition1 = glGetUniformLocation(gProgramId, "pointLights[1].quadratic");
    glUniform1f(pointLightPosition1, 0.44);                                                    // Set quadratic variable (for attenuation)

    // point light 3 - Sword Light Yellow
    GLint pointLightPosition2 = glGetUniformLocation(gProgramId, "pointLights[2].position");
    glUniform3f(pointLightPosition2, pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);     // Assigns point light position
    pointLightPosition2 = glGetUniformLocation(gProgramId, "pointLights[2].ambient");
    glUniform3f(pointLightPosition2, 0.1f, 0.1f, 0.1f);                                       // Set ambient color / intensity
    pointLightPosition2 = glGetUniformLocation(gProgramId, "pointLights[2].diffuse");
    glUniform3f(pointLightPosition2, 1.0f, 1.0f, 0.4f);                                        // Set diffuse color / intensity
    pointLightPosition2 = glGetUniformLocation(gProgramId, "pointLights[2].specular");
    glUniform3f(pointLightPosition2, 1.0f, 1.0f, 0.8f);                                        // Set specular color / intensity
    pointLightPosition2 = glGetUniformLocation(gProgramId, "pointLights[2].constant");
    glUniform1f(pointLightPosition2, 1.0f);                                                     // Set constant variable (for attenuation)
    pointLightPosition2 = glGetUniformLocation(gProgramId, "pointLights[2].linear");
    glUniform1f(pointLightPosition2, 0.14);                                                     // Set linear variable (for attenuation)
    pointLightPosition2 = glGetUniformLocation(gProgramId, "pointLights[2].quadratic");
    glUniform1f(pointLightPosition2, 0.44);                                                    // Set quadratic variable (for attenuation)

    // point light 4 - Magic Orb Light Red
    GLint pointLightPosition3 = glGetUniformLocation(gProgramId, "pointLights[3].position");
    glUniform3f(pointLightPosition3, pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);     // Assigns point light position
    pointLightPosition3 = glGetUniformLocation(gProgramId, "pointLights[3].ambient");
    glUniform3f(pointLightPosition3, 0.1f, 0.0f, 0.0f);                                       // Set ambient color / intensity
    pointLightPosition3 = glGetUniformLocation(gProgramId, "pointLights[3].diffuse");
    glUniform3f(pointLightPosition3, 1.0f, 0.0f, 0.0f);                                        // Set diffuse color / intensity
    pointLightPosition3 = glGetUniformLocation(gProgramId, "pointLights[3].specular");
    glUniform3f(pointLightPosition3, 1.0f, 0.5f, 0.5f);                                        // Set specular color / intensity
    pointLightPosition3 = glGetUniformLocation(gProgramId, "pointLights[3].constant");
    glUniform1f(pointLightPosition3, 1.0f);                                                     // Set constant variable (for attenuation)
    pointLightPosition3 = glGetUniformLocation(gProgramId, "pointLights[3].linear");
    glUniform1f(pointLightPosition3, 0.14);                                                     // Set linear variable (for attenuation)
    pointLightPosition3 = glGetUniformLocation(gProgramId, "pointLights[3].quadratic");
    glUniform1f(pointLightPosition3, 0.07);                                                    // Set quadratic variable (for attenuation)

    // point light 5 - Hammer Light Blue
    GLint pointLightPosition4 = glGetUniformLocation(gProgramId, "pointLights[4].position");
    glUniform3f(pointLightPosition4, pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z);     // Assigns point light position
    pointLightPosition4 = glGetUniformLocation(gProgramId, "pointLights[4].ambient");
    glUniform3f(pointLightPosition4, 0.0f, 0.0f, 0.1f);                                       // Set ambient color / intensity
    pointLightPosition4 = glGetUniformLocation(gProgramId, "pointLights[4].diffuse");
    glUniform3f(pointLightPosition4, 0.0f, 0.0f, 1.0f);                                        // Set diffuse color / intensity
    pointLightPosition4 = glGetUniformLocation(gProgramId, "pointLights[4].specular");
    glUniform3f(pointLightPosition4, 0.5f, 0.5, 1.0f);                                        // Set specular color / intensity
    pointLightPosition4 = glGetUniformLocation(gProgramId, "pointLights[4].constant");
    glUniform1f(pointLightPosition4, 1.0f);                                                     // Set constant variable (for attenuation)
    pointLightPosition4 = glGetUniformLocation(gProgramId, "pointLights[4].linear");
    glUniform1f(pointLightPosition4, 0.14);                                                     // Set linear variable (for attenuation)
    pointLightPosition4 = glGetUniformLocation(gProgramId, "pointLights[4].quadratic");
    glUniform1f(pointLightPosition4, 0.07);                                                    // Set quadratic variable (for attenuation)
    
                                                                                               // Transforms the camera: move the camera back (z axis)
 
    // Grabs projection matrix from UGetProjectionMatrix() based on user's toggle
    glm::mat4 projection = UGetProjectionMatrix();
   
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");
    glm::mat4 view = gCamera.GetViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Set the shader to be used
    glUseProgram(gProgramId);
    // Create fragment shader texture array ref
    auto loc = glGetUniformLocation(gProgramId, "u_Textures");
    int samplers[10] = { 0,1,2,3,4,5,6,7,8,9 };
    // Grab uniform value
    glUniform1iv(loc, 10, samplers);
    
    // Each texture here need to correspond with a location in fragment texture array. CURRENT SIZE : 9
    // Set texture array 0 to blank texture for complete transparency
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blankTexture);
    
    // Set texture array 1 to CONCRETE
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, gTextureIdConcrete);
    
    // Set texture array 2 to METAL
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, gTextureIdMetal);

    // Set texture array 3 to LEATHER
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, gTextureIdLeather);

    // Set texture array 4 to HAMMER METAL
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, gTextureIdHammerHandleMetal);

    // Set texture array 5 to WOOD
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_2D, gTextureIdWood);

    // Set texture array 6 to BLADE METAL
    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_2D, gTextureIdBladeMetal);

    // Set texture array 7 to CRYSTAL
    glActiveTexture(GL_TEXTURE0 + 7);
    glBindTexture(GL_TEXTURE_2D, gTextureIdCrystal);

    // Set texture array 8 to MAGIC ORB
    glActiveTexture(GL_TEXTURE0 + 8);
    glBindTexture(GL_TEXTURE_2D, gTextureIdMagicOrb);

    // Set texture array 9 to STAFF WOOD
    glActiveTexture(GL_TEXTURE0 + 9);
    glBindTexture(GL_TEXTURE_2D, gTextureIdStaffWood);

    // Draw the meshes
    DrawPlane();

    DrawHammer();    
    DrawHammerHandleMetal();
    DrawHammerHandle();

    DrawScythe();
    DrawScytheBlade();
    DrawScytheSupport();

    DrawSwordHandle();
    DrawSwordPommel();
    DrawSwordBlade();

    DrawStaff();
    DrawStaffOrb();
    DrawStaffSupport();

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.

}

void DrawHammer()
{
    // Set Hammer Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 32.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.1f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);    
    // Set material specular
    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gHammer.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(0.5f, 0.5f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // 1. Scales the object
    glm::mat4 scale = glm::scale(glm::vec3(0.9f, 1.0f, 1.0f));
    // 2. Rotates shape
    glm::mat4 rotation = glm::rotate(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. No Translation
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Draws the triangles using triangles mode
    glDrawArrays(GL_TRIANGLES, 0, gHammer.nVertices); 

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void DrawHammerHandle()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 1.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);                                                        // Set material specular
    glBindVertexArray(gHammerHandle.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(1.0f, 1.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // 1. Scales the object
    glm::mat4 scale = glm::scale(glm::vec3(0.41f, 1.49f, 0.41f));
    // 2. Rotates shape
    glm::mat4 rotation = glm::rotate(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Translation
    glm::mat4 translation = glm::translate(glm::vec3(-0.17f, 1.25f, -0.1f));

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Draws the triangles using triangle strip mode
    glDrawArrays(GL_TRIANGLE_STRIP, 0, gHammerHandle.nVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void DrawHammerHandleMetal()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 100.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 24.0f);                                                        // Set material specular
    glBindVertexArray(gHammerHandleMetal.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(1.0f, 1.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // 1. Scales the object
    glm::mat4 scale = glm::scale(glm::vec3(0.4f, 1.5f, 0.4f));
    // 2. Rotates shape
    glm::mat4 rotation = glm::rotate(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Translation
    glm::mat4 translation = glm::translate(glm::vec3(-0.17f, 1.25f, -0.1f));

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Draws the triangles using triangle strip mode
    glDrawArrays(GL_TRIANGLE_STRIP, 0, gHammerHandleMetal.nVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void DrawScythe()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 6.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);                                                        // Set material specular
    glBindVertexArray(gScythe.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(0.5f, 0.5f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Draws handle of scythe using loop, i as modifier
    for(float i = 0; i < 4; i++)
    {
        // 1. Scales the object
        glm::mat4 scale = glm::scale(glm::vec3(0.41f, 2.49f, 0.41f));
        // 2. Rotates shape
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(-3.17f, 0.11f, 5.0f - (i * 2.0f)));
        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;
        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Draws the triangles using traingle strip mode
        glDrawArrays(GL_TRIANGLE_STRIP, 0, gScythe.nVertices);
    }

    for (float i = 0; i < 2; i++)
    {
        // 1. Scales the object
        glm::mat4 scale = glm::scale(glm::vec3(0.35f, 0.60f, 0.35f));
        // 2. Rotates shape
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f - ( i * 0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(-3.50f, 0.11f + (i * 0.175f), 4.0f - (i * 2.0f)));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gScythe.nVertices);
    }

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void DrawScytheBlade()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 100.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 24.0f);                                                        // Set material specular
    glBindVertexArray(gScytheBlade.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(1.0f, 1.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));


        glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 0.35f));
        // 2. Rotates shape
        glm::mat4 rotation = glm::rotate(3.141592f * (1.05f), glm::vec3(0.0f, 1.0f, 0.0f));
        // 3. No Translation
        glm::mat4 translation = glm::translate(glm::vec3(-3.25f, 0.1f, -2.0f));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Draws the triangles using triangle strip mode
        glDrawArrays(GL_TRIANGLE_STRIP, 0, gScytheBlade.nVertices);


    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void DrawScytheSupport()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 32.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);                                                        // Set material specular
    glBindVertexArray(gScytheSupport.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(1.0f, 1.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));
    // Loop for drawing the handle connectors
    for (float i = 0.0f; i < 2.0f; i++)
    {
        glm::mat4 scale = glm::scale(glm::vec3(0.5f, 0.05f, 0.5f));
        // 2. Rotates shape
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(-3.17f, 0.11f, 2.0f+(i*2)));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Draws the triangles using triangle strip mode
        glDrawArrays(GL_TRIANGLE_STRIP, 0, gScytheSupport.nVertices);
    }
    // loop for drawing bolts
    for (float i = 0; i < 2; i++)
    {
        // 1. Scales the object
        glm::mat4 scale = glm::scale(glm::vec3(0.40f, 0.10f, 0.40f));
        // 2. Rotates shape
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f - (i * 0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(-3.3f, 0.11f + (i * 0.065f), 4.0f - (i * 2.0f)));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gScytheSupport.nVertices);
    }

    glm::mat4 scale = glm::scale(glm::vec3(0.5f, 0.2f, 0.5f));
    // 2. Rotates shape
    glm::mat4 rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
    // 3. Translation
    glm::mat4 translation = glm::translate(glm::vec3(-3.17f, 0.11f, -2.245f));

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, gScytheSupport.nVertices);

    for (float i = 0.0f; i < 2; i++)
    {
        glm::mat4 scale = glm::scale(glm::vec3(0.15f, 0.1f, 0.15f));

        glm::mat4 rotation = glm::rotate(3.141592f * 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 translation = glm::translate(glm::vec3(-3.17f, 0.25f - (i*0.025f), -2.275f + (i*0.5f)));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gScytheSupport.nVertices);
    }
    glBindVertexArray(0);
}

void DrawSwordHandle()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 32.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);                                                        // Set material specular
    glBindVertexArray(gSwordHandle.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(1.0f, 1.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

        // 1. Scales the object
        glm::mat4 scale = glm::scale(glm::vec3(0.25f, 0.75f, 0.25f));
        // 2. Rotates shape
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.075f, 3.0f));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gSwordHandle.nVertices);




        // 1. Scales the object
        scale = glm::scale(glm::vec3(0.27f, 0.15f, 0.2f));
        // 2. Rotates shape
        rotation = glm::rotate(3.141592f * 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        translation = glm::translate(glm::vec3(0.0f, 0.075f, 2.6f));

        // Model matrix: transformations are applied right-to-left order
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gSwordHandle.nVertices);



        // 1. Scales the object
        scale = glm::scale(glm::vec3(0.2f, 0.125f, 0.2f));
        // 2. Rotates shape
        rotation = glm::rotate(3.141592f * 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        translation = glm::translate(glm::vec3(0.0f, 0.075f, 2.5f));

        // Model matrix: transformations are applied right-to-left order
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gSwordHandle.nVertices);



        // 1. Scales the object
        scale = glm::scale(glm::vec3(0.3f, 0.8f, 0.2f));
        // 2. Rotates shape
        rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
        // 3. Translation
        translation = glm::translate(glm::vec3(0.0f, 0.075f, 3.35f));

        // Model matrix: transformations are applied right-to-left order
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gSwordHandle.nVertices);
        glBindVertexArray(0);
}

void DrawSwordPommel()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 100.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);                                                        // Set material specular
    glBindVertexArray(gSwordPommel.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(3.0f, 3.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

        // 1. Scales the object
        glm::mat4 scale = glm::scale(glm::vec3(0.075f, 0.075f, 0.075f));
        // 2. Rotates shape
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.075f, 2.5f));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gSwordPommel.nVertices);
    glBindVertexArray(0);
}

void DrawSwordBlade()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 64.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);                                                        // Set material specular
    glBindVertexArray(gSwordBlade.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(1.0f, 1.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // 1. Scales the object
    glm::mat4 scale = glm::scale(glm::vec3(0.2f, 0.1f, 3.0f));
    // 2. Rotates shape
    glm::mat4 rotation = glm::rotate(3.141592f * 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Translation
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.025f, 3.35f));

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, gSwordBlade.nVertices);
    glBindVertexArray(0);
}

void DrawStaff()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 6.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);                                                        // Set material specular
    glBindVertexArray(gStaff.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(0.5f, 0.5f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // Main wood
    for (float i = 0; i < 3; i++)
    {
        // 1. Scales the object
        glm::mat4 scale = glm::scale(glm::vec3(0.3f, 2.49f, 0.3f));
        // 2. Rotates shape
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(3.17f, 0.11f, 5.0f - (i * 2.0f)));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaff.nVertices);
    }

    // Staff  head
        // 1. Scales
        glm::mat4 scale = glm::scale(glm::vec3(0.1f, 1.0f, 0.1f));
        // 2. Rotates
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        rotation += glm::rotate(3.141592f * 4.75f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(3.17f, 0.4f, -0.525f));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaff.nVertices);



        // 1. Scales
        scale = glm::scale(glm::vec3(0.1f, 1.0f, 0.1f));
        // 2. Rotates
        rotation = glm::rotate(3.141592f * 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        translation = glm::translate(glm::vec3(3.17f, 1.05f, -0.925f));

        // Model matrix: transformations are applied right-to-left order
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaff.nVertices);



        // 1. Scales
        scale = glm::scale(glm::vec3(0.1f, 1.0f, 0.1f));
        // 2. Rotates
        rotation = glm::rotate(3.141592f * 2.35619f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        translation = glm::translate(glm::vec3(3.17f, 1.7f, -0.5f));

        // Model matrix: transformations are applied right-to-left order
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaff.nVertices);



        // 1. Scales
        scale = glm::scale(glm::vec3(0.1f, 1.0f, 0.1f));
        // 2. Rotates
        rotation = glm::rotate(3.141592f * 0.785398f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        translation = glm::translate(glm::vec3(3.17f, 1.56f, 0.25f));

        // Model matrix: transformations are applied right-to-left order
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaff.nVertices);



    // Staff tail
    for (float i = 0.0f; i < 6; i++)
    {
        // 1. Scales
        glm::mat4 scale = glm::scale(glm::vec3(0.3f, 0.1f + (i * 0.005f), 0.3f));
        // 2. Rotates
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f + (-i * 0.25f), glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(3.17f, 0.11f + pow((i * 0.1f),2), 6.29f + (i * 0.05f)));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaff.nVertices);
    }
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

void DrawStaffOrb()
{
    bool orbOrbit = true;
    const float angularVelocity = glm::radians(4.0f);
    // moves orb in magical directions
    if (orbOrbit)
    {
        float angle = angularVelocity * gDeltaTime / 5000.0f;
        glm::vec3 rotationAxis(0.0f, 0.5f, 0.0f);
        glm::vec4 newPosition = glm::rotate(angle, rotationAxis) * glm::vec4(gLightPosition, 1.0f);
        gLightPosition.x = newPosition.x;
        gLightPosition.y = newPosition.y;
        gLightPosition.z = newPosition.z;
    }
    // Set Plane Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 100.0f);                                                       // Set material shininess, very shiny for module 6 marble
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);                                                        // Set material specular
    glBindVertexArray(gStaffOrb.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(3.0f, 3.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // 1. Scales
    glm::mat4 scale = glm::scale(glm::vec3(0.075f, 0.075f, 0.075f));
    // 2. Rotates
    glm::mat4 rotation = glm::rotate(3.141592f * 0.1f, glm::vec3(1.0f, -gLightPosition.x, 0.0f));
    // 3. Translation
    glm::mat4 translation = glm::translate(glm::vec3(3.17f - (gLightPosition.x * 0.01), 1.0f - (gLightPosition.x * 0.01), -0.25f - (gLightPosition.x * 0.01)));

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaffOrb.nVertices);
    glBindVertexArray(0);
}

void DrawStaffSupport()
{
    // Set Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 100.0f);                                                       // Set material shininess
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 1.0f);                                                        // Set material specular
    glBindVertexArray(gStaffSupport.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(1.0f, 1.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // 1. Scales
        glm::mat4 scale = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
        // 2. Rotates
        glm::mat4 rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        glm::mat4 translation = glm::translate(glm::vec3(3.17f, 0.11f, 0.0f));

        // Model matrix: transformations are applied right-to-left order
        glm::mat4 model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        GLint modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaffSupport.nVertices);


        // 1. Scales
        scale = glm::scale(glm::vec3(0.25f, 0.1f, 0.25f));
        // 2. Rotates
        rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        rotation += glm::rotate(3.141592f * 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        translation = glm::translate(glm::vec3(3.17f, 0.56f, -0.94f));

        // Model matrix: transformations are applied right-to-left order
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaffSupport.nVertices);



        // 1. Scales
        scale = glm::scale(glm::vec3(0.25f, 0.1f, 0.25f));
        // 2. Rotates
        rotation = glm::rotate(3.141592f * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        rotation += glm::rotate(3.141592f * 0.785398f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        translation = glm::translate(glm::vec3(3.17f, 1.50f, -0.94f));

        // Model matrix: transformations are applied right-to-left order
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaffSupport.nVertices);



        // 1. Scales
        scale = glm::scale(glm::vec3(0.25f, 0.1f, 0.25f));
        // 2. Rotates
        rotation = glm::rotate(3.141592f * 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        // 3. Translation
        translation = glm::translate(glm::vec3(3.17f, 1.925f, -0.05f));

        // Model matrix: transformations are applied right-to-left order
        model = translation * rotation * scale;

        // Retrieves and passes transform matrices to the Shader program
        modelLoc = glGetUniformLocation(gProgramId, "model");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, gStaffSupport.nVertices);
        glBindVertexArray(0);
}

void DrawPlane()
{
    // Set Plane Material
    GLint materialShininess = glGetUniformLocation(gProgramId, "material.shininess");
    glUniform1f(materialShininess, 500.0f);                                                       // Set material shininess, very shiny for module 6 marble
    GLint materialDiffuse = glGetUniformLocation(gProgramId, "material.diffuse");
    glUniform1f(materialDiffuse, 1.0f);                                                         // Set material diffuse
    GLint materialSpecular = glGetUniformLocation(gProgramId, "material.specular");
    glUniform1f(materialSpecular, 3.0f);                                                        // Set material specular
    glBindVertexArray(gPlane.vao);

    // Scale Texture
    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    gUVScale = glm::vec2(1.0f, 1.0f);
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    glm::mat4 scale = glm::scale(glm::vec3(0.35f, 0.35f, 0.35f));
    // 2. Rotates shape by 120 degrees on the x axis
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    // 3. No Translation
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));

    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Draws the triangles using triangles mode
    glDrawArrays(GL_TRIANGLES, 0, gPlane.nVertices); // Draws the pyramid outlined

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
}

// Implements the UCreatePlane function
void UCreatePlane(GLMesh& mesh)
{

    GLfloat verts[] = {
       // Plane
       // Vertex Position           Color                      Tex Coord        Tex Index   Normal         
          -25.0f, 0.0f, -25.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      1.0f,       0.0f, 1.0f, 0.0f,  // (0, 0) Bottom Left
          -25.0f, 0.0f,  25.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      1.0f,       0.0f, 1.0f, 0.0f,// (0, 1) Top Left
           25.0f, 0.0f,  25.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      1.0f,       0.0f, 1.0f, 0.0f,// (1, 1) Top Right

           25.0f, 0.0f,  25.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      1.0f,       0.0f, 1.0f, 0.0f,// (0, 1) Top Right
           25.0f, 0.0f, -25.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      1.0f,       0.0f, 1.0f, 0.0f,// (1, 0) Bottom Right
          -25.0f, 0.0f, -25.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      1.0f,       0.0f, 1.0f, 0.0f,// (0, 0) Bottom Left
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

     // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}
const int STRIDE = 13;
void UCreateCylinder(GLfloat verts[], int segments, float radius, float halfLen, float texId)
{
    int t, idx = 0;
    float increment = (float)(360.0f / (float)(segments - 1));
    const float pi = 3.1415926f;
    //create top
    int iZeroCounter = 2;
    float hh = halfLen / 2.0f;
    for (t = 0; t < segments; t++) {
        float angle = (float)(pi / 180.0f * t * increment);
        float thisCos = (float)(radius * cos(angle));
        float thisSin = (float)(radius * sin(angle));

        if (iZeroCounter++ >= 2) {
            verts[idx++] = 0.0f;
            verts[idx++] = +hh;
            verts[idx++] = 0.0f;
            verts[idx++] = 0.0f;                   // r
            verts[idx++] = 1.0f;                   // g
            verts[idx++] = 0.0f;                   // b
            verts[idx++] = 1.0f;                   // a
            verts[idx++] = angle / (2 * pi);                            // t.x
            verts[idx++] = (hh);     // t.y
            verts[idx++] = texId;                                       // t.i

            verts[idx++] = 0.0f;                                        // n.x
            verts[idx++] = 1.0f;                                        // n.y
            verts[idx++] = 0.0f;                                        // n.z
            iZeroCounter = 0;
        }

        verts[idx++] = thisCos;
        verts[idx++] = +hh;
        verts[idx++] = thisSin;
        verts[idx++] = 0.0f;                   // r
        verts[idx++] = 0.0f;                   // g
        verts[idx++] = 0.0f;                   // b
        verts[idx++] = 0.0f;                   // a
        verts[idx++] = angle / (2 * pi);                                // t.x
        verts[idx++] = ((hh + (halfLen / 2.0f)) / halfLen);             // t.y
        verts[idx++] = texId;                                           // t.i

        verts[idx++] = 0.0f;                                            // n.x
        verts[idx++] = 1.0f;                                            // n.y
        verts[idx++] = 0.0f;                                            // n.z


    }
    // create bottom
    hh = -halfLen / 2.0f;
    iZeroCounter = 2;
    for (t = 0; t < segments; t++) {
        float angle = (float)(pi / 180.0f * t * increment);
        float thisCos = (float)(radius * cos(angle));
        float thisSin = (float)(radius * sin(angle));

        if (iZeroCounter++ >= 2) {
            verts[idx++] = 0.0f;
            verts[idx++] = hh;
            verts[idx++] = 0.0f;
            verts[idx++] = 0.0f;                   // r
            verts[idx++] = 1.0f;                   // g
            verts[idx++] = 0.0f;                   // b
            verts[idx++] = 1.0f;                   // a
            verts[idx++] = angle / (2 * pi);       // t.x
            verts[idx++] = (hh);                   // t.y
            verts[idx++] = texId;                  // t.i

            verts[idx++] = 0.0f;                   // n.x
            verts[idx++] = -1.0f;                  // n.y
            verts[idx++] = 0.0f;                   // n.z
            iZeroCounter = 0;
        }
        verts[idx++] = thisCos;
        verts[idx++] = hh;
        verts[idx++] = thisSin;
        verts[idx++] = 0.0f;                                            // r
        verts[idx++] = 1.0f;                                            // g
        verts[idx++] = 0.0f;                                            // b
        verts[idx++] = 1.0f;                                            // a
        verts[idx++] = angle / (2 * pi);                                // t.x
        verts[idx++] = ((hh + (halfLen / 2.0f)) / halfLen);             // t.y
        verts[idx++] = texId;                                           // t.i

        verts[idx++] = 0.0f;                                            // n.x
        verts[idx++] = -1.0f;                                            // n.y
        verts[idx++] = 0.0f;                                            // n.z


    }

    // create the cylinder sides
    for (t = 0; t < segments + 2; t++) {
        float angle = (float)(pi / 180.0f * t * increment);
        float thisCos = (float)(radius * cos(angle));
        float thisSin = (float)-(radius * sin(angle));

        verts[idx++] = thisCos;
        verts[idx++] = hh;
        verts[idx++] = thisSin;
        verts[idx++] = 0.0f;                   // r
        verts[idx++] = 1.0f;                   // g
        verts[idx++] = 0.0f;                   // b
        verts[idx++] = 1.0f;                   // a

            // praise http://cse.csusb.edu/tongyu/courses/cs520/notes/texture.php for helping me with texture mapping equations
            verts[idx++] = angle/(2*pi);                        // t.x
            verts[idx++] = ((hh + (halfLen/2.0f)) / halfLen);   // t.y
            verts[idx++] = texId;                               // t.i

            verts[idx++] = thisCos;                             // n.x
            verts[idx++] = 0.0f;                                // n.y
            verts[idx++] = thisSin;                             // n.z
       
        hh *= -1.0f;
    }
    hh = halfLen / 2.0f;
    for (t = 0; t < segments; t++) {
        float angle = (float)(pi / 180.0f * t * increment);
        float thisCos = (float)-(radius * cos(angle));
        float thisSin = (float)(radius * sin(angle));

        if (iZeroCounter++ >= 2) {
            verts[idx++] = 0.0f;
            verts[idx++] = -hh;
            verts[idx++] = 0.0f;
            verts[idx++] = 0.0f;                   // r
            verts[idx++] = 1.0f;                   // g
            verts[idx++] = 0.0f;                   // b
            verts[idx++] = 1.0f;                   // a
            verts[idx++] = angle / (2 * pi);                        // t.x
            verts[idx++] = ((hh + (halfLen / 2.0f)) / halfLen);     // t.y
            verts[idx++] = texId;                                   // t.i

            verts[idx++] = thisCos;                                 // n.x
            verts[idx++] = 0.0f;                                    // n.y
            verts[idx++] = thisSin;                                 // n.z

            iZeroCounter = 0;
        }
        /* not needed
        verts[idx++] = thisCos;
        verts[idx++] = -hh;
        verts[idx++] = thisSin;
        verts[idx++] = 0.0f;                   // r
        verts[idx++] = 1.0f;                   // g
        verts[idx++] = 0.0f;                   // b
        verts[idx++] = 1.0f;                   // a
        verts[idx++] = angle / (2 * pi);                        // t.x
        verts[idx++] = ((hh + (halfLen / 2.0f)) / halfLen);     // t.y
        verts[idx++] = 3.0f;                                    // t.i

        verts[idx++] = thisCos;                                 // n.x
        verts[idx++] = 0.0f;                                    // n.y
        verts[idx++] = thisSin;                                 // n.z
        */
    }
}

void UCreateSphere(GLfloat verts[], float radius, int gradation, float texId)
{
    // Sphere algorithm found here: https://stackoverflow.com/questions/59890031/problem-texturing-sphere-using-triangle-strip
    // *** There is an issue this alogorithm creates that shoots an invisible triangle from the top of the sphere to infinity, need to fix
    // altered to be used with my stride setup
    const float PI = 3.141592f;
    float x, y, z, beta; // Storage for coordinates and angles
    int idx = 0; // array strider
    for (int i = 0; i < gradation; ++i) {
        float alpha = PI * (float)i / (float)(gradation);
        for (beta = 0.0f; beta < 2.01 * PI; beta += PI / gradation) {
            verts[idx++] = (float)(radius * cos(beta) * sin(alpha));    // x
            verts[idx++] = (float)(radius * sin(beta) * sin(alpha));    // y
            verts[idx++] = (float)(radius * cos(alpha));                // z
            verts[idx++] = 0.0f;                                        // r
            verts[idx++] = 1.0f;                                        // g
            verts[idx++] = 0.0f;                                        // b
            verts[idx++] = 1.0f;                                        // a
            verts[idx++] = beta / (2.0f * PI);                          // t.x
            verts[idx++] = alpha / PI;                                  // t.y
            verts[idx++] = texId;                                       // t.i
            verts[idx++] = (float)(radius * cos(beta) * sin(alpha));    // n.x
            verts[idx++] = (float)(radius * sin(beta) * sin(alpha));    // n.y
            verts[idx++] = (float)(radius * cos(alpha));                // n.z

            verts[idx++] = (float)(radius * cos(beta) * sin(alpha + PI / gradation));   // x
            verts[idx++] = (float)(radius * sin(beta) * sin(alpha + PI / gradation));   // y
            verts[idx++] = (float)(radius * cos(alpha + PI / gradation));               // z
            verts[idx++] = 0.0f;                                                        // r
            verts[idx++] = 1.0f;                                                        // g
            verts[idx++] = 0.0f;                                                        // b
            verts[idx++] = 1.0f;                                                        // a
            verts[idx++] = beta / (2.0f * PI);                                          // t.x
            verts[idx++] = alpha / PI + 1.0f / gradation;                               // t.y
            verts[idx++] = texId;                                                       // t.i
            verts[idx++] = (float)(radius * cos(beta) * sin(alpha));                    // n.x
            verts[idx++] = (float)(radius * sin(beta) * sin(alpha));                    // n.y
            verts[idx++] = (float)(radius * cos(alpha));                                // n.z
        }
    }
}

void UCreateHammerHandle(GLMesh& mesh)
{
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (6 * NUM_SIDES);
    GLfloat verts[NUM_VERTICES];
    UCreateCylinder(verts, NUM_SIDES, 0.2f, 1.0f, 3.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateHammerHandleMetal(GLMesh& mesh)
{
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (6 * NUM_SIDES);
    GLfloat verts[NUM_VERTICES];
    UCreateCylinder(verts, NUM_SIDES, 0.2f, 1.0f, 4.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

// Implements the UCreatePlane function
void UCreateHammer(GLMesh& mesh)
{

    // Position and Color data
    GLfloat verts[] = {
        // Vertex Positions    Colors (r,g,b,a)            Tex Coord(x,y)  Tex i    Normal

        // Hammer Base
          //  Bottom
          -0.5f, 0.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, -1.0f, 0.0f,  // (0, 0) FL 1
           0.5f, 0.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   0.0f, -1.0f, 0.0f, // (1, 0) FR 2
           0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, -1.0f, 0.0f, // (1, 1) BR 3
          
           0.5f, 0.0f, -0.4f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     2.0f,   0.0f, -1.0f, 0.0f, // (1, 1) BR 3
          -0.5f, 0.0f, -0.4f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     2.0f,   0.0f, -1.0f, 0.0f, // (0, 1) BL 4 
          -0.5f, 0.0f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     2.0f,   0.0f, -1.0f, 0.0f, // (0, 0) FL 1

          // Front Bottom Edge
          -0.5f, 0.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, -1.0f, 1.0f, // (0, 0) FL 1
           0.5f, 0.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   0.0f, -1.0f, 1.0f, // (1, 0) FR 2
           0.5f, 0.1f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, -1.0f, 1.0f, // (1, 1) BR 5

           0.5f, 0.1f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, -1.0f, 1.0f, // (1, 1) BR 5
          -0.5f, 0.1f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   0.0f, -1.0f, 1.0f, // (0, 1) BL 6 
          -0.5f, 0.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, -1.0f, 1.0f, // (0, 0) FL 1

          // Front
          -0.5f, 0.1f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 0.0f, 1.0f, // (0, 1) BL 6 
           0.5f, 0.1f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   0.0f, 0.0f, 1.0f, // (1, 1) BR 5
           0.5f, 0.5f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 0.0f, 1.0f, // (1, 1) BR 7

           0.5f, 0.5f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 0.0f, 1.0f, // (1, 1) BR 7
          -0.5f, 0.5f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      2.0f,   0.0f, 0.0f, 1.0f, // (0, 1) BL 8 
          -0.5f, 0.1f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 0.0f, 1.0f, // (0, 1) BL 6

          // Front Top Edge
          -0.5f, 0.5f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 1.0f, 1.0f, // (0, 1) BL 8
           0.5f, 0.5f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   0.0f, 1.0f, 1.0f, // (1, 1) BR 7
           0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 1.0f, 1.0f, // (1, 1) BR 9

           0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 1.0f, 1.0f, // (1, 1) BR 9
          -0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   0.0f, 1.0f, 1.0f, // (0, 1) BL 10
          -0.5f, 0.5f,  0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 1.0f, 1.0f, // (0, 1) BL 8

          // Top
          -0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 1.0f, 0.0f, // (0, 1) BL 10
           0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   0.0f, 1.0f, 0.0f, // (1, 1) BR 9
           0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 1.0f, 0.0f, // (1, 1) BR 11

           0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 1.0f, 0.0f, // (1, 1) BR 11
          -0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   0.0f, 1.0f, 0.0f, // (1, 1) BR 12
          -0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 1.0f, 0.0f, // (0, 1) BL 10

          // Back Top Edge
          -0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 1.0f, -1.0f, // (0, 1) BL 12
           0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   0.0f, 1.0f, -1.0f, // (1, 1) BR 11
           0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 1.0f, -1.0f, // (1, 1) BR 13

           0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 1.0f, -1.0f, // (1, 1) BR 13
          -0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   0.0f, 1.0f, -1.0f, // (0, 1) BL 14
          -0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 1.0f, -1.0f, // (0, 1) BL 12

           // Back
          -0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 0.0f, -1.0f, // (0, 1) BL 14
           0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   0.0f, 0.0f, -1.0f, // (1, 1) BR 13
           0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 0.0f, -1.0f, // (1, 1) BR 15

           0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, 0.0f, -1.0f, // (1, 1) BR 15
          -0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   0.0f, 0.0f, -1.0f, // (0, 1) BL 16
          -0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, 0.0f, -1.0f, // (0, 1) BL 14

          // Back Bottom Edge
          -0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, -1.0f, -1.0f, // (0, 0) FL 16
           0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   0.0f, -1.0f, -1.0f, // (1, 0) FR 15
           0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, -1.0f, -1.0f, // (1, 1) BR 3

           0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   0.0f, -1.0f, -1.0f, // (1, 1) BR 3
          -0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   0.0f, -1.0f, -1.0f, // (0, 1) BL 4
          -0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   0.0f, -1.0f, -1.0f, // (0, 0) FL 16

          // Left Side
          -0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 0.0f, 0.0f, // (0, 0) FL 17
          -0.7f, 0.1f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   -1.0f, 0.0f, 0.0f, // (1, 0) FR 18
          -0.7f, 0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 0.0f, 0.0f, // (1, 1) BR 19

          -0.7f, 0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 0.0f, 0.0f, // (1, 1) BR 19
          -0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   -1.0f, 0.0f, 0.0f, // (0, 1) BL 20
          -0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 0.0f, 0.0f, // (0, 0) FL 17

          // Right Side
           0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 0.0f, 0.0f, // (0, 0) FL 20
           0.7f, 0.1f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   1.0f, 0.0f, 0.0f, // (1, 0) FR 19
           0.7f, 0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 0.0f, 0.0f, // (1, 1) BR 21

           0.7f, 0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 0.0f, 0.0f, // (1, 1) BR 21
           0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   1.0f, 0.0f, 0.0f, // (0, 1) BL 22
           0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 0.0f, 0.0f, // (0, 0) FL 20

          // Left Side Top Edge
          -0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 1.0f, 0.0f, // (0, 0) FL 20
          -0.7f, 0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   -1.0f, 1.0f, 0.0f, // (1, 0) FR 19
          -0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 1.0f, 0.0f, // (1, 1) BR 10

          -0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 1.0f, 0.0f, // (1, 1) BR 10
          -0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   -1.0f, 1.0f, 0.0f, // (0, 1) BL 12
          -0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 1.0f, 0.0f, // (0, 0) FL 20

          // Left Side Top Front Edge Triangle
          -0.5f, 0.6f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 1.0f, 1.0f, // (0, 0) FL 10
          -0.5f, 0.5f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   -1.0f, 1.0f, 1.0f, // (1, 0) FR 8
          -0.7f, 0.5f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 1.0f, 1.0f, // (1, 1) BR 19

          // Left Side Top Back Edge Triangle 
          -0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 1.0f, -1.0f, // (1, 1) BR 12
          -0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   -1.0f, 1.0f, -1.0f, // (0, 1) BL 20
          -0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 1.0f, -1.0f, // (0, 0) FL 14

          // Left Side Front Edge
          -0.7f, 0.5f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 0.0f, 1.0f, // (0, 0) FL 19
          -0.5f, 0.5f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   -1.0f, 0.0f, 1.0f, // (1, 0) FR 8
          -0.5f, 0.1f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 0.0f, 1.0f, // (1, 1) BR 6

          -0.5f, 0.1f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 0.0f, 1.0f, // (1, 1) BR 6
          -0.7f, 0.1f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   -1.0f, 0.0f, 1.0f, // (0, 1) BL 18
          -0.7f, 0.5f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 0.0f, 1.0f, // (0, 0) FL 19

          // Left Side Bottom Front Edge Triangle
          -0.5f, 0.1f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, -1.0f, 1.0f, // (0, 0) FL 6
          -0.5f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   -1.0f, -1.0f, 1.0f, // (1, 0) FR 1
          -0.7f, 0.1f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, -1.0f, 1.0f, // (1, 1) BR 18
            
          // Left Side Bottom Back Edge Triangle
          -0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, -1.0f, -1.0f, // (1, 1) BR 17
          -0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   -1.0f, -1.0f, -1.0f, // (0, 1) BL 16
          -0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, -1.0f, -1.0f, // (0, 0) FL 4

          // Left Side Bottom Edge
          -0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, -1.0f, 0.0f, // (0, 0) FL 4
          -0.5f, 0.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   -1.0f, -1.0f, 0.0f, // (1, 0) FR 1
          -0.7f, 0.1f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, -1.0f, 0.0f, // (1, 1) BR 18

          -0.7f, 0.1f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, -1.0f, 0.0f, // (1, 1) BR 18
          -0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   -1.0f, -1.0f, 0.0f, // (0, 1) BL 18
          -0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, -1.0f, 0.0f, // (0, 0) FL 4

          // Left Side Back Edge
          -0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 0.0f, -1.0f, // (0, 0) FL 17
          -0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   -1.0f, 0.0f, -1.0f, // (1, 0) FR 20
          -0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 0.0f, -1.0f, // (1, 1) BR 14

          -0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   -1.0f, 0.0f, -1.0f, // (1, 1) BR 14
          -0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   -1.0f, 0.0f, -1.0f, // (0, 1) BL 16
          -0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   -1.0f, 0.0f, -1.0f, // (0, 0) FL 17

          // Right Side Top Edge
           0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 1.0f, 0.0f, // (0, 0) FL 20
           0.7f, 0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   1.0f, 1.0f, 0.0f, // (1, 0) FR 19
           0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 1.0f, 0.0f, // (1, 1) BR 10

           0.5f, 0.6f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 1.0f, 0.0f, // (1, 1) BR 10
           0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   1.0f, 1.0f, 0.0f, // (0, 1) BL 12
           0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 1.0f, 0.0f, // (0, 0) FL 20

          // Right Side Top Front Edge Triangle
           0.5f, 0.6f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 1.0f, 1.0f, // (0, 0) FL 10
           0.5f, 0.5f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   1.0f, 1.0f, 1.0f, // (1, 0) FR 8
           0.7f, 0.5f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 1.0f, 1.0f, // (1, 1) BR 19

          // Right Side Top Back Edge Triangle 
           0.5f, 0.6f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 1.0f, -1.0f, // (1, 1) BR 12
           0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   1.0f, 1.0f, -1.0f, // (0, 1) BL 20
           0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 1.0f, -1.0f, // (0, 0) FL 14

          // Right Side Front Edge
           0.7f, 0.5f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 0.0f, 1.0f, // (0, 0) FL 19
           0.5f, 0.5f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   1.0f, 0.0f, 1.0f, // (1, 0) FR 8
           0.5f, 0.1f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 0.0f, 1.0f, // (1, 1) BR 6

           0.5f, 0.1f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 0.0f, 1.0f, // (1, 1) BR 6
           0.7f, 0.1f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   1.0f, 0.0f, 1.0f, // (0, 1) BL 18
           0.7f, 0.5f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 0.0f, 1.0f, // (0, 0) FL 19

          // Right Side Bottom Front Edge Triangle
           0.5f, 0.1f, 0.1f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, -1.0f, 1.0f, // (0, 0) FL 6
           0.5f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   1.0f, -1.0f, 1.0f, // (1, 0) FR 1
           0.7f, 0.1f, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, -1.0f, 1.0f, // (1, 1) BR 18
            
          // Right Side Bottom Back Edge Triangle
           0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, -1.0f, -1.0f, // (1, 1) BR 17
           0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   1.0f, -1.0f, -1.0f, // (0, 1) BL 16
           0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, -1.0f, -1.0f, // (0, 0) FL 4

          // Right Side Bottom Edge
           0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, -1.0f, 0.0f, // (0, 0) FL 4
           0.5f, 0.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   1.0f, -1.0f, 0.0f, // (1, 0) FR 1
           0.7f, 0.1f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, -1.0f, 0.0f, // (1, 1) BR 18

           0.7f, 0.1f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, -1.0f, 0.0f, // (1, 1) BR 18
           0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   1.0f, -1.0f, 0.0f, // (0, 1) BL 18
           0.5f, 0.0f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, -1.0f, 0.0f, // (0, 0) FL 4

          // Right Side Back Edge
           0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 0.0f, -1.0f, // (0, 0) FL 17
           0.7f, 0.5f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     2.0f,   1.0f, 0.0f, -1.0f, // (1, 0) FR 20
           0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 0.0f, -1.0f, // (1, 1) BR 14

           0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     2.0f,   1.0f, 0.0f, -1.0f, // (1, 1) BR 14
           0.5f, 0.1f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     2.0f,   1.0f, 0.0f, -1.0f, // (0, 1) BL 16
           0.7f, 0.1f, -0.4f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     2.0f,   1.0f, 0.0f, -1.0f, // (0, 0) FL 17

        /* Commented out - in new function
        // Hammer Handle (Will swap with cylinder soon)
          // Handle to Base
          -0.1f, 0.6f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   0.0f, -1.0f, 0.0f, // (0, 0) FL 1
           0.1f, 0.6f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     3.0f,   0.0f, -1.0f, 0.0f, // (1, 0) FR 2
           0.1f, 0.6f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   0.0f, -1.0f, 0.0f, // (1, 1) BR 3

           0.1f, 0.6f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   0.0f, -1.0f, 0.0f, // (1, 1) BR 3
          -0.1f, 0.6f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     3.0f,   0.0f, -1.0f, 0.0f, // (0, 1) BL 4
          -0.1f, 0.6f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   0.0f, -1.0f, 0.0f, // (0, 0) FL 1

         // Handle Top
          -0.1f, 2.0f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   0.0f,  1.0f, 0.0f, // (0, 0) FL 5
           0.1f, 2.0f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     3.0f,   0.0f,  1.0f, 0.0f, // (1, 0) FR 6
           0.1f, 2.0f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   0.0f,  1.0f, 0.0f, // (1, 1) BR 7

           0.1f, 2.0f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   0.0f,  1.0f, 0.0f, // (1, 1) BR 7
          -0.1f, 2.0f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     3.0f,   0.0f,  1.0f, 0.0f, // (0, 1) BL 8
          -0.1f, 2.0f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   0.0f,  1.0f, 0.0f, // (0, 0) FL 5

          // Handle Front
          -0.1f, 0.6f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   0.0f,  0.0f, 1.0f, // (0, 0) FL 1
           0.1f, 0.6f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     3.0f,   0.0f,  0.0f, 1.0f, // (1, 0) FR 2
           0.1f, 2.0f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   0.0f,  0.0f, 1.0f, // (1, 1) BR 6

           0.1f, 2.0f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   0.0f,  0.0f, 1.0f, // (1, 1) BR 6
          -0.1f, 2.0f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     3.0f,   0.0f,  0.0f, 1.0f, // (0, 1) BL 5
          -0.1f, 0.6f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   0.0f,  0.0f, 1.0f, // (0, 0) FL 1
              
          // Handle Left
          -0.1f, 0.6f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   -1.0f, 0.0f, 0.0f, // (0, 0) FL 4
          -0.1f, 0.6f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     3.0f,   -1.0f, 0.0f, 0.0f, // (1, 0) FR 1
          -0.1f, 2.0f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   -1.0f, 0.0f, 0.0f, // (1, 1) BR 5

          -0.1f, 2.0f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   -1.0f, 0.0f, 0.0f, // (1, 1) BR 5
          -0.1f, 2.0f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     3.0f,   -1.0f, 0.0f, 0.0f, // (0, 1) BL 8
          -0.1f, 0.6f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   -1.0f, 0.0f, 0.0f, // (0, 0) FL 4

          // Handle Back
          -0.1f, 0.6f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   0.0f, 0.0f, -1.0f, // (0, 0) FL 4
           0.1f, 0.6f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     3.0f,   0.0f, 0.0f, -1.0f, // (1, 0) FR 3
           0.1f, 2.0f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   0.0f, 0.0f, -1.0f, // (1, 1) BR 7

           0.1f, 2.0f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   0.0f, 0.0f, -1.0f, // (1, 1) BR 7
          -0.1f, 2.0f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     3.0f,   0.0f, 0.0f, -1.0f, // (0, 1) BL 8
          -0.1f, 0.6f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   0.0f, 0.0f, -1.0f, // (0, 0) FL 4

          // Handle Right
           0.1f, 0.6f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   1.0f, 0.0f, 0.0f, // (0, 0) FL 2
           0.1f, 0.6f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     3.0f,   1.0f, 0.0f, 0.0f, // (1, 0) FR 3
           0.1f, 2.0f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   1.0f, 0.0f, 0.0f, // (1, 1) BR 7

           0.1f, 2.0f, -0.3f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     3.0f,   1.0f, 0.0f, 0.0f, // (1, 1) BR 7
           0.1f, 2.0f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,     3.0f,   1.0f, 0.0f, 0.0f, // (0, 1) BL 6
           0.1f, 0.6f, -0.1f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     3.0f,   1.0f, 0.0f, 0.0f, // (0, 0) FL 2
           */
            
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)12);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)28);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)36);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float)* (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateScythe(GLMesh& mesh)
{
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (6 * NUM_SIDES);
    GLfloat verts[NUM_VERTICES];
    UCreateCylinder(verts, NUM_SIDES, 0.3f, 1.0f, 5.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateScytheBlade(GLMesh& mesh)
{
    GLfloat verts[] = {
        // Vertex Positions    Colors (r,g,b,a)            Tex Coord(x,y)  Tex i    Normal

        // Blade Base

           0.0f, 0.0f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f,  // (0, 0) FL 1
           0.0f, 0.0f,  1.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 0) FR 2
           1.1f, 0.0f,  1.1f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 1) BR 3

           1.1f, 0.0f,  1.1f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 1) BR 3
           1.1f, 0.0f,  0.1f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 1) BL 4 
           0.0f, 0.0f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 0) FL 1


           1.1f, 0.0f,  0.1f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 1) BR 3
           1.1f, 0.0f,  1.1f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 1) BL 4 
           2.1f, 0.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 0) FL 1  

           2.1f, 0.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 1) BR 3
           2.1f, 0.0f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 1) BL 4 
           1.1f, 0.0f,  0.1f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 0) FL 1  


           2.1f, 0.0f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 1) BR 3
           2.1f, 0.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 1) BL 4 
           3.1f, 0.0f,  0.6f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 0) FL 1  

           3.1f, 0.0f,  0.6f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 1) BR 3
           3.1f, 0.0f, -0.1f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 1) BL 4 
           2.1f, 0.0f,  0.0f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 0) FL 1  


           3.1f, 0.0f, -0.1f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 1) BR 3
           3.1f, 0.0f,  0.6f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 1) BL 4 
           4.1f, 0.0f,  0.1f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 0) FL 1  

           4.1f, 0.0f,  0.1f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 1) BR 3
           4.1f, 0.0f, -0.5f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 1) BL 4 
           3.1f, 0.0f, -0.1f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 0) FL 1  


           4.1f, 0.0f, -0.5f,    1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (1, 1) BR 3
           4.1f, 0.0f,  0.1f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 1) BL 4 
           5.1f, 0.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,     6.0f,   0.0f,   1.0f, 0.0f, // (0, 0) FL 1  
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateScytheSupport(GLMesh& mesh)
{
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (6 * NUM_SIDES);
    GLfloat verts[NUM_VERTICES];
    UCreateCylinder(verts, NUM_SIDES, 0.3f, 1.0f, 6.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateSwordHandle(GLMesh& mesh)
{
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (6 * NUM_SIDES);
    GLfloat verts[NUM_VERTICES];
    UCreateCylinder(verts, NUM_SIDES, 0.3f, 1.0f, 6.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateSwordPommel(GLMesh& mesh)
{
    const int gradation = 30;
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (6 * NUM_SIDES * gradation);
    GLfloat verts[NUM_VERTICES];
    UCreateSphere(verts, 1.0f, gradation, 7.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateSwordBlade(GLMesh& mesh)
{
    float texIndex = 2.0f;
    GLfloat verts[] = {
        // Blade
        // Vertex Position           Color                      Tex Coord        Tex Index   Normal    
            // Front
            0.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       0.0f, 0.0f, -1.0f, // 1 (0, 0) Bottom Left
            1.0f, 0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      texIndex,       0.0f, 0.0f, -1.0f, // 2 (0, 1) Top Left
            0.0f, 1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       0.0f, 0.0f, -1.0f, // 3 (1, 1) Top Right

            0.0f, 1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       0.0f, 0.0f, -1.0f, // 3 (0, 1) Top Right
           -1.0f, 0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      texIndex,       0.0f, 0.0f, -1.0f, // 4 (1, 0) Bottom Right
            0.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       0.0f, 0.0f, -1.0f, // 1 (0, 0) Bottom Left
            
            // Back
            0.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       0.0f, 0.0f, 1.0f, // 5 (0, 0) Bottom Left
            1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      texIndex,       0.0f, 0.0f, 1.0f, // 6 (0, 1) Top Left
            0.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       0.0f, 0.0f, 1.0f, // 7 (1, 1) Top Right

            0.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       0.0f, 0.0f, 1.0f, // 7 (0, 1) Top Right
           -1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      texIndex,       0.0f, 0.0f, 1.0f, // 8 (1, 0) Bottom Right
            0.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       0.0f, 0.0f, 1.0f, // 5 (0, 0) Bottom Left

            // Top Left
            0.0f, 1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       -1.0f, 1.0f, 0.0f, // 3 (0, 0) Bottom Left
            0.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      texIndex,       -1.0f, 1.0f, 0.0f, // 7 (0, 1) Top Left
           -1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       -1.0f, 1.0f, 0.0f, // 8 (1, 1) Top Right

           -1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       -1.0f, 1.0f, 0.0f, // 8 (0, 1) Top Right
           -1.0f, 0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      texIndex,       -1.0f, 1.0f, 0.0f, // 4 (1, 0) Bottom Right
            0.0f, 1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       -1.0f, 1.0f, 0.0f, // 3 (0, 0) Bottom Left

            // Top Right
            0.0f, 1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       1.0f, 1.0f, 0.0f, // 3 (0, 0) Bottom Left
            0.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      texIndex,       1.0f, 1.0f, 0.0f, // 7 (0, 1) Top Left
            1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       1.0f, 1.0f, 0.0f, // 6 (1, 1) Top Right

            1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       1.0f, 1.0f, 0.0f, // 6 (0, 1) Top Right
            1.0f, 0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      texIndex,       1.0f, 1.0f, 0.0f, // 2 (1, 0) Bottom Right
            0.0f, 1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       1.0f, 1.0f, 0.0f, // 3 (0, 0) Bottom Left

            // Bottom Right
            0.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       1.0f, -1.0f, 0.0f, // 1 (0, 0) Bottom Left
            0.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      texIndex,       1.0f, -1.0f, 0.0f, // 5 (0, 1) Top Left
            1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       1.0f, -1.0f, 0.0f, // 6 (1, 1) Top Right

            1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       1.0f, -1.0f, 0.0f, // 6 (0, 1) Top Right
            1.0f, 0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      texIndex,       1.0f, -1.0f, 0.0f, // 2 (1, 0) Bottom Right
            0.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       1.0f, -1.0f, 0.0f, // 1 (0, 0) Bottom Left

            // Bottom Left
            0.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,      -1.0f, -1.0f, 0.0f, // 1 (0, 0) Bottom Left
            0.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      texIndex,      -1.0f, -1.0f, 0.0f, // 5 (0, 1) Top Left
           -1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,      -1.0f, -1.0f, 0.0f, // 8 (1, 1) Top Right

           -1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,      -1.0f, -1.0f, 0.0f, // 8 (0, 1) Top Right
           -1.0f, 0.5f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      texIndex,      -1.0f, -1.0f, 0.0f, // 4 (1, 0) Bottom Right
            0.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,      -1.0f, -1.0f, 0.0f, // 1 (0, 0) Bottom Left

            // Tip
            // Top Right
            0.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,      -1.0f, 1.0f,  0.0f, // 7 (0, 0) Bottom Left
           -1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      texIndex,      -1.0f, 1.0f,  0.0f, // 8 (0, 1) Top Left
            0.0f, 0.5f, 1.1f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,      -1.0f, 1.0f,  0.0f, // 9 (1, 1) Top Right
            // Top Left
            0.0f, 1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       1.0f, 1.0f,  0.0f, // 7 (0, 1) Top Right
            1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      texIndex,       1.0f, 1.0f,  0.0f, // 6 (1, 0) Bottom Right
            0.0f, 0.5f, 1.1f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       1.0f, 1.0f,  0.0f, // 9 (0, 0) Bottom Left
            // Bottom Right
            0.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,      -1.0f, 1.0f,  0.0f, // 5 (0, 0) Bottom Left
            1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,      texIndex,      -1.0f, 1.0f,  0.0f, // 6 (0, 1) Top Left
            0.0f, 0.5f, 1.1f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,      -1.0f, 1.0f,  0.0f, // 9 (1, 1) Top Right
            // Bottom Left
            0.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,      texIndex,       1.0f, 1.0f,  0.0f, // 5 (0, 1) Top Right
           -1.0f, 0.5f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,      texIndex,       1.0f, 1.0f,  0.0f, // 8 (1, 0) Bottom Right
            0.0f, 0.5f, 1.1f,     1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,      texIndex,       1.0f, 1.0f,  0.0f, // 9 (0, 0) Bottom Left
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateStaff(GLMesh& mesh)
{
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (6 * NUM_SIDES);
    GLfloat verts[NUM_VERTICES];
    UCreateCylinder(verts, NUM_SIDES, 0.3f, 1.0f, 9.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateStaffOrb(GLMesh& mesh)
{
    const int gradation = 30;
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (6 * NUM_SIDES * gradation);
    GLfloat verts[NUM_VERTICES];
    UCreateSphere(verts, 5.0f, gradation, 8.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UCreateStaffSupport(GLMesh& mesh)
{
    const int NUM_SIDES = 100;
    const int NUM_VERTICES = STRIDE * (6 * NUM_SIDES);
    GLfloat verts[NUM_VERTICES];
    UCreateCylinder(verts, NUM_SIDES, 0.3f, 1.0f, 4.0f);

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;
    const GLuint floatsPerTexture = 2;
    const GLuint floatsPerTextureIndex = 1;
    const GLuint floatsPerNormal = 3;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal));

    // Strides between vertex coordinates is 10 (x, y, z, r, g, b, a, tx, ty, ti). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex + floatsPerNormal);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerTexture, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerTextureIndex, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerTexture + floatsPerTextureIndex)));
    glEnableVertexAttribArray(4);
}

void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, &mesh.vbo);
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
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
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

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

