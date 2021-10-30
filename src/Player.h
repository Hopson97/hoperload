#pragma once

#include "Maths.h"
#include <SFML/Window/Window.hpp>

class Keyboard;
class World;

enum class PlayerState
{
    Exploring,
    Digging,
};

struct Particle
{
    Transform transform;
    glm::vec3 direction;
    float lifetime;
};

class Player
{
  public:
    Player(World& world);

    void input(const Keyboard& keyboard, const sf::Window& window);

    void update(const sf::Time& dt, std::vector<Particle>& particles);

    void gui();

    const Transform& getTransform() const;

    static inline glm::vec3 box{0.9f, 0.9f, 0.9f};

  private:
    void beginDig(int offsetX, int offsetY);

    void resolveCollisions(const glm::vec3& vel);

    Transform m_transform;
    glm::vec3 m_velocity;
    bool m_isOnGround = false;
    bool m_isTouchingWall = false;

    PlayerState m_state = PlayerState::Exploring;
    glm::vec2 m_digDirection{0.0f, 0.0f};
    glm::ivec2 m_digLocation;
    float m_digSpeed = 1.5f;
    float m_digProgress = 0;

    World* m_pWorld;
};
