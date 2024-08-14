//SDL
#include "SDL_render.h"
#include "SDL_scancode.h"
#include "SDL_video.h"
#include <SDL.h>
//STL
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
//vendor
#include <SDL_prims.h>
#undef main

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

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
    NOT_YET = 0,
    STONE = 1,
    GLASS = 2,
    CLOUD = 3
};

enum PlatformType
{
    STATIC = 0,
    ANCHOR = 1
};

Vector2D SDLBox2D(const Vector2D& vec2)
{
   return Vector2D(vec2.x / 80 - 8, -(vec2.y / 80 - 4.5f));
}

float SDLBox2Df(const float& f)
{
   return f / 80;
}

int Dist(const Vector2Di& p1, const Vector2Di& p2)
{
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

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
    Material Mat;

public:
    Platform(const Vector2Di& center, const int& type = PlatformType::STATIC)
        : StartPos({center.x - 20, center.y - 20}), Width(40), Height(40), Type(type), ID(CreatePlatformID()), Selected(0), Mat(Material::NOT_YET)
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

    Platform(const std::vector<SDL_Point>& verteces, const int& type = PlatformType::STATIC)
        : Selected(0), ID(CreatePlatformID()), Type(type)
    {
        Vector2Di UpperBound = Vector2Di(verteces[0].x, verteces[0].y);
        Vector2Di LowerBound = Vector2Di(verteces[0].x, verteces[0].y);

        for (int i = 0; i < verteces.size(); ++i)
        {
            SDLVerteces.push_back(verteces[i]);

            if (verteces[i].x < LowerBound.x || verteces[i].y < LowerBound.y)
                LowerBound = Vector2Di(verteces[i].x, verteces[i].y);
            if (verteces[i].x > UpperBound.x || verteces[i].y > UpperBound.y)
                UpperBound = Vector2Di(verteces[i].x, verteces[i].y);
        }

        StartPos = LowerBound;
        Width = Dist(LowerBound, Vector2Di(UpperBound.x, LowerBound.y));
        Height = Dist(UpperBound, Vector2Di(UpperBound.x, LowerBound.y));
    }

    void Render(SDL_Renderer* renderer)
    {
        SDL_Color color(1, 1, 1, 1);
        if (Selected)
            color = SDL_Color(0, 1, 0, 1);
        else if (Type == PlatformType::ANCHOR)
            color = SDL_Color(1, 0, 1, 1);

        SDL_DrawPolygon(renderer, SDLVerteces.data(), SDLVerteces.size(), color);
    }

    bool Collision(const Vector2Di& p)
    {
        if (p.x >= StartPos.x && p.x <= StartPos.x + Width && p.y >= StartPos.y && p.y <= StartPos.y + Height)
            return true;
        else
            return false;
    }

    void Select()
    {
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

    std::vector<SDL_Point> GetVerteces()
    {
        return SDLVerteces;
    }

    void SetType(const PlatformType& type)
    {
        Type = type;
    }

    int GetType()
    {
        return Type;
    }

    int GetWidth()
    {
        return Width;
    }

    int GetHeight()
    {
        return Height;
    }

    Material GetMaterial()
    {
        return Mat;
    }

    friend bool operator==(const Platform& plat1, const Platform& plat2)
    {
        return plat1.ID == plat2.ID;
    }
};

struct Box2DPlatform
{
    Vector2D* Verteces;
    Vector2D Position;
    std::vector<Vector2D> Box2DVerteces;
    unsigned int nVerteces;
    unsigned int Type;
    float Width;
    float Height;
    Material Mat;

    Box2DPlatform(Platform& platform)
    {
        std::vector<SDL_Point> SDLVerteces = platform.GetVerteces();
        //[!] There is no check being done to ensure that the verteces are counter-clockwise! [!]
        Type = platform.GetType();
        

        nVerteces = SDLVerteces.size();
        if (SDLVerteces.size() == 4)
        {
            Verteces = nullptr;
            int PosX = (SDLVerteces[0].x + SDLVerteces[3].x) / 2;
            int PosY = (SDLVerteces[0].y + SDLVerteces[1].y) / 2;
            Position = SDLBox2D(Vector2D(PosX, PosY));
            Width = SDLBox2Df(Dist({SDLVerteces[0].x, SDLVerteces[0].y}, {SDLVerteces[3].x, SDLVerteces[3].y}));
            Height = SDLBox2Df(Dist({SDLVerteces[0].x, SDLVerteces[0].y}, {SDLVerteces[1].x, SDLVerteces[1].y}));
        }
        
        else
        {
            for (int i = 0; i < SDLVerteces.size(); ++i)
                Box2DVerteces.push_back(SDLBox2D({(float)SDLVerteces[i].x, (float)SDLVerteces[i].y}));
        
            Verteces = Box2DVerteces.data();
        }
        
        
        Mat = platform.GetMaterial();
    }
};

struct Screen
{
    Vector2D StartPosition;
    Box2DPlatform* Platforms; //[!]First platform is always an anchor[!]
    //Track Music;
    //Background Background;

    Screen(const Vector2D& startPos, Box2DPlatform* platforms)
        : StartPosition(startPos), Platforms(platforms) {}
};

class Stage
{
public:
    std::vector<Platform> Platforms;
    Vector2Di StartPosition;
    std::vector<SDL_Point> EdgeQueue;
    
    std::vector<Screen> StageData;
public:
    void AddPlatform(const Platform& platform)
    {
        Platforms.push_back(platform);
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

    void SetStartPosition(const Vector2Di& mouse)
    {
        StartPosition = Vector2Di(div(mouse.x, 40).quot * 40, div(mouse.y, 40).quot * 40);
        std::cout << "[INFO] Player start position placed at: " << StartPosition.x << " | " << StartPosition.y << '\n';
    }

    void AddEdge(const SDL_Point& vec2)
    {
        EdgeQueue.push_back(vec2);
        std::cout << "[INFO] Vec2 at x: " << vec2.x << " and y: " << vec2.y << " added to queue" << '\n';
    }

    void RenderEdges(SDL_Renderer* renderer)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawPoints(renderer, EdgeQueue.data(), EdgeQueue.size());
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    }

    void ExportScreen()
    {
        std::cout << "Exporting Level Geometry..." << '\n';
        
        std::vector<Box2DPlatform> PlatformsToExport;

        std::cout << "[INFO] Exporting " << Platforms.size() << " Platforms" << '\n';
        for (int i = 0; i < Platforms.size(); ++i)
        {
            PlatformsToExport.push_back(Box2DPlatform(Platforms[i]));
            std::cout << "Platform " << i + 1 << "->" << '\n';
            if (PlatformsToExport[i].nVerteces == 4)
                std::cout << "Rectangle : " <<  PlatformsToExport[i].Position.x << " | " << PlatformsToExport[i].Position.y << '\n';
            else
                for (int j = 0; j < PlatformsToExport[i].nVerteces; ++j)
                {
                    std::cout << "Convex Hull : " << PlatformsToExport[i].Verteces[j].x << " | " << PlatformsToExport[i].Verteces[j].y << '\n';
                }
        } 

        for (int i = 0; i < PlatformsToExport.size(); ++i)
            {
                if (PlatformsToExport[i].Type == PlatformType::ANCHOR) 
                {
                    for (auto it = PlatformsToExport.begin(), lim = PlatformsToExport.end(); it != lim; ++it) 
                    {
                        if (it->Type == PlatformType::ANCHOR) 
                            std::rotate(PlatformsToExport.begin(), it, it + 1);
                    }
                }   
            }

        StageData.push_back(Screen(SDLBox2D({(float)StartPosition.x, (float)StartPosition.y}), PlatformsToExport.data()));

        Platforms.clear();
        EdgeQueue.clear();
    }

    void ExportToFile()
    {

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
        return 0;
    }

    SDL_Window* Window = SDL_CreateWindow("not_yet_level_editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_SHOWN);
    if (!Window)
    {
        std::cout << "[SDL_CreateWindow]: Could not create window.   : " << SDL_GetError() << std::endl;
        return 0;
    }

    SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED + SDL_RENDERER_PRESENTVSYNC);

    if (!Renderer)
    {
        std::cout << "[SDL2]: Could not create renderer.   : " << SDL_GetError() << std::endl;
        return 0;
    }

    Stage stage;

    SDL_Event e;
    bool quit = 0;
    const uint8_t* Keyboard = SDL_GetKeyboardState(0);
    std::vector<Platform> SelectedPlatforms;

    while (!quit + SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            quit = true;
        else if (e.type == SDL_KEYDOWN)
        {
            Keyboard = SDL_GetKeyboardState(0);
            continue;
        }

        else if (e.type == SDL_KEYUP)
        {
            Keyboard = SDL_GetKeyboardState(0);
            if (!Keyboard[SDL_SCANCODE_LCTRL])
            {
                stage.EdgeQueue.clear();
            }
            continue;
        }
             
        else if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            if(e.button.button == SDL_BUTTON_LEFT)
            {
                SDL_GetMouseState(&Mouse_x, &Mouse_y);
                if (Keyboard[SDL_SCANCODE_LSHIFT])
                {
                    Vector2Di cell = Vector2Di((int)(Mouse_x / 40), (int)(Mouse_y / 40));
                    stage.AddPlatform(Platform(Vector2Di(cell.x * 40 + 20, cell.y * 40 + 20)));
                }

                else if (Keyboard[SDL_SCANCODE_S])
                    stage.SetStartPosition(Vector2Di(Mouse_x, Mouse_y));
                
                else if (Keyboard[SDL_SCANCODE_LCTRL])
                {
                    Vector2Di point = Vector2Di((int)(Mouse_x / 40), (int)(Mouse_y / 40));
                    stage.AddEdge(SDL_Point(point.x * 40, point.y * 40));
                }
            }
            
            else if(e.button.button == SDL_BUTTON_RIGHT)
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
        }

        else if (Keyboard[SDL_SCANCODE_DELETE])
        {
            stage.DeleteSelectedPlatforms();
        }
            
        else if (Keyboard[SDL_SCANCODE_C] && Keyboard[SDL_SCANCODE_LCTRL])
        {
            if (!stage.EdgeQueue.empty())
            {
                stage.AddPlatform(Platform(stage.EdgeQueue));
                stage.EdgeQueue.clear();
            }
        }
           
        else if (Keyboard[SDL_SCANCODE_A])
        {
            for (int i = 0; i < stage.Platforms.size(); ++i)
            {
                if (stage.Platforms[i].isSelected())
                    stage.Platforms[i].SetType(PlatformType::ANCHOR);
            }
        }

        else if (Keyboard[SDL_SCANCODE_E])
        {
            if (!stage.Platforms.empty())
                stage.ExportScreen();
        }
            
            

        SDL_SetRenderDrawColor(Renderer, 25, 25, 25, 255);
        SDL_RenderClear(Renderer);

        DrawGridline(40, Renderer);
        DrawCartesianAxis(Renderer);        

        for (int i = 0; stage.Platforms.size() > i; i++)
        {
            stage.Platforms[i].Render(Renderer);
        }

        stage.RenderEdges(Renderer);
        SDL_RenderPresent(Renderer);
    }

    SDL_Quit();
}