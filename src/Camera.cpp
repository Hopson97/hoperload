#include "Camera.h"
#include "Chunks/Chunk.h"
#include "Utility.h"

namespace
{
    int worldHeight = 14;
    int worldWidth = 4;
} // namespace

Camera::Camera()
{
    float aspect = (float)WIDTH / (float)HEIGHT;

    m_projectionMatrix = createProjectionMatrix(aspect, 110.0f);
    m_transform = {{50, worldHeight * CHUNK_SIZE - CHUNK_SIZE / 2, 10}, {0, 270, 0}};
}

void Camera::update()
{
    assert(mp_hook);
    auto viewMatrix = createViewMartix(m_transform, {0, 1, 0});
    m_projectionViewMatrix = m_projectionMatrix * viewMatrix;
    m_frustum.update(m_projectionViewMatrix);

    m_transform.position = {mp_hook->position.x, mp_hook->position.y, m_zoom};
    m_transform.rotation = {0, 270, 0};
}

void Camera::zoomIn()
{
    m_zoom++;
}

void Camera::zoomOut()
{
    m_zoom--;
}

void Camera::hookTransform(const Transform* hook)
{
    mp_hook = hook;
}

const glm::mat4& Camera::getProjectionView() const
{
    return m_projectionViewMatrix;
}
