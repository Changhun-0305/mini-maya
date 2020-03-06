#include "camera.h"

#include <la.h>
#include <iostream>


Camera::Camera():
    Camera(400, 400)
{
    forward = glm::vec3(0,0,-1);
    up = glm::vec3(0,1,0);
    right = glm::vec3(1,0,0);
    theta = 0.0;
    phi = 0.0;
    zoom = 10.0;
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0))
{}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp),
    theta(0.0),
    phi(0.0),
    zoom(10.0)
{
    RecomputeAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    forward(c.forward),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H),
    theta(0.0),
    phi(0.0),
    zoom(10.0)
{}

void Camera::RotateAxis() {
    glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(0, 1, 0)) *
               glm::rotate(glm::mat4(1.0f), glm::radians(phi), glm::vec3(1, 0, 0)) *
               glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, zoom));
    eye = glm::vec3(transform * glm::vec4(0, 0, 0, 1));
    forward = glm::vec3(transform * glm::vec4(0, 0, 1, 0));
    right = glm::vec3(transform * glm::vec4(1, 0, 0, 0));
    up = glm::vec3(transform * glm::vec4(0, 1, 0, 0));
}

void Camera::RecomputeAttributes()
{
    RotateAxis();
    float tan_fovy = tan(glm::radians(fovy/2));
    float len = glm::length(ref - eye);
    aspect = width / static_cast<float>(height);
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
}

glm::mat4 Camera::getViewProj()
{
    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
    theta += deg;
    RecomputeAttributes();
}
void Camera::RotateAboutRight(float deg)
{
    phi += deg;
    RecomputeAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
    zoom += amt;
    RecomputeAttributes();
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;
    RecomputeAttributes();
}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
    RecomputeAttributes();
}

//#include "camera.h"

//#include <la.h>
//#include <iostream>


//Camera::Camera():
//    Camera(400, 400)
//{
//    forward = glm::vec4(0,0,1,0);
//    up = glm::vec4(0,1,0,0);
//    right = glm::vec4(1,0,0,0);
//}

//Camera::Camera(unsigned int w, unsigned int h):
//    Camera(w, h, glm::vec4(0,0,10,1), glm::vec4(0,0,0,1), glm::vec4(0,1,0,0))
//{}

//Camera::Camera(unsigned int w, unsigned int h, const glm::vec4 &e, const glm::vec4 &r, const glm::vec4 &worldUp):
//    fovy(45),
//    width(w),
//    height(h),
//    near_clip(0.1f),
//    far_clip(1000),
//    eye(e),
//    ref(r),
//    theta(0),
//    phi(0),
//    zoom(0),
//    world_up(worldUp)
//{
//    RecomputeAttributes();
//}

//Camera::Camera(const Camera &c):
//    fovy(c.fovy),
//    width(c.width),
//    height(c.height),
//    near_clip(c.near_clip),
//    far_clip(c.far_clip),
//    aspect(c.aspect),
//    eye(c.eye),
//    ref(c.ref),
//    forward(c.forward),
//    up(c.up),
//    right(c.right),
//    world_up(c.world_up),
//    V(c.V),
//    H(c.H)
//{}

//void Camera::RotateAxis() {
//    glm::mat4 transform = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0, 1, 0)) *
//               glm::rotate(glm::mat4(1.0f), phi, glm::vec3(1, 0, 0)) *
//               glm::translate(glm::mat4(1.0f), zoom * glm::vec3(0, 0, 1));
//    eye = glm::vec4(0, 0, 0, 1) * transform + glm::vec4(0, 0, 10, 0);
//    forward = glm::vec4(0, 0, 0, 1) * transform;
//    right = glm::vec4(1, 0, 0, 0) * transform;
//    up = glm::vec4(0, 1, 0, 0) * transform;
//}

//void Camera::RecomputeAttributes()
//{

////    forward = glm::normalize(ref - eye);
////    right = glm::vec4(glm::normalize(glm::cross(glm::vec3(forward), glm::vec3(world_up))), 0);
////    up = glm::vec4(glm::cross(glm::vec3(right), glm::vec3(forward)), 0);
//    RotateAxis();
//    float tan_fovy = tan(glm::radians(fovy/2));

//    float len = glm::length(ref - eye);
//    aspect = width / static_cast<float>(height);
//    V = up*len*tan_fovy;
//    H = right*len*aspect*tan_fovy;
//}

//glm::mat4 Camera::getViewProj()
//{
//    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip)
//            * glm::lookAt(glm::vec3(eye), glm::vec3(ref), glm::vec3(up));
//}

//void Camera::RotateAboutUp(float deg)
//{
//    theta += glm::radians(deg);
//    RecomputeAttributes();
//}

//void Camera::RotateAboutRight(float deg)
//{
//    phi += glm::radians(deg);
//    RecomputeAttributes();
//}

//void Camera::TranslateAlongLook(float amt)
//{
//    glm::vec4 translation = forward * amt;
//    eye += translation;
//    zoom += amt;
//    RecomputeAttributes();
//}

//void Camera::TranslateAlongRight(float amt)
//{
//    glm::vec4 translation = right * amt;
//    eye += translation;
//    ref += translation;
//    RecomputeAttributes();
//}
//void Camera::TranslateAlongUp(float amt)
//{
//    glm::vec4 translation = up * amt;
//    eye += translation;
//    ref += translation;
//    RecomputeAttributes();
//}
