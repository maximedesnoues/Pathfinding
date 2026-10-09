#pragma once
#include "Sprite.h"
#include <memory>

//Tiles configuration make easier: an enum for each type of tile, and a structure to get the info of each tile
enum TileType
{
    Grass,
    HighGrass,
    Dirt,
    Destroyed, // Conservé pour compatibilité

    Sand,
    Road,
    Bridge,
    Water,
    Trees,
    Rock,
    Mountain,
    Swamp
};

struct TileConfig
{
    TileType type;
    unsigned int cost;
    SDL_Rect renderRect;

    TileConfig(TileType _name, unsigned int _cost, int _posXOnSheet, int _posYOnSheet, int _tileWidth, int _tileHeight)
    {
        type = _name;
        cost = _cost;
        renderRect = SDL_Rect{ _posXOnSheet * _tileWidth , _posYOnSheet * _tileHeight , _tileWidth , _tileHeight };
    }

    void Render(SDL_Renderer* screenRenderer, Sprite* tileset, int posX, int posY)
    {
        tileset->Render(screenRenderer, posX, posY, renderRect);
    }
};

class Tile 
{
protected:
    std::shared_ptr<TileConfig> type;

public:
	Tile() = delete;
    Tile(std::shared_ptr<TileConfig> _type, int posX, int posY) : type(_type) {};
	Tile(const Sprite&) = delete;
	Tile(Tile&&) = delete;
	Tile& operator=(const Tile&) = delete;
	~Tile() = default;

    void Render(SDL_Renderer* screenRenderer, Sprite* tileset, int posX, int posY)
    {
        type->Render(screenRenderer, tileset, posX, posY);
    };

    inline int GetCost()
    {
        return type->cost;
    }

    inline TileType GetType()
    {
        return type->type;
    }

};

