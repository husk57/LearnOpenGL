#include <iostream>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <camera.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <model.hpp>
#include <stb_image.h>
#include <mesh.hpp>

int windowWidth = 800, windowHeight = 600;
bool firstMouse = true;
bool camControlEnabled = false;
float previousMouseX = windowWidth/2, previousMouseY = windowHeight/2;
float deltaTime = 0.0f, lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
unsigned int framebuffer, renderbuffer, textureColorbuffer;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
}

void mouse_pos_callback(GLFWwindow* window, double x, double y) {
    if (camControlEnabled == true) {
    if (firstMouse) {
        previousMouseX = x;
        previousMouseY = y;
        firstMouse = false;
    }
            
    float xd = x - previousMouseX;
    float yd = previousMouseY - y; // reversed since y-coordinates range from bottom to top
    previousMouseX = x;
    previousMouseY = y;
    camera.updateRotation(xd, yd);
    }
}

void mouse_button_callback(GLFWwindow* window,int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        camControlEnabled = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        camControlEnabled = false;
        firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void callResizeEvent(GLFWwindow* window, int width, int height);
unsigned int loadCubemap(std::vector<std::string> faces);

int main() {
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
     
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    #endif
    
    
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, callResizeEvent);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    
    //Shader definitions
    Shader mainShader("./Source/vertex.vert", "./Source/fragment.frag");
    Shader postProcessQuad("./Source/postprocess.vert", "./Source/postprocess.frag");
    Shader skyboxShader("./Source/skybox.vert", "./Source/skybox.frag");
    
    //imgui variables
    float linearAtt = 0.09f;
    float quadraticAtt = 0.032f;
    float cutOff = 12.5f;
    float outerCutOff = 13.5f;
    float sunColor[] = {1.0, 1.0, 1.0};
    
    //post process framebufffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
    // screen quad VAO
        unsigned int quadVAO, quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    postProcessQuad.use();
    postProcessQuad.setUniformInt("screenTexture", 0);
    
    //Version Debug
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    //Skybox
    
    float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    std::vector<std::string> faces {
        "./Decals/skybox/right.jpg",
        "./Decals/skybox/left.jpg",
        "./Decals/skybox/top.jpg",
        "./Decals/skybox/bottom.jpg",
        "./Decals/skybox/front.jpg",
        "./Decals/skybox/back.jpg"
    };
    
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setUniformInt("skybox", 6);
    mainShader.use();
    mainShader.setUniformInt("skybox", 6);
    
    //model buffer loaders
    Model character("./Meshes/CoderHusk/robloxOriginal.obj", false, cubemapTexture);
    Model backpack("./Meshes/backpack/backpack.obj", true, cubemapTexture);
    Model bunny("./Meshes/stanford-bunny-obj/stanford-bunny.obj", true, cubemapTexture);
    Model plane("./Meshes/Plane/plane.obj", true, cubemapTexture);
    Model tree("./Meshes/Tree/tree.obj", false, cubemapTexture);
    Model sphere("./Meshes/Sphere/sphere.obj", true, cubemapTexture);
    
    //Render Loop
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &windowWidth,&windowHeight);
        glViewport(0,0,windowWidth, windowHeight);
        
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Text("Camera Position: %f, %f, %f", camera.position.x, camera.position.y, camera.position.z);
        ImGui::Text("Camera Look Vector: %f, %f, %f", camera.front.x, camera.front.y, camera.front.z);
        ImGui::Text("FPS: %d", (int)(1.0/deltaTime));
        ImGui::SliderFloat("Linear Attenuation", &linearAtt, 0.0f, 0.1f);
        ImGui::SliderFloat("Quadratic Attenuation", &quadraticAtt, 0.0f, 0.1f);
        ImGui::SliderFloat("Cut off", &cutOff, 0.0f, 180.0f);
        ImGui::SliderFloat("Outer off", &outerCutOff, 0.0f, 180.0f);
        ImGui::ColorEdit3("Sun Color", sunColor);
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        glm::mat4 viewMatrix = glm::mat4(1.0f);
        glm::mat4 perspectiveMatrix = glm::mat4(1.0f);
        perspectiveMatrix = glm::perspective(glm::radians(45.0f), (float)(windowWidth)/(float)(windowHeight), 0.1f, 100.0f);
        
        glDepthMask(GL_FALSE);
        skyboxShader.use();
        viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setUniformMat4("viewMatrix", glm::value_ptr(viewMatrix));
        skyboxShader.setUniformMat4("perspectiveMatrix", glm::value_ptr(perspectiveMatrix));
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        
        mainShader.use();
        viewMatrix = glm::mat4(1.0f);
        viewMatrix = camera.GetViewMatrix();
        mainShader.setUniformVec3("direction_light", -glm::vec3(-.56, -.54, .62));
        mainShader.setUniformVec4("material.diffuse", glm::vec4(1.0));
        mainShader.setUniformVec4("material.specular", glm::vec4(1.0));
        mainShader.setUniformFloat("material.shininess", 32.0f);
        mainShader.setUniformMat4("viewMatrix", glm::value_ptr(viewMatrix));
        mainShader.setUniformMat4("perspectiveMatrix", glm::value_ptr(perspectiveMatrix));
        mainShader.setUniformVec3("sunColor", glm::vec3(sunColor[0], sunColor[1], sunColor[2]));
        
        mainShader.setUniformVec3("cameraPosition", camera.position);
        
        for (int pointLight = 0; pointLight<4; pointLight++) {
            mainShader.setUniformVec3("pointLights[" + std::to_string(pointLight) + "].position", pointLightPositions[pointLight]);
            mainShader.setUniformFloat("pointLights[" + std::to_string(pointLight) + "].linear", linearAtt);
            mainShader.setUniformFloat("pointLights[" + std::to_string(pointLight) + "].quadratic", quadraticAtt);
        }
        
        mainShader.setUniformVec3("spotLight.position", camera.position);
        mainShader.setUniformVec3("spotLight.direction", camera.front);
        mainShader.setUniformFloat("spotLight.linear", linearAtt);
        mainShader.setUniformFloat("spotLight.quadratic", quadraticAtt);
        mainShader.setUniformFloat("spotLight.cutOff", glm::cos(glm::radians(cutOff)));
        mainShader.setUniformFloat("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOff)));
        
        mainShader.setUniformFloat("time", (float)glfwGetTime());
        
        mainShader.setUniformFloat("material.reflectiveness", 0.0);
        mainShader.setUniformFloat("material.refractiveness", 0.0);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        mainShader.setUniformMat4("modelMatrix", glm::value_ptr(model));
        character.Draw(mainShader);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        mainShader.setUniformMat4("modelMatrix", glm::value_ptr(model));
        backpack.Draw(mainShader);
        
        mainShader.setUniformFloat("material.reflectiveness", 1.0);
        mainShader.setUniformFloat("material.refractiveness", 1.3);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
        model = glm::scale(model, glm::vec3(20.0f));
        mainShader.setUniformMat4("modelMatrix", glm::value_ptr(model));
        bunny.Draw(mainShader);
        
        mainShader.setUniformFloat("material.refractiveness", 0.0);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-7.0f, -1.0f, 7.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        mainShader.setUniformMat4("modelMatrix", glm::value_ptr(model));
        sphere.Draw(mainShader);
        
        mainShader.setUniformFloat("material.reflectiveness", 0.0);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(20.0f));
        mainShader.setUniformMat4("modelMatrix", glm::value_ptr(model));
        plane.Draw(mainShader);
        
        glDisable(GL_CULL_FACE);
        mainShader.setUniformVec4("material.specular", glm::vec4(0.0));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(cos(glm::radians(210.0f))*6.0f, 2.0f, sin(glm::radians(210.0f))*6.0f));
        model = glm::scale(model, glm::vec3(4.0f));
        mainShader.setUniformMat4("modelMatrix", glm::value_ptr(model));
        tree.Draw(mainShader);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(cos(glm::radians(30.0f))*6.0f, 2.0f, sin(glm::radians(30.0f))*6.0f));
        model = glm::scale(model, glm::vec3(4.0f));
        mainShader.setUniformMat4("modelMatrix", glm::value_ptr(model));
        tree.Draw(mainShader);
        glEnable(GL_CULL_FACE);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        postProcessQuad.use();
        postProcessQuad.setUniformVec2("resolution", glm::vec2((float)windowWidth, (float)windowHeight));
        postProcessQuad.setUniformFloat("time", glfwGetTime());
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}


void callResizeEvent(GLFWwindow* window, int width, int height) {
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
