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

void floodLights(Chunk& chunk, VoxelPosition position, int lightLevel)
{
    chunk.setSunlight(position, lightLevel);
    chunk.setSunlight(position + glm::ivec3{0, 0, -1}, std::max(lightLevel - 2, 1));

    auto v = chunk.getVoxel(position);
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
        lightLevel -= 4;
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

void World::update() {}

void World::breakBlock(int x, int y)
{
    placeBlock(x, y, AIR);
}

void World::placeBlock(int x, int y, VoxelID id) 
{
    auto& chunk = m_chunkMap.setVoxel(worldToGlobalVoxelPosition({x, y, 1}), id);
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
    m_chunkRendersList[chunk.position()].~VertexArray();
    m_chunkRendersList[chunk.position()] = std::move(chunkVertexArray);
}

int World::lightLevelAt(int x, int y)
{
    return m_chunkMap.getSunlight(worldToGlobalVoxelPosition({x, y, 1}));
}

void World::render(const Camera& camera)
{
    // Chunks
    m_voxelShader.bind();
    m_voxelShader.set("projectionViewMatrix", camera.m_pvMatrix);
    m_chunkTextures.bind();

    glm::mat4 voxelModel{1.0f};
    voxelModel = glm::translate(voxelModel, {0, 0, 0});
    m_voxelShader.set("modelMatrix", voxelModel);

    for (auto& [position, chunk] : m_chunkRendersList)
    {
        chunk.getRendable().drawElements();
    }
}