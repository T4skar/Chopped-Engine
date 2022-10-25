#ifndef __MODULEGUI_H__
#define __MODULEGUI_H__



#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <stdio.h>
#include "SDL/include/SDL.h"
#include <vector>

#include <string.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL/include/SDL_opengles2.h>
#else
#include <SDL/include/SDL_opengl.h>
#endif

#include "MathGeoLib/include/Algorithm/Random/LCG.h"
//
//class ModuleGUI : public Module
//{
//	ModuleGUI(Application* app, bool start_enabled = true);
//	//DESTRUCTOR
//	~ModuleGUI();
//};
//



#endif // !__MODULEGUI_H__