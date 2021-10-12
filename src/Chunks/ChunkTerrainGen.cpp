#include "ChunkTerrainGen.h"
#include "Voxels.h"

void createChunkTerrain(Chunk& chunk, int chunkX, int chunkY, int worldWidth,
                        int worldHeight, const TerrainGenOptions& terrainGenOptions)
{
    int heightInVoxels = (worldHeight * CHUNK_SIZE) - 1;

    for (int y = 0; y < CHUNK_SIZE; y++)
    {
        auto voxelHeight = CHUNK_SIZE * chunkY + y;

        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            // Top level
            if (voxelHeight == heightInVoxels)
            {
                chunk.setVoxel({x, y, 0}, GRASS);
                chunk.setVoxel({x, y, 1}, GRASS);
            }

            // Upper dirt layer
            else if (voxelHeight > heightInVoxels - 3)
            {
                chunk.setVoxel({x, y, 0}, DIRT);
                chunk.setVoxel({x, y, 1}, DIRT);
            }

            // Lower dirt layer
            else if (voxelHeight > heightInVoxels - CHUNK_SIZE)
            {
                chunk.setVoxel({x, y, 0}, DIRT);
                if (rand() % 64 < 40)
                {
                    chunk.setVoxel({x, y, 1}, DIRT);
                }
            }

            // Stoney
            else
            {
                chunk.setVoxel({x, y, 0}, STONE);
                if (rand() % 64 < 40)
                {
                    chunk.setVoxel({x, y, 1}, STONE);
                }
            }
        }
    }
}
