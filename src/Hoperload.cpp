#include "Hoperload.h"

#include "Chunks/Conversions.h"
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
{
    m_sceneShader.loadFromFile("SceneVertex.glsl", "SceneFragment.glsl");
    m_lightCube.bufferMesh(createCubeMesh({0.5f, 1.5f, 0.5f}));
    m_texture.loadFromFile("OpenGLLogo.png", 8);

    m_player = {{50, worldHeight * CHUNK_SIZE - CHUNK_SIZE / 2 + 1, 1}, {0, 0, 0}};
    m_camera.hookTransform(&m_player);
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
    float PLAYER_SPEED = 0.5f;

    // if (freecam)
    //{
    //        m_camera.zoom -= e.mouseWheelScroll.delta / 2;
    //    if (keyboard.isKeyDown(sf::Keyboard::LControl))
    //    {
    //        PLAYER_SPEED = 5.0f;
    //    }
    //    if (keyboard.isKeyDown(sf::Keyboard::W))
    //    {
    //        camera.position += forwardsVector(camera.rotation) * PLAYER_SPEED;
    //    }
    //    else if (keyboard.isKeyDown(sf::Keyboard::S))
    //    {
    //        camera.position += backwardsVector(camera.rotation) * PLAYER_SPEED;
    //    }
    //    if (keyboard.isKeyDown(sf::Keyboard::A))
    //    {
    //        camera.position += leftVector(camera.rotation) * PLAYER_SPEED;
    //    }
    //    else if (keyboard.isKeyDown(sf::Keyboard::D))
    //    {
    //        camera.position += rightVector(camera.rotation) * PLAYER_SPEED;
    //    }
    //
    //    if (!isMouseActive)
    //    {
    //        return;
    //    }
    //    static auto lastMousePosition = sf::Mouse::getPosition(window);
    //    auto change = sf::Mouse::getPosition(window) - lastMousePosition;
    //    camera.rotation.x -= static_cast<float>(change.y * 0.5);
    //    camera.rotation.y += static_cast<float>(change.x * 0.5);
    //    sf::Mouse::setPosition({(int)window.getSize().x / 2, (int)window.getSize().y /
    //    2},
    //                           window);
    //    lastMousePosition.x = (int)window.getSize().x / 2;
    //    lastMousePosition.y = (int)window.getSize().y / 2;
    //
    //    camera.rotation.x = glm::clamp(camera.rotation.x, -89.9f, 89.9f);
    //    camera.rotation.y = (int)camera.rotation.y % 360;
    //}
    // else
    //{

    if (keyboard.isKeyDown(sf::Keyboard::W))
    {
        m_playerVelocity.y += PLAYER_SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::S))
    {
        m_playerVelocity.y -= PLAYER_SPEED;
    }
    if (keyboard.isKeyDown(sf::Keyboard::A))
    {
        m_playerVelocity.x -= PLAYER_SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::D))
    {
        m_playerVelocity.x += PLAYER_SPEED;
    }

    if (keyboard.isKeyDown(sf::Keyboard::Space))
    {
        m_world.breakBlock(m_player.position.x, m_player.position.y);
    }
}

void Hoperload::onUpdate(const sf::Time& time)
{
    m_camera.update();
    if (!freecam)
    {
        m_player.position += m_playerVelocity * time.asSeconds();
        m_playerVelocity *= 0.98;
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
    float light = m_world.lightLevelAt(m_player.position.x, m_player.position.y) / 15.0f;
    m_sceneShader.set("lightColour", glm::vec3{light, light, light});

    auto lightModel = createModelMatrix(m_player);
    m_sceneShader.set("modelMatrix", lightModel);
    m_lightCube.getRendable().drawElements();

    m_world.render(m_camera);
}
void Hoperload::onGUI()
{ /*guiDebugScreen(m_camera.m_transform);*/
}
