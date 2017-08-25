#include "DebugMapWindow.h"
#include "src/System.h"
#include "src/Game.h"
#include "src/Map.h"
#include "src/Resource.h"
#include "src/Fog.h"

int ZoomFactor = 3;

void PutPixel(SDL_Surface *surface, int x, int y, int red, int green, int blue)
{
	Uint32 pixel = SDL_MapRGBA(surface->format, red, green, blue, 255);

    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    *(Uint32 *)p = pixel;
}

DebugMapWindow::DebugMapWindow()
{
	SDL_CreateWindowAndRenderer( MAP_SIZE * ZoomFactor, MAP_SIZE * ZoomFactor, 0, &m_Window, &m_Renderer );
	SDL_RenderSetLogicalSize(m_Renderer, MAP_SIZE, MAP_SIZE);
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	m_Surface = SDL_CreateRGBSurface(0, MAP_SIZE, MAP_SIZE, 32, 
											0x000000ff,
											0x0000ff00, 
											0x00ff0000, 
											0xff000000
											);
	m_Texture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, m_Surface->w, m_Surface->h);

}

DebugMapWindow::~DebugMapWindow()
{
	
}

void DebugMapWindow::Update()
{
	SDL_SetRenderDrawColor ( m_Renderer, 0, 0, 0, 255 );
	SDL_RenderClear ( m_Renderer );

	for(int y = 0; y < MAP_SIZE; y++)
	{
		for(int x = 0; x < MAP_SIZE; x++)
		{
			if(Fog_IsRevealed(0, x, y) || Fog_IsRevealed(1, x, y))
			{
				switch(Map_GetTileType(x, y))
				{
					case 0:
					default:
					PutPixel(m_Surface, x, y, 96, 128, 96);
					break;
				}
			}
			else
			{
				switch(Map_GetTileType(x, y))
				{
					case 0:
					default:
					PutPixel(m_Surface, x, y, 48, 64, 48);
					break;
				}
			}
		}
	}
	
	for(int n = 0; n < MAX_UNITS; n++)
	{
		Unit* unit = &Game.Units[n];
		if(unit->type != UnitType_Invalid)
		{
			if(unit->hp == 0)
			{
				//PutPixel(m_Surface, unit->agent.x, unit->agent.y, 8, 8, 8);
			}
			else if(unit->team)
			{
				PutPixel(m_Surface, unit->agent.x, unit->agent.y, 32, 32, 192);
			}
			else
			{
				PutPixel(m_Surface, unit->agent.x, unit->agent.y, 192, 32, 32);
			}
		}
	}
	
	for(int n = 0; n < MAX_BUILDINGS; n++)
	{
		Building* building = &Game.Buildings[n];
		if(building->type != BuildingType_Invalid)
		{
			const BuildingTypeInfo* typeInfo = &AllBuildingTypeInfo[building->type];
			
			for(int i = 0; i < typeInfo->width; i++)
			{
				for(int j = 0; j < typeInfo->height; j++)
				{
					if(building->hp == 0)
					{
					//	PutPixel(m_Surface, building->x + i, building->y + j, 8, 8, 8);
					}
					else if(building->team)
					{
						PutPixel(m_Surface, building->x + i, building->y + j, 32, 32, 96);
					}
					else
					{
						PutPixel(m_Surface, building->x + i, building->y + j, 96, 32, 32);
					}
				}
			}
		}
	}
	
	for(int n = 0; n < MAX_RESOURCES; n++)
	{
		EntityID id;
		id.type = Entity_Resource;
		id.id = n;
		
		uint8_t x, y;
		Resource_GetLocation(id, &x, &y);

		uint8_t remaining = Resource_GetRemaining(id);
		
		if(remaining > 0)
		{
			PutPixel(m_Surface, x, y, 128 + 8 * remaining, 128 + 8 * remaining, 32);
		}
	}
	
	SDL_UpdateTexture(m_Texture, NULL, m_Surface->pixels, m_Surface->pitch);
	SDL_RenderCopy(m_Renderer, m_Texture, NULL, NULL);
	SDL_RenderPresent(m_Renderer);
}
