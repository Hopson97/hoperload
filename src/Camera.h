#pragma once

#include "Maths.h"

struct Camera
{
    Camera();

    void update();

    void zoomIn();
    void zoomOut();
    void hookTransform(const Transform* hook);
    const glm::mat4& getProjectionView() const;

    

  private:
    const Transform* mp_hook = nullptr;
    Transform m_transform;
    float m_zoom = 10;

    ViewFrustum m_frustum;

    glm::mat4 m_projectionMatrix;
    glm::mat4 m_projectionViewMatrix;
};
