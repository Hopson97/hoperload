#include "World.h"
#include "Camera.h"
#include "Chunks/ChunkMesh.h"
#include "Chunks/ChunkTerrainGen.h"
#include "Chunks/Conversions.h"
#include "Chunks/Voxels.h"

namespace
{
    int worldHeight = 14;
    int worldWidth = 4;
} // namespace

World::World()
{
    m_voxelShader.loadFromFile("TerrainVertex.glsl", "TerrainFragment.glsl");
    m_chunkTextures.create(16, 16);
    initVoxelSystem(m_chunkTextures);

    for (int cy = 0; cy < worldHeight; cy++)
    {

        for (int cx = 0; cx < worldWidth; cx++)
        {
            Chunk& chunk = m_chunkMap.addChunk({cx, cy});
            createChunkTerrain(chunk, cx, cy, worldWidth, worldHeight, {});

            chunk.floodLights();

            ChunkMesh mesh = createGreedyChunkMesh(chunk);
            VertexArray chunkVertexArray;
            chunkVertexArray.bufferMesh(mesh);
            m_chunkRendersList[chunk.position()] = std::move(chunkVertexArray);
        }
    }

    m_chunkBuildThread = std::thread([&]{
        doChunkBuildThread();
    });
}

World::~World() 
{
    m_isRunning = false;
    m_chunkBuildThread.join();
}

void World::update()
{
}

void World::breakBlock(int x, int y)
{
    placeBlock(x, y, AIR);
}

void World::placeBlock(int x, int y, VoxelID id)
{
    std::unique_lock<std::mutex> lock(m_lock);
    auto& chunk = m_chunkMap.setVoxel(worldToGlobalVoxelPosition({x, y, 1}), id);
    m_chunkBuildQueue.emplace(chunk.position());
}

int World::getLightLevel(int x, int y)
{
    std::unique_lock<std::mutex> lock(m_lock);
    return m_chunkMap.getLightLevel(worldToGlobalVoxelPosition({x, y, 1}));
}

void World::doChunkBuildThread() 
{
    while(m_isRunning) {
        std::unique_lock<std::mutex> lock(m_lock);
        if (!m_chunkBuildQueue.empty()) {
            auto itr = m_chunkBuildQueue.begin();
            auto& chunk = m_chunkMap.getChunk(*itr);
            
            chunk.resetLights();
            chunk.floodLights();

            ChunkMesh mesh = createGreedyChunkMesh(chunk);
            m_chunkBufferQueue.emplace(chunk.position(), std::move(mesh));
            m_chunkBuildQueue.erase(itr);
        }
    }
}


/*
.emplace(std::piecewise_construct, std::forward_as_tuple(chunk),
                     std::forward_as_tuple(this, chunk))
*/
void World::render(const Camera& camera)
{
    // Chunks
    m_voxelShader.bind();
    m_voxelShader.set("projectionViewMatrix", camera.getProjectionView());
    m_chunkTextures.bind();

    glm::mat4 voxelModel{1.0f};
    voxelModel = glm::translate(voxelModel, {0, 0, 0});
    m_voxelShader.set("modelMatrix", voxelModel);

    std::unique_lock<std::mutex> lock(m_lock);
    for (auto& [position, chunk] : m_chunkRendersList)
    {
        auto itr = m_chunkBufferQueue.find(position);
        if (itr != m_chunkBufferQueue.end()) {
            VertexArray chunkVertexArray;
            chunkVertexArray.bufferMesh(itr->second);
            m_chunkRendersList[position].~VertexArray();
            m_chunkRendersList[position] = std::move(chunkVertexArray);
            m_chunkBufferQueue.erase(itr);
        }
        chunk.getRendable().drawElements();
    }
}