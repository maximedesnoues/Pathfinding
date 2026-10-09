#include "FlowFieldMap.h"
#include "Map.h"
#include "GameConfig.h"

void FlowFieldMap::ResetMap(int width, int height)
{
	distanceMap.resize(height);
	for (int i = 0; i < height; i++)
	{
		distanceMap[i].resize(width);
		for (int j = 0; j < width; j++)
		{
			distanceMap[i][j] = 0xFFFFFFFF;
		}
	}

	flowField.resize(height);
	for (int i = 0; i < height; i++)
	{
		flowField[i].resize(width);
		for (int j = 0; j < width; j++)
		{
			flowField[i][j] = CVect(0, 0, 0);
		}
	}
}

CVect FlowFieldMap::GetNextTile(CVect current) const
{
	if (current.v[0] < 0 || current.v[0] >= flowField[0].size() || current.v[1] < 0 || current.v[1] >= flowField.size())
		return current;
	CVect dist = current + flowField[(unsigned int)current.v[1]][(unsigned int)current.v[0]];
	dist.v[0] = (int)dist.v[0] + 0.5f;
	dist.v[1] = (int)dist.v[1] + 0.5f;
	return dist;
}

void FlowFieldMap::Render(SDL_Renderer* screenRenderer)
{
	//display the distance map
	for (unsigned int i = 0; i < distanceMap.size(); i++)
	{
		for (unsigned int j = 0; j < distanceMap[i].size(); j++)
		{
			if (distanceMap[i][j] != 0xFFFFFFFF)
			{
				if(distanceMap[i][j] == 0 || distanceMap[i][j] == 0xFFFFFFFF)
					continue;
				SDL_SetRenderDrawColor(screenRenderer, 255, 0, 0, 255);
				CVect center = CVect((j + 0.5f) * TILE_SIZE, (i + 0.5f) * TILE_SIZE, 0);
				CVect dir = center + flowField[i][j] * TILE_SIZE / 2;
				SDL_RenderDrawLine(screenRenderer, (unsigned int)center.v[0], (unsigned int)center.v[1], (unsigned int)dir.v[0], (unsigned int)dir.v[1]);
			}
		}
	}

}
