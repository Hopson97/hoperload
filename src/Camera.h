#pragma once

#include "Maths.h"
#include <SFML/Window/Window.hpp>

class Keyboard;

struct Camera
{
    Camera();

    void inputFreeCamera(const Keyboard& keyboard, const sf::Window& window);

    void update(bool followHook);

    void zoomIn();
    void zoomOut();
    void hookTransform(const Transform* hook);
    const glm::mat4& getProjectionView() const;

    const Transform& getTransform() const;

  private:
    const Transform* mp_hook = nullptr;
    Transform m_transform;
    float m_zoom = 9;

    ViewFrustum m_frustum;

    glm::mat4 m_projectionMatrix;
    glm::mat4 m_projectionViewMatrix;
};
