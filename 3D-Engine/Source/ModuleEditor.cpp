#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"

#include "Time.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <stdio.h>
#include "SDL/include/SDL.h"
//#include <shellapi.h>

#include "MathGeoLib/include/Algorithm/Random/LCG.h"


#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL/include/SDL_opengles2.h>
#else
#include <SDL/include/SDL_opengl.h>
#endif

#include <string>
#include <xstring>


#ifdef _DEBUG
#pragma comment( lib, "Source/External/MathGeoLib/libx86/MGLDebug/MathGeoLib.lib")
#else
#pragma comment (lib, "Source/External/MathGeoLib/libx86/MGLRelease/MathGeoLib.lib")
#endif

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

// Destructor
ModuleEditor::~ModuleEditor()
{
	//logs.clear();
	login.clear();

}

// Called before render is available
bool ModuleEditor::Init()
{
	LOG("Init ImGui Module");

	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	//////////SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	//////////SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//////////SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//////////SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

	////////// Create window with graphics context
	////////SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	////////SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	////////SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	/*SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);*/
	gl_context = SDL_GL_CreateContext(App->window->window);
	SDL_GL_MakeCurrent(App->window->window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);
	App->scene->LoadScene();
	
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	return ret;
}

// Called every update
update_status ModuleEditor::Update(float dt)
{
	

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();



	ImGuiIO& io = ImGui::GetIO(); (void)io;





	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Quitting ImGui Module");
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	login.clear();
	//logs.clear();

	return true;
}

bool ModuleEditor::DrawMenuBar()
{
	
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("Close all windows"))
			{
				show_demo_window = false;
			}

			

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{

			if (ImGui::MenuItem("Console"))
			{
				show_console_window = true;
			}

			if (ImGui::MenuItem("Modules"))
			{
				show_settings_window = true;
			}

			if (ImGui::MenuItem("Performance"))
			{
				show_performance_window = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{

			if (ImGui::MenuItem("About"))
			{
				show_about_window = true;
			}

			// End Drawing Help Menu:
			ImGui::EndMenu();
		}

		

		ImGui::EndMainMenuBar();
	}

	return true;
}

void ModuleEditor::AddLogs(const char* text)
{
	// logs.appendf(text);

	login.push_back(text);
	scroll = true;

}

void ModuleEditor::OpenWeb(const char* url)const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);

}

void ModuleEditor::ConsoleWindow()
{
	std::string* e = nullptr;

	if (ImGui::Begin("Console", &show_console_window)) {

		// ImGui::Text(logs.begin());
		for (unsigned int i = 0; i < login.size(); ++i) {
			e = &login[i];

			ImGui::Text(e->c_str());
		}
		if (scroll) ImGui::SetScrollHereY(1.0f), scroll = false;
	}
	
	LOG("%i", a);
	a++;

	ImGui::End();
}

void ModuleEditor::Render3DWindow() {

	ImGui::Begin("Scene View", &show_render3d_window);
	


	/*Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();*/

	/*int renderX = ImGui::GetWindowPos().x;
	int renderY = ImGui::GetWindowPos().y;
	int renderW = ImGui::GetWindowWidth();
	int renderH = ImGui::GetWindowHeight();*/
	//App->renderer3D->OnResize(renderX, -renderY + renderH, renderW, renderH);

	//SDL_GetWindowSize(App->window->window, &winWidth, &winHeight);
	//App->renderer3D->OnResize(0, 0, winWidth, winHeight);

	ImGui::End();
}

void ModuleEditor::PerformanceWindow()
{
	if (!show_performance_window)
	{
		if (!fps_info.empty())
		{
			ms_info.clear();
		}

		if (!ms_info.empty())
		{
			ms_info.clear();
		}

		return;
	}

	if (fps_info.size() == TIMER_BUFFER_LENGTH)
	{
		fps_info.erase(fps_info.begin());
	}
	fps_info.push_back(Time->FPS());

	if (ms_info.size() == TIMER_BUFFER_LENGTH)
	{
		ms_info.erase(ms_info.begin());
	}
	ms_info.push_back(Time->DeltaTimeMs());

	ImGui::Begin("Performance", &show_performance_window);

	char title[25];

	sprintf_s(title, 25, "Framerate %.1f", fps_info.back());
	ImGui::PlotHistogram("##framerate", &fps_info.front(), fps_info.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	sprintf_s(title, 25, "Milliseconds %.1f", ms_info.back());
	ImGui::PlotHistogram("##milliseconds", &ms_info.front(), ms_info.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	ImGui::Text("\n");
	ImGui::Text("Hardware");
	//App->renderer->OnPerformanceWindow();

	ImGui::End();
}
void ModuleEditor::ConfigWindow() {

	
}

void ModuleEditor::AboutWindow() {

	if (ImGui::Begin("About"))
	{
		ImGui::Text("App name:");
		ImGui::SameLine();
		ImGui::TextColored({ 0,255,232,1 }, "%s", TITLE);
		ImGui::Text("");

		ImGui::Text("By:");
		if (ImGui::MenuItem("Albert Martin Lucena"))
		{
			OpenWeb("https://github.com/T4skar");
		}
		
		ImGui::Text("");

		ImGui::TextWrapped("Chopedd is love choped is live");
		ImGui::Text("");

		
		ImGui::Text("");

		ImGui::Text("3rd Party Libreries Used:");
		ImGui::TextWrapped(LIBRARIES_USED);
		//ImGui::ColorButton();
		ImGui::Text("License:");
		ImGui::SameLine();
		ImGui::TextColored({ 0,255,232,1 }, "MIT License");
		ImGui::Text("");

		ImGui::Text("Copyright (c) 2022 T4skar");
		ImGui::Text("");
		ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copyof this software and associated documentation files(the 'Software'), to dealin the Software without restriction, including without limitation the rightsto use, copy, modify, merge, publish, distribute, sublicense, and /or sellcopies of the Software, and to permit persons to whom the Software isfurnished to do so, subject to the following conditions : ");
		ImGui::Text("");
		ImGui::TextWrapped({ "The above copyright notice and this permission notice shall be included in all"
			"copies or substantial portions of the Software." });
		ImGui::Text("");
		ImGui::TextWrapped({"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
			"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
			"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE"
			"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
			"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
			"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE"
			"SOFTWARE."});
	}
	ImGui::End();
}

void ModuleEditor::Draw() {

	DrawMenuBar();

	if (show_console_window) {

		ConsoleWindow();
	}

	if (show_demo_window) {
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	if (show_render3d_window) {

		Render3DWindow();
	}

	if (show_config_window) {

		ConfigWindow();
	}

	if (show_about_window) {

		AboutWindow();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::BarFile() {
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New"))
		{

		}
		if (ImGui::MenuItem("Save"))
		{

			App->scene->SaveScene();
		}
		if (ImGui::MenuItem("Load"))
		{

			App->scene->LoadScene();
		}
		if (ImGui::MenuItem("Close"))
		{
			close = true;

		}
		ImGui::EndMenu();
	}
}

void ModuleEditor::BarWindows() {
	if (ImGui::BeginMenu("Windows"))
	{
		if (ImGui::Checkbox("Console Window", &show_console_window))
		{

		}
		if (ImGui::Checkbox("Demo Window", &show_demo_window))
		{

		}
		if (ImGui::Checkbox("Scene View Window", &show_render3d_window))
		{

		}
		if (ImGui::Checkbox("Configuration", &show_config_window))
		{

		}
		if (ImGui::Checkbox("About", &show_about_window))
		{

		}


		ImGui::EndMenu();
	}
	
}
void ModuleEditor::BarXXX() {
	if (ImGui::Begin("Windowads")) {
		ImGui::Text("Options");
		if (ImGui::CollapsingHeader("APP"))
		{

			ImGui::Text("App name:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%s", TITLE);

			ImGui::Text("Organization:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%s", ORGANIZATION);
		}
		if (ImGui::CollapsingHeader("Window"))
		{

			ImGui::Text("Brightness:");
			ImGui::SameLine();
			float brigth = SDL_GetWindowBrightness(App->window->window);
			if (ImGui::SliderFloat("Brightness", &brigth, 0, 1))
			{
				SDL_SetWindowBrightness(App->window->window, brigth);
			}

			ImGui::Text("Width:");
			ImGui::SameLine();
			float widht = App->window->screenWidth;
			if (ImGui::SliderFloat("Width", &widht, 20, 1000))
			{
				SDL_SetWindowSize(App->window->window, widht, App->window->screenHeight);
				// SDL_SetWindowBrightness(App->window->window, brigth);
			}
			ImGui::Text("Height:");
			ImGui::SameLine();
			float height = App->window->screenHeight;
			if (ImGui::SliderFloat("Height", &height, 20, 1000))
			{
				SDL_SetWindowSize(App->window->window, App->window->screenWidth, height);
				// SDL_SetWindowBrightness(App->window->window, brigth);
			}
		}
		if (ImGui::CollapsingHeader("Hardware"))
		{
			SDL_version compiled;
			SDL_version linked;

			SDL_VERSION(&compiled);
			SDL_GetVersion(&linked);

			const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
			const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
		   // const GLubyte* vram = glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX); // Returns a hint to the model

			//const GLubyte* version = glGetString(GL_VERSION); // Returns a hint to the model



			ImGui::Text("SDL Version: ");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%u.%u.%u", compiled.major, compiled.minor, compiled.patch);
			ImGui::Separator();//--------------
			ImGui::Text("CPUs:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%d (cache %i Kb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
			ImGui::Text("System RAM:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%f Gb", (float)SDL_GetSystemRAM());
			ImGui::Text("Caps:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%s %s", SDL_Has3DNow() ? "3DNow" : "", SDL_HasAltiVec() ? "AltiVec" : "");
			ImGui::Separator();//--------------
			ImGui::Text("GPU:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "vendor %s device %s", vendor);
			ImGui::Text("Brand:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%s", renderer);
			ImGui::Text("VRAM Budget:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%i", 1);
			ImGui::Text("VRAM Usage:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%s", ORGANIZATION);
			ImGui::Text("VRAM Avaliable:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%s", ORGANIZATION);
			ImGui::Text("VRAM Reserved:");
			ImGui::SameLine();
			ImGui::TextColored({ 255,0,0,1 }, "%s", ORGANIZATION);
		}



	}

	ImGui::End();
}

