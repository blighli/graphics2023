#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>
#include <model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // earth model and shader
    // -------------------------
    Model earthModel("src/homework2/objects/earth/Earth 2K.obj");
    Shader EarthShader("src/homework2/shaders/earth.vs", "src/homework2/shaders/earth.fs");
    vector<float> earth_bdbox = earthModel.BoundingBox();
    float earth_scale = sqrt(earth_bdbox[1]);
    // texture
    unsigned int earth_texture = loadTexture("src/homework2/objects/earth/Clouds_2K.png");
    EarthShader.use();
    EarthShader.setInt("earth_texture", 1);
    
    // sun model and shader
    // -----------
    Shader SunShader("src/homework2/shaders/sun.vs", "src/homework2/shaders/sun.fs");
    Model sunModel("src/homework2/objects/sol/sol.obj");
    vector<float> sun_bdbox = sunModel.BoundingBox();
    float sun_scale = sqrt(sun_bdbox[1]);
    
    // texture
    unsigned int sun_texture = loadTexture("src/homework2/objects/sol/2k_sun.jpg");

    SunShader.use();
    SunShader.setInt("sun_texture", 0);

    // mar model and shader
    Model marModel("src/homework2/objects/mar/Mars 2K.obj");
    Shader MarShader("src/homework2/shaders/mar.vs", "src/homework2/shaders/mar.fs");
    vector<float> mar_bdbox = marModel.BoundingBox();
    float mar_scale = sqrt(mar_bdbox[1]);

    // mercury model and shader
    Model mercuryModel("src/homework2/objects/mercury/Mercury 1K.obj");
    vector<float> mercury_bdbox = mercuryModel.BoundingBox();
    float mercury_scale = sqrt(mercury_bdbox[1]);

    // venus model and shader
    Model venusModel("src/homework2/objects/venus/Venus_1K.obj");
    vector<float> venus_bdbox = venusModel.BoundingBox();
    float venus_scale = sqrt(venus_bdbox[1]);

    // jupiter model and shader
    Model jupiterModel("src/homework2/objects/jupiter/13905_Jupiter_V1_l3.obj");
    vector<float> jupiter_bdbox = jupiterModel.BoundingBox();
    float jupiter_scale = sqrt(jupiter_bdbox[1]);

    // saturno model and shader
    Model saturnoModel("src/homework2/objects/saturno/13906_Saturn_v1_l3.obj");
    vector<float> saturno_bdbox = saturnoModel.BoundingBox();
    float saturno_scale = sqrt(saturno_bdbox[1]);

    // urano model and shader
    Shader UranoShader("src/homework2/shaders/urano.vs", "src/homework2/shaders/urano.fs");
    Model uranoModel("src/homework2/objects/urano/urano.obj");
    vector<float> urano_bdbox = uranoModel.BoundingBox();
    float urano_scale = sqrt(urano_bdbox[1]);
    // texture
    unsigned int urano_texture = loadTexture("src/homework2/objects/urano/2k_uranus.jpg");
    UranoShader.use();
    UranoShader.setInt("urano_texture", 2);

    // neptuno model and shader
    Shader NeptunoShader("src/homework2/shaders/neptuno.vs", "src/homework2/shaders/neptuno.fs");
    Model neptunoModel("src/homework2/objects/neptuno/neptuno.obj");
    vector<float> neptuno_bdbox = neptunoModel.BoundingBox();
    float neptuno_scale = sqrt(neptuno_bdbox[1]);
    // texture
    unsigned int neptuno_texture = loadTexture("src/homework2/objects/neptuno/2k_neptune.jpg");
    NeptunoShader.use();
    NeptunoShader.setInt("neptuno_texture", 3);

    // background
    Shader BackgroundShader("src/homework2/shaders/background.vs", "src/homework2/shaders/background.fs");
    Model backgroundModel("src/homework2/objects/background/background.obj");
    vector<float> background_bdbox = backgroundModel.BoundingBox();
    float background_scale = sqrt(background_bdbox[1]);
    // texture
    unsigned int background_texture = loadTexture("src/homework2/objects/background/StarsMap_2500x1250.jpg");
    BackgroundShader.use();
    BackgroundShader.setInt("background_texture", 4);

    glm::vec3 lightPos(0.0f,  0.0f,  0.0f);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // earth
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, earth_texture);

        // don't forget to enable shader before setting uniforms
        EarthShader.use();
        EarthShader.setVec3("light.position", lightPos);
        EarthShader.setVec3("viewPos", camera.Position);

        // light properties
        EarthShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        EarthShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        EarthShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        EarthShader.setFloat("light.constant", 1.0f);
        EarthShader.setFloat("light.linear", 0.0035f);
        EarthShader.setFloat("light.quadratic", 0.0001f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        glm::mat4 view = camera.GetViewMatrix();
        EarthShader.setMat4("projection", projection);
        EarthShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        // 公转
        float revolution_angle = (float)glfwGetTime() * glm::radians(1.5f); // 转换角度为弧度
        model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle), 0.0f, 10.0f * glm::sin(-revolution_angle))); 
        // 自转
        float rotation_angle = revolution_angle * 365.0 * 0.1; // 转换角度为弧度
        glm::vec3 rotation_axis(0.0f, glm::cos(glm::radians(22.3f)), glm::sin(glm::radians(22.3f)));
        model = glm::rotate(model, rotation_angle, rotation_axis);
        model = glm::scale(model, glm::vec3(1.0f/earth_scale));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        EarthShader.setMat4("model", model);
        earthModel.Draw(EarthShader);

        // sun
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sun_texture);

        SunShader.use();
        // view/projection transformations
        SunShader.setMat4("projection", projection);
        SunShader.setMat4("view", view);

        // render the loaded model
        model = glm::mat4(1.0f);
        // 自转
        float rotation_angle_ = rotation_angle / 25.0; // 转换角度为弧度
        rotation_axis = glm::vec3(0.0f, glm::cos(glm::radians(7.25f)), glm::sin(glm::radians(7.25f)));
        model = glm::rotate(model, rotation_angle_, rotation_axis);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f/sun_scale*2.5));	// it's a bit too big for our scene, so scale it down
        SunShader.setMat4("model", model);
        sunModel.Draw(SunShader);

        // mar 
        MarShader.use();
        MarShader.setVec3("light.position", lightPos);
        MarShader.setVec3("viewPos", camera.Position);

        // light properties
        MarShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        MarShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        MarShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        MarShader.setFloat("light.constant", 1.0f);
        EarthShader.setFloat("light.linear", 0.0035f);
        EarthShader.setFloat("light.quadratic", 0.0001f);

        // view/projection transformations
        MarShader.setMat4("projection", projection);
        MarShader.setMat4("view", view);

        // render the loaded model
        model = glm::mat4(1.0f);
        // 公转
        // revolution_angle = (float)glfwGetTime() * glm::radians(1.5f); // 转换角度为弧度
        model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 1.88 - glm::radians(150.0f)) * 1.52, 0.0f, 10.0f * glm::sin(-revolution_angle / 1.88- glm::radians(150.0f)) * 1.52)); 
        // 自转
        // rotation_angle = revolution_angle * 365.0 * 0.1; // 转换角度为弧度
        rotation_angle_ = rotation_angle / 1.03;
        rotation_axis = glm::vec3(0.0f, glm::cos(glm::radians(25.2f)), glm::sin(glm::radians(25.2f)));
        model = glm::rotate(model, rotation_angle_, rotation_axis);
        model = glm::scale(model, glm::vec3(1.0f/mar_scale*0.53));	// it's a bit too big for our scene, so scale it down
        MarShader.setMat4("model", model);
        marModel.Draw(MarShader);

        // mercury
        MarShader.use();
        // render the loaded model
        model = glm::mat4(1.0f);
        // 公转
        model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 0.24 - glm::radians(60.0f)) * 0.39, 0.0f, 10.0f * glm::sin(-revolution_angle / 0.24 - glm::radians(60.0f)) * 0.39)); 
        // 自转
        rotation_angle_ = rotation_angle / 58.6;
        rotation_axis = glm::vec3(0.0f, glm::cos(glm::radians(0.034f)), glm::sin(glm::radians(0.034f)));
        model = glm::rotate(model, rotation_angle_, rotation_axis);
        model = glm::scale(model, glm::vec3(1.0f/mercury_scale*0.38));	// it's a bit too big for our scene, so scale it down
        MarShader.setMat4("model", model);
        mercuryModel.Draw(MarShader);

        // venus
        MarShader.use();
        // render the loaded model
        model = glm::mat4(1.0f);
        // 公转
        // revolution_angle = (float)glfwGetTime() * glm::radians(1.5f); // 转换角度为弧度
        model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 0.62 - glm::radians(210.0f)) * 0.72, 0.0f, 10.0f * glm::sin(-revolution_angle / 0.62 - glm::radians(210.0f)) * 0.72)); 
        // 自转
        rotation_angle_ = rotation_angle / 243;
        rotation_axis = glm::vec3(0.0f, glm::cos(glm::radians(177.0f)), glm::sin(glm::radians(177.0f)));
        model = glm::rotate(model, rotation_angle_, rotation_axis);
        model = glm::scale(model, glm::vec3(1.0f/venus_scale*0.95));	// it's a bit too big for our scene, so scale it down
        MarShader.setMat4("model", model);
        venusModel.Draw(MarShader);

        // jupiter
        MarShader.use();
        // render the loaded model
        model = glm::mat4(1.0f);
        // 公转
        // revolution_angle = (float)glfwGetTime() * glm::radians(1.5f); // 转换角度为弧度
        // model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 11.86) * 5.20, 0.0f, 10.0f * glm::sin(-revolution_angle / 11.86) * 5.20)); 
        model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 11.86 - glm::radians(300.0f)) * 2.60, 0.0f, 10.0f * glm::sin(-revolution_angle / 11.86 - glm::radians(300.0f)) * 2.60));
        // 自转
        rotation_angle_ = rotation_angle / 0.41;
        rotation_axis = glm::vec3(0.0f, glm::cos(glm::radians(3.1f)), glm::sin(glm::radians(3.1f)));
        model = glm::rotate(model, rotation_angle_, rotation_axis);
        model = glm::scale(model, glm::vec3(1.0f/jupiter_scale*10.97/3.5));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        MarShader.setMat4("model", model);
        jupiterModel.Draw(MarShader);

        // saturno
        MarShader.use();
        // render the loaded model
        model = glm::mat4(1.0f);
        // 公转
        // revolution_angle = (float)glfwGetTime() * glm::radians(1.5f); // 转换角度为弧度
        // model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 29.46) * 9.58, 0.0f, 10.0f * glm::sin(-revolution_angle / 29.46) * 9.58)); 
        model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 29.46 - glm::radians(90.0f)) * 3.50, 0.0f, 10.0f * glm::sin(-revolution_angle / 29.46 - glm::radians(90.0f)) * 3.50));
        // 自转
        rotation_angle_ = rotation_angle / 0.45;
        rotation_axis = glm::vec3(0.0f, glm::cos(glm::radians(26.7f)), glm::sin(glm::radians(26.7f)));
        // model = glm::rotate(model, rotation_angle_, rotation_axis);
        model = glm::scale(model, glm::vec3(1.0f/saturno_scale*9.14/1.8));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        MarShader.setMat4("model", model);
        saturnoModel.Draw(MarShader);

        // urano
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, urano_texture);

        UranoShader.use();
        UranoShader.setVec3("light.position", lightPos);
        UranoShader.setVec3("viewPos", camera.Position);

        // light properties
        UranoShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        UranoShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        UranoShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        UranoShader.setFloat("light.constant", 1.0f);
        EarthShader.setFloat("light.linear", 0.0035f);
        EarthShader.setFloat("light.quadratic", 0.0001f);
        
        // view/projection transformations
        UranoShader.setMat4("projection", projection);
        UranoShader.setMat4("view", view);

        // render the loaded model
        model = glm::mat4(1.0f);
        // 公转
        // revolution_angle = (float)glfwGetTime() * glm::radians(1.5f); // 转换角度为弧度
        // model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 84.01) * 19.22, 0.0f, 10.0f * glm::sin(-revolution_angle / 84.01) * 19.22)); 
        model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 84.01 - glm::radians(240.0f)) * 4.2, 0.0f, 10.0f * glm::sin(-revolution_angle / 84.01 - glm::radians(240.0f)) * 4.2)); // 19.22 太远了
        // 自转
        rotation_angle_ = rotation_angle / 0.72;
        rotation_axis = glm::vec3(0.0f, glm::cos(glm::radians(97.8f)), glm::sin(glm::radians(97.8f)));
        model = glm::rotate(model, rotation_angle_, rotation_axis);
        model = glm::scale(model, glm::vec3(1.0f/urano_scale*3.98/2.2));	// it's a bit too big for our scene, so scale it down
        UranoShader.setMat4("model", model);
        uranoModel.Draw(UranoShader);

        // neptuno
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, neptuno_texture);

        NeptunoShader.use();
        NeptunoShader.setVec3("light.position", lightPos);
        NeptunoShader.setVec3("viewPos", camera.Position);

        // light properties
        NeptunoShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        NeptunoShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        NeptunoShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        NeptunoShader.setFloat("light.constant", 1.0f);
        EarthShader.setFloat("light.linear", 0.0035f);
        EarthShader.setFloat("light.quadratic", 0.0001f);
        
        // view/projection transformations
        NeptunoShader.setMat4("projection", projection);
        NeptunoShader.setMat4("view", view);

        // render the loaded model
        model = glm::mat4(1.0f);
        // 公转
        // revolution_angle = (float)glfwGetTime() * glm::radians(1.5f); // 转换角度为弧度
        // model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 164.8) * 30.05, 0.0f, 10.0f * glm::sin(-revolution_angle / 164.8) * 30.05)); 
        model = glm::translate(model, glm::vec3(10.0f * glm::cos(-revolution_angle / 164.8 - glm::radians(30.0f)) * 4.8, 0.0f, 10.0f * glm::sin(-revolution_angle / 164.8 - glm::radians(30.0f)) * 4.8)); // 30.05太远了
        // 自转
        rotation_angle_ = rotation_angle / 0.67;
        rotation_axis = glm::vec3(0.0f, glm::cos(glm::radians(28.3f)), glm::sin(glm::radians(28.3f)));
        model = glm::rotate(model, rotation_angle_, rotation_axis);
        model = glm::scale(model, glm::vec3(1.0f/neptuno_scale*3.86/2.2));	// it's a bit too big for our scene, so scale it down
        NeptunoShader.setMat4("model", model);
        neptunoModel.Draw(NeptunoShader);

        // background
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, background_texture);

        // render the loaded model
        BackgroundShader.use();
        // view/projection transformations
        BackgroundShader.setMat4("projection", projection);
        BackgroundShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f/background_scale*150.0f));	// it's a bit too big for our scene, so scale it down
        BackgroundShader.setMat4("model", model);
        backgroundModel.Draw(BackgroundShader);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
