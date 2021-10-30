#include "Hoperload.h"

#include "Chunks/Conversions.h"
#include "Chunks/Voxels.h"
#include "GUI.h"
#include "Utility.h"

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

        auto& position = m_player.getTransform().position;
        if (keyboard.isKeyDown(sf::Keyboard::Space))
        {
            for (int y = -1; y < 1; y++)
            {

                for (int x = -1; x < 1; x++)
                {
                    m_world.breakBlock(position.x + x, position.y + y);
                }
            }
        }

        if (keyboard.isKeyDown(sf::Keyboard::U))
        {
            m_world.placeBlock(position.x, position.y, VoxelType::TEST_TORCH);
        }
    }
}

void Hoperload::onUpdate(const sf::Time& dt)
{
    m_camera.update(!freecam);

    if (!freecam)
    {
        m_player.update(dt);
    }
}

void Hoperload::onRender()
{
    setClearColour(COLOUR_SKY_BLUE);

    Framebuffer::unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Normal stuff
    m_sceneShader.bind();
    m_sceneShader.set("projectionViewMatrix", m_camera.getProjectionView());

    glEnable(GL_CULL_FACE);
    auto playerTransform = m_player.getTransform();
    float light =
        m_world.getLightLevel(playerTransform.position.x, playerTransform.position.y) /
        15.0f;
    m_sceneShader.set("lightColour", glm::vec3{light, light, light});

    playerTransform.position.y -= Player::box.y;
    auto lightModel = createModelMatrix(playerTransform);
    m_sceneShader.set("modelMatrix", lightModel);
    m_playerCube.getRendable().drawElements();

    m_world.render(m_camera);
}

void Hoperload::onGUI()
{
    guiDebugScreen(m_camera.getTransform());
    m_player.gui();
}
