#include "Game.h"

#include "Chunks/ChunkMesh.h"
#include "Chunks/ChunkTerrainGen.h"
#include "Chunks/Voxels.h"
#include "Utility.h"

int worldHeight = 4;
int worldWidth = 4;

int toIndex(int x, int y) { return x + y * CHUNK_SIZE; }

void floodLights(Chunk& chunk, VoxelPosition position, int lightLevel)
{
    chunk.setSunlight(position, lightLevel);
    chunk.setSunlight(position + glm::ivec3{0, 0, -1}, std::max(lightLevel - 2, 1));

    auto v = chunk.qGetVoxel(position);
    if (v == AIR)
    {

        lightLevel -= 1;
    }
    else if (v == WATER)
    {
        lightLevel -= 2;
    }
    else
    {
        lightLevel -= 6;
    }

    if (lightLevel <= 0)
    {
        return;
    }

    auto tryFlood = [&](const glm::ivec3 offset) {
        auto newFloodPosition = position + offset;

        if (chunk.getSunlight(newFloodPosition) <= lightLevel)
        {
            floodLights(chunk, newFloodPosition, lightLevel);
        }
    };

    tryFlood({1, 0, 0});
    tryFlood({-1, 0, 0});
    tryFlood({0, 1, 0});
    tryFlood({0, -1, 0});
}

Game::Game()
{
    m_sceneShader.loadFromFile("SceneVertex.glsl", "SceneFragment.glsl");
    m_lightCube.bufferMesh(createCubeMesh({0.5f, 1.5f, 0.5f}));
    m_texture.loadFromFile("OpenGLLogo.png", 8);

    m_voxelShader.loadFromFile("TerrainVertex.glsl", "TerrainFragment.glsl");

    float aspect = (float)WIDTH / (float)HEIGHT;
    m_projectionMatrix = createProjectionMatrix(aspect, 110.0f);

    m_chunkTextures.create(16, 16);
    initVoxelSystem(m_chunkTextures);

    m_cameraTransform = {{50, worldHeight * CHUNK_SIZE - CHUNK_SIZE / 2, 10},
                         {0, 270, 0}};
    m_playerPosition = {{50, worldHeight * CHUNK_SIZE - CHUNK_SIZE / 2 + 1, 1},
                        {0, 0, 0}};

    for (int cy = 0; cy < 4; cy++)
    {

        for (int cx = 0; cx < 2; cx++)
        {
            Chunk& chunk = m_chunkMap.addChunk({cx, cy});
            createChunkTerrain(chunk, cx, cy, worldWidth, worldHeight, {});

            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                for (int y = 0; y < CHUNK_SIZE; y++)
                {
                    if (chunk.qGetVoxel({x, y, 0}) == AIR)
                    {
                        floodLights(chunk, {x, y, 1}, 16);
                    }
                }
            }

            ChunkMesh mesh = createGreedyChunkMesh(chunk);
            VertexArray chunkVertexArray;
            chunkVertexArray.bufferMesh(mesh);
            m_chunkRendersList[chunk.position()] = std::move(chunkVertexArray);
        }
    }
}

void Game::onInput(const Keyboard& keyboard, const sf::Window& window, bool isMouseActive)
{
    Transform& camera = m_cameraTransform;

    float PLAYER_SPEED = 0.1f;
    // if (keyboard.isKeyDown(sf::Keyboard::LControl))
    //{
    //    PLAYER_SPEED = 5.0f;
    //}
    // if (keyboard.isKeyDown(sf::Keyboard::W))
    //{
    //    camera.position += forwardsVector(camera.rotation) * PLAYER_SPEED;
    //}
    // else if (keyboard.isKeyDown(sf::Keyboard::S))
    //{
    //    camera.position += backwardsVector(camera.rotation) * PLAYER_SPEED;
    //}
    // if (keyboard.isKeyDown(sf::Keyboard::A))
    //{
    //    camera.position += leftVector(camera.rotation) * PLAYER_SPEED;
    //}
    // else if (keyboard.isKeyDown(sf::Keyboard::D))
    //{
    //    camera.position += rightVector(camera.rotation) * PLAYER_SPEED;
    //}

    if (keyboard.isKeyDown(sf::Keyboard::W))
    {
        m_playerPosition.position.y += PLAYER_SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::S))
    {
        m_playerPosition.position.y -= PLAYER_SPEED;
    }
    if (keyboard.isKeyDown(sf::Keyboard::A))
    {
        m_playerPosition.position.x -= PLAYER_SPEED;
    }
    else if (keyboard.isKeyDown(sf::Keyboard::D))
    {
        m_playerPosition.position.x += PLAYER_SPEED;
    }

    m_cameraTransform.position.x = m_playerPosition.position.x;
    m_cameraTransform.position.y = m_playerPosition.position.y;

    if (keyboard.isKeyDown(sf::Keyboard::Space))
    {
        for (int y = -1; y <= 1; y++)
        {
            for (int x = -1; x <= 1; x++)
            {
                breakBlock(m_playerPosition.position.x + x,
                           m_playerPosition.position.y + y);
            }
        }
    }

    // if (!isMouseActive)
    {
        return;
    }
    static auto lastMousePosition = sf::Mouse::getPosition(window);
    auto change = sf::Mouse::getPosition(window) - lastMousePosition;
    camera.rotation.x -= static_cast<float>(change.y * 0.5);
    camera.rotation.y += static_cast<float>(change.x * 0.5);
    sf::Mouse::setPosition({(int)window.getSize().x / 2, (int)window.getSize().y / 2},
                           window);
    lastMousePosition.x = (int)window.getSize().x / 2;
    lastMousePosition.y = (int)window.getSize().y / 2;

    camera.rotation.x = glm::clamp(camera.rotation.x, -89.9f, 89.9f);
    camera.rotation.y = (int)camera.rotation.y % 360;
}

void Game::onUpdate() {}

void Game::breakBlock(int x, int y)
{
    // Convert player position to voxel position
    VoxelPosition localVoxel = {x % CHUNK_SIZE, y % CHUNK_SIZE, 1};

    // Convert to chunk position
    ChunkPosition localChunk = {
        x / CHUNK_SIZE,
        y / CHUNK_SIZE,
    };

    // Get the global voxel position
    VoxelPosition globalVoxel = {localVoxel.x + localChunk.x * CHUNK_SIZE,
                                 localVoxel.y + localChunk.y * CHUNK_SIZE, 1};

    auto& chunk = m_chunkMap.setVoxel(globalVoxel, AIR);
    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            if (chunk.qGetVoxel({x, y, 0}) == AIR)
            {
                floodLights(chunk, {x, y, 1}, 16);
            }
        }
    }
    ChunkMesh mesh = createGreedyChunkMesh(chunk);
    VertexArray chunkVertexArray;
    chunkVertexArray.bufferMesh(mesh);
    m_chunkRendersList[chunk.position()] = std::move(chunkVertexArray);
}

void Game::onRender()
{
    setClearColour(COLOUR_SKY_BLUE);

    auto viewMatrix = createViewMartix(m_cameraTransform, {0, 1, 0});
    auto projectionViewMatrix = m_projectionMatrix * viewMatrix;
    m_frustum.update(projectionViewMatrix);

    Framebuffer::unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderScene(projectionViewMatrix);
}

void Game::onGUI() { guiDebugScreen(m_cameraTransform); }

void Game::renderScene(const glm::mat4& projectionViewMatrix)
{
    // Normal stuff
    m_sceneShader.bind();
    m_sceneShader.set("projectionViewMatrix", projectionViewMatrix);
    m_sceneShader.set("eyePosition", m_cameraTransform.position);

    glEnable(GL_CULL_FACE);

    m_sceneShader.set("lightColour", glm::vec3{1.0, 1.0, 1.0});

    auto lightModel = createModelMatrix(m_playerPosition);
    m_sceneShader.set("modelMatrix", lightModel);
    m_lightCube.getRendable().drawElements();

    // Chunks
    m_voxelShader.bind();
    m_voxelShader.set("projectionViewMatrix", projectionViewMatrix);
    m_chunkTextures.bind();

    glm::mat4 voxelModel{1.0f};
    voxelModel = glm::translate(voxelModel, {0, 0, 0});
    m_voxelShader.set("modelMatrix", voxelModel);

    for (auto& [position, chunk] : m_chunkRendersList)
    {
        chunk.getRendable().drawElements();
    }
}
