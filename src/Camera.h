#pragma once

#include "Maths.h"

struct Camera
{

    glm::mat4 m_projectionMatrix;
    glm::mat4 m_pvMatrix;
    Transform m_transform;
    ViewFrustum m_frustum;

    Camera();
    void update();

    float zoom = 10;
};
