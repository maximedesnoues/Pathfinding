#include "FlowFieldMap.h"
#include "PlayerController.h"
#include "Map.h"
#include "Game.h"
#include "Unit.h"

#include <queue>
#include <memory>

std::shared_ptr<PointOfInterrest> PlayerController::FindCheapestVillageUsingDijkstra(std::shared_ptr<Unit> unit, bool allowDiags = false)
{
	//This function selects the nearest Village from unit
	//all point of interrests are in game->GetPointsOfInterest()

	return nullptr;

}

void PlayerController::ComputePathUsingAStar(std::shared_ptr<Unit> unit, CVect to, bool allowDiags = false)
{
	//This function computes the path from a unit to a destination using A*

	//Temp code, to be replaced by your own code
	std::vector<CVect> path;
	path.push_back(to);
	unit->SetPath(path);
}

void FlowFieldMap::GenerateFlowField(std::shared_ptr<Game> game, CVect cvect, bool useCamps = false, bool allowDiags = false)
{
	//This function generates the flow field to get to the right destination:
	//if useCamps = false => use cvect
	//if useCamps = true => use game to get the proper camp
	auto map = game->GetMap();
	ResetMap(map->GetWidth(), map->GetHeight());
	
}