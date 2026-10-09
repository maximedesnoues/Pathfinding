#include "PlayerController.h"
#include "Unit.h"
#include "Map.h"
#include "Game.h"
#include <queue>
#include <stack>
#include "FlowFieldMap.h"
#include "Font.h"
#include "PointOfInterrest.h"


void PlayerController::UnselectAllUnits()
{
    for (auto unit : selectedUnits)
    {
        unit->SetSelected(false);
    }
    selectedUnits.clear();
}

void PlayerController::SelectUnitAtMousePos(int x, int y)
{
    auto unit = game->GetUnitAtPos(x / TILE_SIZE, y / TILE_SIZE);
    if (unit)
    {
        selectedUnits.insert(unit);
        unit->SetSelected(true);
    }
}

void PlayerController::SelectUnitsInRect(const SDL_Rect& rect)
{
    auto clampedRect = SDL_Rect{ rect.x / TILE_SIZE, rect.y / TILE_SIZE, (rect.x + rect.w) / TILE_SIZE, (rect.y + rect.h) / TILE_SIZE };
    if (clampedRect.w < clampedRect.x)
    {
        int tmp = clampedRect.x;
        clampedRect.x = clampedRect.w;
        clampedRect.w = tmp;
    }
    if (clampedRect.h < clampedRect.y)
    {
        int tmp = clampedRect.y;
        clampedRect.y = clampedRect.h;
        clampedRect.h = tmp;
    }

    game->SelectUnitsInRect(clampedRect, selectedUnits);
}

void PlayerController::Init(std::shared_ptr<Game> _game)
{
    game = _game;
}

void PlayerController::Update(int deltaTime)
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            game->Exit();
        }
        else if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                game->Exit();
            }
        }
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            int x, y;
            SDL_GetMouseState(&x, &y);

            if (e.button.button == 3)
            {
                UnselectAllUnits();
                textTextureDirty = true;
            }
            else
            {
                SDL_GetMouseState(&selectionRect.x, &selectionRect.y);
                selectionRect.w = selectionRect.h = 0;
                if (selectedUnits.size() != 0)
                {
                    CVect dest = CVect(selectionRect.x / TILE_SIZE * 1.0f, selectionRect.y / TILE_SIZE * 1.0f, 0);
                    MoveUnit(dest);
                }
                else
                {
                    selectionInProgress = true;
                }
            }
        }
        if (e.type == SDL_MOUSEMOTION)
        {
            if (selectionInProgress)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                selectionRect.w = x - selectionRect.x;
                selectionRect.h = y - selectionRect.y;
            }
        }
        if (e.type == SDL_MOUSEBUTTONUP)
        {
            if (selectionInProgress)
            {
                selectionInProgress = false;
                UnselectAllUnits();
                SelectUnitsInRect(selectionRect);
                textTextureDirty = true;
            }
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
        {
            if (selectedUnits.size() > 0)
            {
			    targetedVillage = FindCheapestVillageUsingDijkstra(*selectedUnits.begin(), allowDiag);

                if (targetedVillage != nullptr)
                {
                    CVect targetPos = targetedVillage->GetPosition();
                    MoveUnit(targetPos);
                }
            }
            textTextureDirty = true;
		}
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_a)
        {
            moveMode = (MoveMode)(((int)moveMode + 1) % 4);
            textTextureDirty = true;
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_d)
        {
            allowDiag = !allowDiag;
            textTextureDirty = true;
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
        {
            testMap = std::make_shared<FlowFieldMap>();
            testMap->GenerateFlowField(game, CVect(), true, allowDiag);
            for (auto unit : selectedUnits)
                unit->SetFlowFieldMap(testMap);
        }
    }
}

void PlayerController::MoveUnit(const CVect& dest)
{
    switch (moveMode)
    {
    case MoveMode::simple:
        for (auto unit : selectedUnits)
            unit->MoveTo(dest.x, dest.y);
        break;
    case MoveMode::path:
        for (auto unit : selectedUnits)
            unit->AddToPath(dest.x, dest.y);
        break;
    case MoveMode::aStar:

        for (auto unit : selectedUnits)
        {
            ComputePathUsingAStar(unit, dest, allowDiag);
        }
        break;
    case MoveMode::FlowField:
        testMap = std::make_shared<FlowFieldMap>();
        testMap->GenerateFlowField(game, dest, false, allowDiag);
        for (auto unit : selectedUnits)
            unit->SetFlowFieldMap(testMap);
        break;
    }
}

void PlayerController::Render(SDL_Renderer* screenRenderer)
{ 
    if (selectionInProgress)
        SDL_RenderDrawRect(screenRenderer, &selectionRect);    
    if(testMap != nullptr)
		testMap->Render(screenRenderer);
    if (targetedVillage != nullptr)
    {
        CVect pos = targetedVillage->GetPosition();
        SDL_Rect rect = {
            (int)pos.x * TILE_SIZE,
            (int)pos.y * TILE_SIZE,
            TILE_SIZE,
            TILE_SIZE
        };

        SDL_SetRenderDrawColor(screenRenderer, 255, 230, 0, 255);
        SDL_RenderDrawRect(screenRenderer, &rect);
    }

    if(textTextureDirty || textTexture == nullptr)
        RebuildText(screenRenderer);
    if (textTexture != nullptr)
    {
		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		SDL_QueryTexture(textTexture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(screenRenderer, textTexture, nullptr, &rect);
    }
}

PlayerController::~PlayerController()
{
    if(textTexture != nullptr)
        SDL_DestroyTexture(textTexture);
}

void PlayerController::SelectUnit(std::shared_ptr<Unit> unit)
{
    selectedUnits.insert(unit);
    unit->SetSelected(true);
}

void PlayerController::RebuildText(SDL_Renderer* screenRenderer)
{
    std::shared_ptr<Font> font = SpriteFactory::GetFont("arial.ttf", 24);
    SDL_Color White = { 255, 255, 255 };
    char message[4096];
#ifndef WIN32
    if (selectedUnits.size() == 0)
    {
        snprintf(message, 255, "No unit selected, select some by making a selection rectangle.");
    }
    else
    {
        static const std::string modes[4] = { "simple", "path", "A*" , "flow field" };
        snprintf(message, 255, "Nb selected: %zu.\nLeft click to move, right to unselect.\nSpace to select another unit, a to toggle move mode, d to toggle diagonals.\nCurrent move mode: %s - diag allowed: %s", selectedUnits.size(), modes[(int)moveMode].c_str(), allowDiag ? "true" : "false");
    }
#else
    if (selectedUnits.size() == 0)
    {
        sprintf_s(message, 255, "No unit selected, select some by making a selection rectangle.");
    }
    else
    {
        static const std::string modes[4] = { "simple", "path", "A*" , "flow field"};
        sprintf_s(message, 255, "Nb selected: %zu.\nLeft click to move, right to unselect.\nSpace to select another unit, a to toggle move mode, d to toggle diagonals.\nCurrent move mode: %s - diag allowed: %s", selectedUnits.size(), modes[(int)moveMode].c_str(), allowDiag ? "true" : "false");
    }
#endif
    if(textTexture)
        SDL_DestroyTexture(textTexture);
    textTexture = font->CreateTextTexture(message, White, screenRenderer);


}