#include "Game.h"
#include "Map.h"
#include "IGameEntity.h"
#include <SDL.h>
#include "CollisionManager.h"
#include "ICollidable.h"
#include <iostream>
#include <SDL_scancode.h>
#include "Font.h"
#include "SpriteFactory.h"
#include <time.h>
#include "Unit.h"
#include "PlayerController.h"
#include "PointOfInterrest.h"

Game::~Game() = default;

typedef int Token;

bool Game::Init()
{
	_isRunning = true;
    playerController = std::make_shared<PlayerController>();
    playerController->Init(thisWeakPtr.lock());

    map = Map::Create();
    map->Init(nullptr);

    //map POI
    for (int i = 0; i < 15; ++i)
    {
        auto pointOfInterrest = PointOfInterrest::Create();
        PointOfInterrestConfig config;
        if (i < 3)
            config.type = EntityType::GoblinCamp;
        else
            config.type = EntityType::Village;
        pointOfInterrest->Init(&config);
        while (true)
        {
            int x = rand() % map->GetWidth();
            int y = rand() % map->GetHeight();
            if (map->GetTileType(x, y) == TileType::Grass)
            {
                pointOfInterrest->SetPosition(x, y);
                break;
            }
        }
        _entities.insert(pointOfInterrest);
        pointsOfInterest.push_back(pointOfInterrest);
    }

    //create units
    int nb = 10;
    for (int i = 0; i < nb; ++i)
    {
        auto unit = Unit::Create();
        UnitConfig config;
        config.map = map;
        unit->Init(&config);
        while (true)
        {
            int x = rand() % map->GetWidth();
            int y = rand() % map->GetHeight();
            if (map->GetTileCost(x, y) != 0xFFFFFFFF)
            {
                unit->SetPosition(x, y);
                break;
            }
        }
        _entities.insert(unit);
        collisionManager.AddCollider(unit);
        collisionManager.AddCollisionCallback(unit, 1, std::bind(&Unit::OnCollide, unit, std::placeholders::_1));
    }

	return true;
}

void Game::Update(int deltaTime)
{
    
    playerController->Update(deltaTime);

    if (_isRunning)
    {
        collisionManager.Tick();
        if (_entitiesToRemove.size() > 0)
        {
            for (auto entity : _entitiesToRemove)
            {
                RemoveEntity(entity);
            }
            _entitiesToRemove.clear();
        }

        _delayEntityRemoval = true;
        for (auto entity : _entities)
        {
            entity->Update(deltaTime);
        }
        _delayEntityRemoval = false;
        if (_entitiesToRemove.size() > 0)
        {
            for (auto entity : _entitiesToRemove)
            {
                RemoveEntity(entity);
            }
            _entitiesToRemove.clear();
        }

    }
}

void Game::Render(SDL_Renderer* screenRenderer)
{
    
	if (_isRunning)
    {
        _delayEntityRemoval = true;
        map->Render(screenRenderer);
        /*for (auto entity : _entities)
        {
            entity->Render(screenRenderer);
        }*/

        for (auto entity : _entities)
        {
            if (entity->GetEntityType() != EntityType::Unit)
                entity->Render(screenRenderer);
        }

        for (auto entity : _entities)
        {
            if (entity->GetEntityType() == EntityType::Unit)
                entity->Render(screenRenderer);
        }

        playerController->Render(screenRenderer);
        _delayEntityRemoval = false;
    }

#if _DEBUG
    
    std::shared_ptr<Font> font = SpriteFactory::GetFont("arial.ttf", 24);
    SDL_Color White = { 255, 255, 255 };
    char message[255];
    #ifndef WIN32
        snprintf(message, 255, "Entities: %zu", _entities.size());
    #else
        sprintf_s(message, 255, "Entities: %zu", _entities.size());
    #endif
    auto texture = font->CreateTextTexture(message, White, screenRenderer);
    int texW = 0;
    int texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { 0, 480 - texH, texW, texH };
    SDL_RenderCopy(screenRenderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    
#endif
}

void Game::Release()
{
    for (auto entity : _entities)
    {
        entity->Release();
    }
    _entities.clear();

}


bool Game::IsRunning() const
{
	return _isRunning;
}

void Game::AddEntity(std::shared_ptr<IGameEntity> entity)
{
    _entities.insert(entity);
}

void Game::RemoveEntity(std::shared_ptr<IGameEntity> entity)
{
    if (_delayEntityRemoval)
    {
        _entitiesToRemove.insert(entity);
        return;
    }
    _entities.erase(entity);
}

std::shared_ptr<Unit> Game::GetUnitAtPos(int x, int y, bool excludeSelected)
{
    for (auto elem : _entities)
    {
        std::shared_ptr<Unit> unit = std::dynamic_pointer_cast<Unit>(elem);
        if (unit != nullptr && unit->IsOnTile(x, y))
        {
              if(!excludeSelected || !unit->IsSelected())
                return unit;
        }
    }
    return nullptr;
}

void Game::SelectUnitsInRect(const SDL_Rect& rect, std::set< std::shared_ptr<Unit>>& selectedUnits)
{
    //shortcut: w & h = pos 2
    for (auto elem : _entities)
    {
        std::shared_ptr<Unit> unit = std::dynamic_pointer_cast<Unit>(elem);
        if (unit != nullptr && unit->IsInside(rect))
        {
            unit->SetSelected(true);
            selectedUnits.insert(unit);
        }
    }
}

void Game::Exit()
{
    _isRunning = false;
}

std::shared_ptr<Game> Game::Create()
{
    std::shared_ptr<Game> newShared = std::make_shared<Game>();
    newShared->thisWeakPtr = newShared;
    return newShared;
}
