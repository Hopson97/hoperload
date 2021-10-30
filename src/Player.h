#pragma once

#include "Maths.h"
#include <SFML/Window/Window.hpp>

class Keyboard;
class World;

class Player
{
  public:
    Player(World& world);

    void input(const Keyboard& keyboard, const sf::Window& window);

    void update(const sf::Time& dt);

    void gui();

    const Transform& getTransform() const;

    static inline glm::vec3 box{0.5f, 0.5f, 0.5f};

  private:
    void resolveCollisions(const glm::vec3& vel);

    Transform m_transform;
    glm::vec3 m_velocity;
    bool m_isOnGround = false;

    World* m_pWorld;
};
