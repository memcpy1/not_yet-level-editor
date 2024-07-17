//SDL
#include <SDL.h>
//STL
#include <iostream>
#include <vector>
#include <algorithm>
//vendor
#include <SDL_prims.h>
#undef main


struct Vector2D
{
    float x;
    float y;
};

struct Vector2Di
{
    int x;
    int y;
};

enum Material
{
    STONE = 0,
    GLASS = 1,
    ICE = 2
};

enum PlatformType
{
    STATIC = 0,
    ANCHOR = 1
};

void DrawCartesianAxis(SDL_Renderer* renderer)
{
   SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0xFF);
   SDL_RenderDrawLine(renderer, 0, 720 / 2, 1280, 720 / 2);
   SDL_SetRenderDrawColor(renderer, 0, 255, 60, 0xFF);
   SDL_RenderDrawLine(renderer, 1280 / 2, 0, 1280 / 2, 720);
}

void DrawGridline(const int& pGridSize, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 33, 33, 33, 255);

   for (int x = 0; x < 1 + 32 * pGridSize; x += pGridSize) 
   {
      SDL_RenderDrawLine(renderer, x, 0, x, 720);
   }

   for (int y = 0; y < 1 + 18 * pGridSize; y += pGridSize) 
   {
      SDL_RenderDrawLine(renderer, 0, y, 1280, y);
   }
}

std::size_t CreatePlatformID()
{
    static std::size_t Entities = 0;
    ++Entities;
    return Entities;
}

class Platform
{
private:  
    std::size_t ID;
    int Type;
    bool Selected;
    Vector2Di StartPos;
    std::vector<SDL_Point> SDLVerteces;
    int Width;
    int Height;

public:
    Platform(const Vector2Di& center, const int& type = PlatformType::STATIC)
        : StartPos({center.x - 20, center.y - 20}), Width(40), Height(40), Type(type), ID(CreatePlatformID()), Selected(0) 
    {
        SDLVerteces.push_back(SDL_Point(center.x - 20, center.y - 20));
        SDLVerteces.push_back(SDL_Point(center.x - 20, center.y + 20));
        SDLVerteces.push_back(SDL_Point(center.x + 20, center.y + 20));
        SDLVerteces.push_back(SDL_Point(center.x + 20, center.y - 20));
    
        std::cout << "Verteces :  " << SDLVerteces.size() << '\n';
        std::cout << "StartPos :  " << SDLVerteces[0].x << " | " << SDLVerteces[0].y << '\n';
        std::string strType = (Type == PlatformType::STATIC) ? "STATIC" : "ANCHOR";
        std::cout << "Type :  " << strType << '\n'; 
    }

    Platform(const Vector2Di& startPos, const int& width, const int& height, const int& type = PlatformType::STATIC)
        : StartPos(startPos), Width(width), Height(height), Type(type), Selected(0) 
    {
        SDLVerteces.push_back(SDL_Point(startPos.x, startPos.y));
        SDLVerteces.push_back(SDL_Point(startPos.x + width, startPos.y));
        SDLVerteces.push_back(SDL_Point(startPos.x + width, startPos.y + height));
        SDLVerteces.push_back(SDL_Point(startPos.x, startPos.y + height));
    }

    void Render(SDL_Renderer* renderer)
    {
        SDL_Color color(1, 1, 1, 1);
        if (Selected)
            color = SDL_Color(0, 1, 0, 1);

        SDL_DrawPolygon(renderer, SDLVerteces.data(), SDLVerteces.size(), color);
    }

    bool Collision(const Vector2Di& p)
    {
        if (p.x >= StartPos.x && p.x <= StartPos.x + Width && p.y >= StartPos.y && p.y <= StartPos.y + Height)
            return true;
        else
            return false;
    }

    bool Collision(const Platform& platform)
    {

    }

    void Select()
    {
        std::cout << "selected" << '\n';
        Selected = true;
    }

    bool isSelected()
    {
        return Selected;
    }

    void Deselect()
    {
        Selected = false;
    }

    Vector2Di GetStartPos()
    {
        return StartPos;
    }

    int GetWidth()
    {
        return Width;
    }

    int GetHeight()
    {
        return Height;
    }

    friend bool operator==(const Platform& plat1, const Platform& plat2)
    {
        return plat1.ID == plat2.ID;
    }
};

class Stage
{
public:
    std::vector<Platform> Platforms;
    std::size_t nPlatforms;
public:
    void AddPlatform(const Platform& platform)
    {
        Platforms.push_back(platform);
        nPlatforms = Platforms.size();
    }

    void DeleteSelectedPlatforms()
    {
        for (int i = 0; i < Platforms.size(); i++)
        {
            if (Platforms[i].isSelected())
            {
                auto it = std::find(Platforms.begin(), Platforms.end(), Platforms[i]);
                Platforms.erase(it);
            }
        }
    }

    void DeletePlatforms(const std::vector<Platform>& platformList)
    {
        for (int i = 0; i < Platforms.size(); i++)
        {
            for (int j = 0; j < platformList.size(); j++)
            {
                if (platformList[j] == Platforms[i])
                {
                    auto it = std::find(Platforms.begin(), Platforms.end(), Platforms[i]);
                    Platforms.erase(it);
                }
                    
            }
        }
    }

    void MergeSelectedIntoRect()
    {
        std::vector<Platform> Selected;
        Vector2Di StartPosition = {0};
        int Width = 0;
        int Height = 0;
        
        for (int i = 0; i < Platforms.size(); i++)
        {
            if (Platforms[i].isSelected())
                Selected.push_back(Platforms[i]);
        }

        

        StartPosition.x = Selected[0].GetStartPos().x;
        StartPosition.y = Selected[0].GetStartPos().y;

        for (int i = 0; i < Selected.size(); i++)
        {
            if (Selected[i].GetStartPos().x < StartPosition.x)
                StartPosition.x = Selected[i].GetStartPos().x;
            if (Selected[i].GetStartPos().y < StartPosition.y)
                StartPosition.y = Selected[i].GetStartPos().y;

            Width += Selected[i].GetWidth();
            Height += Selected[i].GetHeight();
        }

        
        AddPlatform(Platform(StartPosition, Width / 2, Height / 2));
        DeleteSelectedPlatforms(); //Vector out of subscript
    }
};

int main()
{
    int Width = 1280;
    int Height = 720;

    int Mouse_x = 0;
    int Mouse_y = 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "[SDL2]: SDL_Init() failed   : " << SDL_GetError() << '\n';
        return false;
    }

    SDL_Window* Window = SDL_CreateWindow("not_yet_level_editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_SHOWN);
    if (!Window)
    {
        std::cout << "[SDL_CreateWindow]: Could not create window.   : " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED + SDL_RENDERER_PRESENTVSYNC);

    if (!Renderer)
    {
        std::cout << "[SDL2]: Could not create renderer.   : " << SDL_GetError() << std::endl;
        return false;
    }

    Stage stage;

    SDL_Event e;
    bool quit = 0;
    const uint8_t* Keyboard = SDL_GetKeyboardState(0);
    std::vector<Platform> SelectedPlatforms;

    while (!quit + SDL_PollEvent(&e))
    {
        if (Keyboard[SDL_SCANCODE_Q] && Keyboard[SDL_SCANCODE_LSHIFT])
            quit = true;

        switch (e.type)
        {
            case SDL_QUIT:
                quit = true;
            break;
            case SDL_KEYDOWN:
                Keyboard = SDL_GetKeyboardState(0);
            break;
            case SDL_MOUSEBUTTONDOWN:
                if(e.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_GetMouseState(&Mouse_x, &Mouse_y);
                    if (Keyboard[SDL_SCANCODE_LSHIFT])
                    {
                        Vector2Di cell = Vector2Di((int)(Mouse_x / 40), (int)(Mouse_y / 40));
                        stage.AddPlatform(Platform(Vector2Di(cell.x * 40 + 20, cell.y * 40 + 20)));
                    }
                }

                if(e.button.button == SDL_BUTTON_RIGHT)
                {
                    SDL_GetMouseState(&Mouse_x, &Mouse_y);
                    for (int i = 0; i < stage.Platforms.size(); i++)
                    {
                        if (stage.Platforms[i].Collision(Vector2Di(Mouse_x, Mouse_y)))
                        {
                            if (!stage.Platforms[i].isSelected())
                                stage.Platforms[i].Select();
                            else
                                stage.Platforms[i].Deselect();
                        }
                            
                    }
                }
                    
            break;
        }
        
        if (Keyboard[SDL_SCANCODE_DELETE])
            stage.DeleteSelectedPlatforms();

        if (Keyboard[SDL_SCANCODE_M])
            stage.MergeSelectedIntoRect();
        

        SDL_SetRenderDrawColor(Renderer, 25, 25, 25, 255);
        SDL_RenderClear(Renderer);

        DrawGridline(40, Renderer);
        DrawCartesianAxis(Renderer);        

        for (int i = 0; stage.Platforms.size() > i; i++)
        {
            stage.Platforms[i].Render(Renderer);
        }

        SDL_RenderPresent(Renderer);
    }

    SDL_Quit();
}