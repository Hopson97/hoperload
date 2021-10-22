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
    auto& chunk = m_chunkMap.setVoxel(worldToGlobalVoxelPosition({x, y, 1}), id);
    chunk.resetLights();
    chunk.floodLights();

    ChunkMesh mesh = createGreedyChunkMesh(chunk);
    VertexArray chunkVertexArray;
    chunkVertexArray.bufferMesh(mesh);
    m_chunkRendersList[chunk.position()].~VertexArray();
    m_chunkRendersList[chunk.position()] = std::move(chunkVertexArray);
}

int World::getLightLevel(int x, int y)
{
    return m_chunkMap.getLightLevel(worldToGlobalVoxelPosition({x, y, 1}));
}

void World::render(const Camera& camera)
{
    // Chunks
    m_voxelShader.bind();
    m_voxelShader.set("projectionViewMatrix", camera.getProjectionView());
    m_chunkTextures.bind();

    glm::mat4 voxelModel{1.0f};
    voxelModel = glm::translate(voxelModel, {0, 0, 0});
    m_voxelShader.set("modelMatrix", voxelModel);

    for (auto& [position, chunk] : m_chunkRendersList)
    {
        chunk.getRendable().drawElements();
    }
}