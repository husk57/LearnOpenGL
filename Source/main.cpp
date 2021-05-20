#include <iostream>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.hpp>
#include <texture.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <camera.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int windowWidth = 800, windowHeight = 600;
bool firstMouse = true;
bool camControlEnabled = false;
float previousMouseX = windowWidth/2, previousMouseY = windowHeight/2;
float deltaTime = 0.0f, lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
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

void callResizeEvent(GLFWwindow* window, int width, int height) {
    glViewport(0,0, width, height);
    windowWidth = width;
    windowHeight = height;
}

int main() {
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
     
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, callResizeEvent);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
    
    float vertices[] = {
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
                 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
                 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
                 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
                -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f,

                -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
                 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
                 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
                 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
                -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

                -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
                -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
                -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

                 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
                 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
                 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
                 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
                 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
                 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
                 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
                 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
                 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
                -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,

                -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
                 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
                 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
                 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
    };
    
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    
    //Vertex Block
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    //Texture Block
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //Normal Block
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    Shader mainShader("./Source/vertex.vert", "./Source/fragment.frag");
    Texture diffuseMapTexture("./Textures/containerDiffMap.png");
    Texture specularMapTexture("./Textures/containerSpecMap.png");
    Texture emissionMapTexture("./Textures/matrix.jpeg");
    mainShader.use();
    mainShader.setUniformInt("material.diffuse", 0);
    mainShader.setUniformInt("material.specular", 1);
    mainShader.setUniformInt("material.emission", 2);
    mainShader.setUniformFloat("material.shininess", 64.0f);
    
    ImVec4 light_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    float linearAtt = 0.09f;
    float quadraticAtt = 0.032f;
    float xL = -0.374f;
    float yL = -0.52f;
    float zL = -1.0f;
    float cutOff = 12.5f;
    float outerCutOff = 17.5;
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Text("Camera Position: %f, %f, %f", camera.position.x, camera.position.y, camera.position.z);
        ImGui::Text("Camera Look Vector: %f, %f, %f", camera.front.x, camera.front.y, camera.front.z);
        ImGui::Text("FPS: %d", (int)(1.0/deltaTime));
        ImGui::ColorEdit3("Light Color", (float*)&light_color);
        ImGui::SliderFloat("Linear Attenuation", &linearAtt, 0.0f, 0.1f);
        ImGui::SliderFloat("Quadratic Attenuation", &quadraticAtt, 0.0f, 0.1f);
        ImGui::SliderFloat("Cut off", &cutOff, 0.0f, 180.0f);
        ImGui::SliderFloat("Outer off", &outerCutOff, 0.0f, 180.0f);
        ImGui::SliderFloat("Light X Dir", &xL, -1.0f, 1.0f);
        ImGui::SliderFloat("Light Y Dir", &yL, -1.0f, 1.0f);
        ImGui::SliderFloat("Light Z Dir", &zL, -1.0f, 1.0f);
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 viewMatrix = glm::mat4(1.0f);
        viewMatrix = camera.GetViewMatrix();
        glm::mat4 perspectiveMatrix = glm::mat4(1.0f);
        perspectiveMatrix = glm::perspective(glm::radians(45.0f), (float)(windowWidth)/(float)(windowHeight), 0.1f, 100.0f);
        
        mainShader.use();
        mainShader.setUniformVec3("cameraPosition", camera.position);
        
        mainShader.setUniformVec3("dirLight.ambient", glm::vec3(0.2));
        mainShader.setUniformVec3("dirLight.diffuse",  glm::vec3(1.0));
        mainShader.setUniformVec3("dirLight.specular", glm::vec3(1.0));
        mainShader.setUniformVec3("dirLight.direction", glm::vec3(xL, yL, zL));
        
        for (int pointLight = 0; pointLight<4; pointLight++) {
            mainShader.setUniformVec3("pointLights[" + std::to_string(pointLight) + "].position", pointLightPositions[pointLight]);
            mainShader.setUniformFloat("pointLights[" + std::to_string(pointLight) + "].constant", 1.0f);
            mainShader.setUniformFloat("pointLights[" + std::to_string(pointLight) + "].linear", linearAtt);
            mainShader.setUniformFloat("pointLights[" + std::to_string(pointLight) + "].quadratic", quadraticAtt);
            mainShader.setUniformVec3("pointLights[" + std::to_string(pointLight) + "].ambient", glm::vec3(0.2));
            mainShader.setUniformVec3("pointLights[" + std::to_string(pointLight) + "].diffuse", glm::vec3(1.0));
            mainShader.setUniformVec3("pointLights[" + std::to_string(pointLight) + "].specular", glm::vec3(1.0));
        }
        
        mainShader.setUniformVec3("spotLight.position", camera.position);
        mainShader.setUniformVec3("spotLight.direction", camera.front);
        mainShader.setUniformVec3("spotLight.ambient", glm::vec3(1.0));
        mainShader.setUniformVec3("spotLight.diffuse", glm::vec3(1.0));
        mainShader.setUniformVec3("spotLight.specular", glm::vec3(1.0));
        mainShader.setUniformFloat("spotLight.constant", 1.0f);
        mainShader.setUniformFloat("spotLight.linear", linearAtt);
        mainShader.setUniformFloat("spotLight.quadratic", quadraticAtt);
        mainShader.setUniformFloat("spotLight.cutOff", cutOff);
        mainShader.setUniformFloat("spotLight.outerCutOff", outerCutOff);
        
        mainShader.setUniformFloat("time", (float)glfwGetTime());
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMapTexture.texture);
         
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMapTexture.texture);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMapTexture.texture);
        
        mainShader.setUniformMat4("viewMatrix", glm::value_ptr(viewMatrix));
        mainShader.setUniformMat4("perspectiveMatrix", glm::value_ptr(perspectiveMatrix));
        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
            modelMatrix = glm::rotate(modelMatrix, 20.0f*(float)(i), glm::vec3(1.0f, 0.3f, 0.5f));
            
            mainShader.setUniformMat4("modelMatrix", glm::value_ptr(modelMatrix));
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
                
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
