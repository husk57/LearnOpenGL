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
    
    
    Shader mainShader("./Source/vertex.vert", "./Source/fragment.frag");
    
    float linearAtt = 0.09f;
    float quadraticAtt = 0.032f;
    float cutOff = 12.5f;
    float outerCutOff = 13.5f;
    
    Model character("./Meshes/CoderHusk/robloxOriginal.obj", false);
    Model backpack("./Meshes/backpack/backpack.obj", true);
    Model bunny("./Meshes/stanford-bunny-obj/stanford-bunny.obj", true);
    Model plane("./Meshes/Plane/plane.obj", true);
    Model tree("./Meshes/Tree/tree.obj", false);
    
    while (!glfwWindowShouldClose(window)) {
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
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mainShader.use();
        mainShader.setUniformVec4("material.diffuse", glm::vec4(1.0));
        mainShader.setUniformVec4("material.specular", glm::vec4(1.0));
        mainShader.setUniformFloat("material.shininess", 32.0f);
        glm::mat4 viewMatrix = glm::mat4(1.0f);
        viewMatrix = camera.GetViewMatrix();
        glm::mat4 perspectiveMatrix = glm::mat4(1.0f);
        perspectiveMatrix = glm::perspective(glm::radians(45.0f), (float)(windowWidth)/(float)(windowHeight), 0.1f, 100.0f);
        mainShader.setUniformMat4("viewMatrix", glm::value_ptr(viewMatrix));
        mainShader.setUniformMat4("perspectiveMatrix", glm::value_ptr(perspectiveMatrix));
        
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
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
        model = glm::scale(model, glm::vec3(20.0f));
        mainShader.setUniformMat4("modelMatrix", glm::value_ptr(model));
        bunny.Draw(mainShader);
        
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
