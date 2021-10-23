#pragma once

#include "Camera.h"
#include "Graphics/GLWrappers.h"
#include "Maths.h"
#include "Player.h"
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

    World m_world;
    Player m_player;
    Camera m_camera;
};
