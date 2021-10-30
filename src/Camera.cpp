#include "Camera.h"
#include "Chunks/Chunk.h"
#include "Utility.h"

namespace
{
} // namespace

Camera::Camera()
{
    float aspect = (float)WIDTH / (float)HEIGHT;

    m_projectionMatrix = createProjectionMatrix(aspect, 90.0f);
    m_transform = {{0, 0, 0}, {0, 270, 0}};
}

void Camera::inputFreeCamera(const Keyboard& keyboard, const sf::Window& window)
{
    float PLAYER_SPEED = 0.25;
    if (keyboard.isKeyDown(sf::Keyboard::LControl))
    {
        PLAYER_SPEED = 1.0f;
    }
    if (keyboard.isKeyDown(sf::Keyboard::W))
    {
        m_transform.position += forwardsVector(m_transform.rotation) * PLAYER_SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::S))
    {
        m_transform.position += backwardsVector(m_transform.rotation) * PLAYER_SPEED;
    }
    if (keyboard.isKeyDown(sf::Keyboard::A))
    {
        m_transform.position += leftVector(m_transform.rotation) * PLAYER_SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::D))
    {
        m_transform.position += rightVector(m_transform.rotation) * PLAYER_SPEED;
    }

    static auto lastMousePosition = sf::Mouse::getPosition(window);
    auto change = sf::Mouse::getPosition(window) - lastMousePosition;
    m_transform.rotation.x -= static_cast<float>(change.y * 0.5);
    m_transform.rotation.y += static_cast<float>(change.x * 0.5);
    sf::Mouse::setPosition({(int)window.getSize().x / 2, (int)window.getSize().y / 2},
                           window);
    lastMousePosition.x = (int)window.getSize().x / 2;
    lastMousePosition.y = (int)window.getSize().y / 2;

    m_transform.rotation.x = glm::clamp(m_transform.rotation.x, -89.9f, 89.9f);
    m_transform.rotation.y = (int)m_transform.rotation.y % 360;
}

void Camera::update(bool followHook)
{
    auto viewMatrix = createViewMartix(m_transform, {0, 1, 0});
    m_projectionViewMatrix = m_projectionMatrix * viewMatrix;
    m_frustum.update(m_projectionViewMatrix);

    if (followHook)
    {
        assert(mp_hook);
        m_transform.position = {mp_hook->position.x, mp_hook->position.y, m_zoom};
        m_transform.rotation = {0, 270, 0};
    }
}

void Camera::zoomIn()
{
    m_zoom--;
}

void Camera::zoomOut()
{
    m_zoom++;
}

void Camera::hookTransform(const Transform* hook)
{
    mp_hook = hook;
}

const glm::mat4& Camera::getProjectionView() const
{
    return m_projectionViewMatrix;
}

const Transform& Camera::getTransform() const
{
    return m_transform;
}
