#include "Hoperload.h"

#include "Chunks/Conversions.h"
#include "Chunks/Voxels.h"
#include "GUI.h"
#include "Utility.h"
#include <iostream>

namespace
{
    int worldHeight = 14;
    int worldWidth = 4;
} // namespace

int toIndex(int x, int y)
{
    return x + y * CHUNK_SIZE;
}

Hoperload::Hoperload()
    : m_player(m_world)
{
    m_sceneShader.loadFromFile("SceneVertex.glsl", "SceneFragment.glsl");
    m_playerCube.bufferMesh(createCubeMesh(Player::box));
    m_particleCube.bufferMesh(createCubeMesh({0.1, 0.1, 0.1}));
    m_texture.loadFromFile("OpenGLLogo.png", 8);

    m_camera.hookTransform(&m_player.getTransform());
}

bool freecam = false;
void Hoperload::onEvent(const sf::Event& e)
{
    switch (e.type)
    {
        case sf::Event::KeyReleased:
            if (e.key.code == sf::Keyboard::F)
            {
                freecam = !freecam;
            }
            if (e.key.code == sf::Keyboard::U && !m_player.isDigging())
            {
                auto& position = m_player.getTransform().position;

                m_world.placeBlock(position.x, position.y, VoxelType::TEST_TORCH);
            }
            break;

        case sf::Event::MouseWheelScrolled:
            if (e.mouseWheelScroll.delta > 0)
            {
                m_camera.zoomIn();
            }
            else
            {
                m_camera.zoomOut();
            }
            break;

        default:
            break;
    }
}

void Hoperload::onInput(const Keyboard& keyboard, const sf::Window& window,
                        bool isMouseActive)
{
    if (freecam)
    {
        m_camera.inputFreeCamera(keyboard, window);
    }
    else
    {
        m_player.input(keyboard, window);
    }
}

void Hoperload::onUpdate(const sf::Time& dt)
{
    m_camera.update(!freecam);

    if (!freecam)
    {
        m_player.update(dt, m_particles);

        for (auto itr = m_particles.begin(); itr != m_particles.end();)
        {

            itr->transform.position += itr->direction * dt.asSeconds();
            itr->direction.y -= 3 * dt.asSeconds();

            itr->lifetime -= dt.asSeconds();
            if (itr->lifetime < 0)
            {
                itr = m_particles.erase(itr);
            }
            else
            {
                itr++;
            }
        }
    }
}

void Hoperload::onRender()
{
    glEnable(GL_DEPTH_TEST);

    setClearColour(COLOUR_SKY_BLUE);

    Framebuffer::unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_world.render(m_camera);

    // Normal stuff
    m_sceneShader.bind();
    m_sceneShader.set("projectionViewMatrix", m_camera.getProjectionView());

    glEnable(GL_CULL_FACE);
    auto playerTransform = m_player.getTransform();
    float light =
        m_world.getLightLevel(playerTransform.position.x, playerTransform.position.y) /
        15.0f;
    m_sceneShader.set("lightColour", glm::vec3{light, light, light});

    playerTransform.position.z += Player::box.z / 2;
    playerTransform.position.y -= Player::box.y;
    m_sceneShader.set("modelMatrix", createModelMatrix(playerTransform));
    m_playerCube.getRendable().drawElements();

    for (auto& particle : m_particles)
    {
        m_sceneShader.set("modelMatrix", createModelMatrix(particle.transform));
        m_particleCube.getRendable().drawElements();
    }
}

void Hoperload::onGUI()
{
    guiDebugScreen(m_camera.getTransform());
    m_player.gui();
}
