#pragma once

#include "Camera.h"
#include "GUI.h"
#include "Graphics/GLWrappers.h"
#include "Maths.h"
#include "World.h"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Window.hpp>
#include <vector>

class Keyboard;

class Hoperload
{

  public:
    Hoperload();

    void onEvent(const sf::Event& e);
    void onInput(const Keyboard& keyboard, const sf::Window& window, bool isMouseActive);
    void onUpdate(const sf::Time& time);
    void onRender();
    void onGUI();

  private:
    Shader m_sceneShader;
    VertexArray m_playerCube;
    Texture2D m_texture;

    Transform m_player;
    glm::vec3 m_playerVelocity;

    World m_world;
    Camera m_camera;
};
