#ifndef camera_hpp
#define camera_hpp

#include <glm/glm.hpp>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    float speed = 5.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float sensitivity = 0.1f;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 front = glm::vec3(
                                cos(glm::radians(yaw)),
                                sin(glm::radians(pitch)),
                                sin(glm::radians(yaw))
                                );
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right;
    
    
    Camera(glm::vec3 iPosition) {
        position = iPosition;
    }
    
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }
    
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = speed * deltaTime;
        if (direction == FORWARD) {
                position += front * velocity;
        }
        if (direction == BACKWARD) {
                position -= front * velocity;
        }
        if (direction == LEFT) {
                position -= glm::normalize(glm::cross(front, up)) * velocity;
        }
        if (direction == RIGHT) {
                position += glm::normalize(glm::cross(front, up)) * velocity;
        }
        if (direction == UP) {
                position -= up * velocity;
        }
        if (direction == DOWN) {
                position += up * velocity;
        }
    }
    void updateRotation(float xd, float yd) {
        xd *= sensitivity;
        yd *= sensitivity;
        yaw += xd;
        pitch += yd;
        if(pitch > 89.0f) {
          pitch =  89.0f;
        }
        if(pitch < -89.0f) {
          pitch = -89.0f;
        }
        front = glm::normalize(glm::vec3(
                          cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                          sin(glm::radians(pitch)),
                          sin(glm::radians(yaw)) * cos(glm::radians(pitch))
                          ));
    }
};

#endif
