#pragma once

#include "Chunks/Chunk.h"
#include "GUI.h"
#include "Graphics/GLWrappers.h"
#include "Maths.h"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Window.hpp>
#include <atomic>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class Keyboard;

class Game
{

  public:
    Game();

    void onInput(const Keyboard& keyboard, const sf::Window& window, bool isMouseActive);

    void onUpdate();

    void onRender();

    void onGUI();


  private:
    void breakBlock();
    void renderScene(const glm::mat4& projectionViewMatrix);

    Shader m_sceneShader;
    VertexArray m_lightCube;
    Texture2D m_texture;

    ViewFrustum m_frustum;
    Transform m_cameraTransform;
    Transform m_playerPosition;

    glm::mat4 m_projectionMatrix;

    sf::Clock m_timer;

    ChunkMap m_chunkMap;

    TextureArray2D m_chunkTextures;
    std::unordered_map<ChunkPosition, VertexArray, ChunkPositionHash> m_chunkRendersList;
    std::vector<VertexArray> m_chunkVertexArrays;
    Shader m_voxelShader;
};
