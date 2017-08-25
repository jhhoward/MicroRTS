#pragma once

#include <SDL.h>

class DebugMapWindow
{
public:
	DebugMapWindow();
	~DebugMapWindow();
	
	void Update();

private:	
	SDL_Window* m_Window;
	SDL_Renderer* m_Renderer;
	SDL_Surface* m_Surface;
	SDL_Texture* m_Texture;
};