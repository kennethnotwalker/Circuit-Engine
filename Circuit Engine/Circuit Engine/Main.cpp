#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <iostream>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include "MathVector.h"
#include "Node.h"
#include "vector"
#include "SDL3_ttf/SDL_ttf.h"
#include "ImageLoader.h"
#include "Complex.h"
#include "DevicePreset.h"
#include "StateHandler.h"
#include "Constants.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "imgui.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_demo.cpp"
#include "imgui_tables.cpp"
#include "imgui_impl_sdl3.cpp"
#include "imgui_impl_sdlrenderer3.cpp"

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif


using namespace std;

ImageLoader imageLoader;

vector<Device*> devices;
vector<Wire*> wires;
vector<Terminal*> forced;
bool simulating = false;
const double STEP_SIZE = 0.1;

bool KEYSDOWN[322];
bool KEYSUP[322];
bool KEYSHELD[322];

bool mouseDown, mouseJustPressed, mouseJustReleased = false;

MVector heldPosition = MVector(2, 0, 0);

StateHandler state;
DeviceLibrary deviceLib;

void createAndSolveLinearSystem(vector<Node*>& nodes, int generatorIndex, int _rows)
{
	ComplexMatrix* solver = new ComplexMatrix(_rows, _rows + 1);

	vector<int> equationIDs;
	int equationsInSolver = 0;

	for (int index = 0; index < nodes.size(); index++)
	{
		Node* node = nodes[index];
		std::vector<complex*> equations;
		bool forced = false;

		node->equationGenerator(solver, equations, equationIDs, forced, generatorIndex);

		for (int i = 0; i < equations.size(); i++)
		{
			if (equationsInSolver < solver->rows && solver->addLIRow(equationsInSolver, equations[i]))
			{
				equationsInSolver++;
			}
			delete[] equations[i];
		}

	}

	ComplexMatrix* solution = solver->RREF();

	if (generatorIndex == 0) {
		for (int i = 0; i < solution->rows; i++)
		{
			int nonzeroes = 0;
			int solutionIndex = 0;

			for (int c = 0; c < solution->rows; c++)
			{
				if (abs(solution->get(i, c)) > ERROR_MARGIN)
				{
					nonzeroes++;
				}
				if (abs(solution->get(i, c) - 1) < ERROR_MARGIN)
				{
					solutionIndex = c;
				}
			}
			if (nonzeroes == 1)
			{
				//cout << "Row " << i << ": Setting Node " << solutionIndex << " to " << solution->get(i, solution->cols - 1) << endl;
				getNodeByID(solutionIndex)->voltage = solution->get(i, solution->cols - 1);
			}
		}
	}
	if (generatorIndex == 1) {
		for (int i = 0; i < solution->rows; i++)
		{
			int nonzeroes = 0;
			int solutionIndex = 0;

			for (int c = 0; c < solution->rows; c++)
			{
				if (abs(solution->get(i, c)) > ERROR_MARGIN)
				{
					nonzeroes++;
				}
				if (abs(solution->get(i, c) - 1) < ERROR_MARGIN)
				{
					solutionIndex = c;
				}
			}
			if (nonzeroes == 1)
			{
				//cout << "Row " << i << ": Setting Node " << solutionIndex << " to " << solution->get(i, solution->cols - 1) << endl;
				getTerminalByID(solutionIndex)->current = solution->get(i, solution->cols - 1);
			}
		}
	}

	delete solution;
	delete solver;
}

void placeAsset(DevicePreset& preset, MVector pos, double rot)
{
	Device* device = new Device(pos, preset, imageLoader);
	device->rotation = rot;
	for (int i = 0; i < preset.properties.size(); i++)
	{
		device->setProperty(preset.properties[i], 1);
	}

	devices.push_back(device);
}

void assetClicked(DevicePreset& preset)
{
	state.preset = preset;
	state.mode = state.MODE_PLACE;
	state.targetRotation = 0;
}


void process(bool& running, SDL_Renderer* r, ImGuiIO& io)
{
	if (KEYSDOWN[SDL_Scancode::SDL_SCANCODE_SPACE])
	{
		simulating = !simulating;
	}

	if (KEYSDOWN[SDL_Scancode::SDL_SCANCODE_W])
	{
		state.mode = state.MODE_PLACE;
		state.preset.baseType = -1;
		state.selectedDevice = nullptr;
	}

	if (KEYSDOWN[SDL_Scancode::SDL_SCANCODE_ESCAPE])
	{
		state.mode = state.MODE_SELECT;
	}
	float mouseX, mouseY = 0;

	SDL_GetMouseState(&mouseX, &mouseY);

	MVector mousePosition(2, mouseX, mouseY);
	
	
	if (!io.WantCaptureMouse)
	{
		if (state.mode == state.MODE_PLACE)
		{
			if (state.preset.baseType == -1) {
				if (mouseJustPressed)
				{
					heldPosition = nodeSnap(mousePosition);
				}
				if (mouseDown)
				{
					SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
					SDL_FRect rect = { mouseX - 5, mouseY - 5, 10, 10 };

					MVector projectedPos = nodeSnap(mousePosition);
					SDL_RenderLine(r, heldPosition[0], heldPosition[1], projectedPos[0], projectedPos[1]);
				}
				if (mouseJustReleased)
				{
					Wire* wire = new Wire(heldPosition, nodeSnap(mousePosition));
					if (wire->connected)
					{
						wires.push_back(wire);
					}
					else
					{
						delete wire;
					}
				}
			}
			else
			{
				SDL_Texture* preview = imageLoader.getImage(state.preset.name);
				MVector previewPos = nodeSnap(mousePosition);
				double h = 100;
				double w = h * (preview->w) / (preview->h);
				SDL_FRect prect = { previewPos[0] - w / 2, previewPos[1] - h / 2, w, h };

				SDL_RenderTextureRotated(r, preview, NULL, &prect, state.targetRotation, NULL, SDL_FLIP_NONE);

				if (KEYSDOWN[SDL_Scancode::SDL_SCANCODE_R])
				{
					state.targetRotation = (int)(state.targetRotation / 90.0) * 90.0;
					state.targetRotation = ((int)(state.targetRotation + 90)) % 360;
				}

				if (mouseJustPressed)
				{
					placeAsset(state.preset, previewPos, state.targetRotation);
					state.mode = state.MODE_SELECT;
				}
			}
		}
		//cout << "checking!" << endl;
		bool deviceHasBeenSelected = false;
		for (int i = 0; i < devices.size(); i++)
		{
			Device* device = devices[i];
			SDL_FRect selectionRect = device->getSelectionRect();

			bool inX = mouseX > selectionRect.x && mouseX < selectionRect.x + selectionRect.w;
			bool inY = mouseY > selectionRect.y && mouseY < selectionRect.y + selectionRect.h;		
			
			if (inX && inY && state.mode == state.MODE_MOVE && device == state.selectedDevice)
			{
				if (mouseJustPressed)
				{
					state.dragging = true;
				}
				else if(mouseJustReleased && state.dragging)
				{
					state.dragging = false;
				}
			}

			if (!state.dragging && inX && inY && mouseJustPressed && (state.mode == state.MODE_SELECT || state.mode == state.MODE_MOVE))
			{
				state.selectedDevice = device;
				deviceHasBeenSelected = true;
				state.mode = state.MODE_MOVE;
				break;
			}
			else if (inX && inY && !mouseJustPressed)
			{
				SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
				SDL_RenderRect(r, &selectionRect);
				break;
			}
		}

		if (state.mode == state.MODE_MOVE && state.dragging && state.selectedDevice != nullptr)
		{
			state.selectedDevice->position = nodeSnap(mousePosition);
		}

		if (mouseJustPressed && state.mode == state.MODE_MOVE && !state.dragging && !deviceHasBeenSelected)
		{
			state.selectedDevice = nullptr;
			state.mode = state.MODE_SELECT;
		}
	}

	std::vector<Node*> nodes = getNodeList();
	if (simulating) {
		
		createAndSolveLinearSystem(nodes, 0, nodes.size());
		//calculate current
		//set terminal IDs
		int termIDs = 0;
		for (int d = 0; d < devices.size(); d++)
		{
			Device* device = devices[d];
			for (int termIndex = 0; termIndex < device->terminals.size(); termIndex++)
			{
				Terminal* terminal = device->terminals[termIndex];
				terminal->id = termIDs;
				terminal->foundCurrent = false;
				termIDs++;
			}
		}

		createAndSolveLinearSystem(nodes, 1, termIDs);

		//run step updates
		for (int d = 0; d < devices.size(); d++)
		{
			devices[d]->stepUpdate(10e-12);
		}

		for (int d = 0; d < devices.size(); d++)
		{
			devices[d]->storeProperties();
		}
	}

	

	for (int d = 0; d < devices.size(); d++)
	{
		devices[d]->calculateCurrent();
		devices[d]->render(r);
		

		if (state.selectedDevice == devices[d])
		{
			SDL_FRect selectionRect = devices[d]->getSelectionRect();
			SDL_SetRenderDrawColor(r, 150, 255, 150, 50);
			if (state.dragging) { SDL_SetRenderDrawColor(r, 150, 255, 255, 50); }
			SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_ADD);
			SDL_RenderFillRect(r, &selectionRect);
			SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
			SDL_RenderRect(r, &selectionRect);
		}
	}

	for (int n = 0; n < nodes.size(); n++)
	{
		Node* node = nodes[n];
		node->render(r);
	}

	for (int d = 0; d < devices.size(); d++)
	{
		Device* device = devices[d];
		device->setProperty("current", device->terminals[0]->current);
	}

	
	return;
}

void loadDevices(SDL_Renderer* renderer)
{
	//load images

	for (int i = 0; i < deviceLib.presets.size(); i++)
	{
		DevicePreset preset = deviceLib.presets[i];
		imageLoader.loadImage(renderer, preset.name, preset.imgPath);

		//imageLoader.loadImage(renderer, "symbol_R", "imgs/resistor.png");
		//imageLoader.loadImage(renderer, "symbol_V", "imgs/vsource.png");
		//imageLoader.loadImage(renderer, "symbol_G", "imgs/ground.png");
	}

	//load devices

}


int main(void)
{
	for (int i = 0; i < 322; i++)
	{
		KEYSDOWN[i] = false;
		KEYSUP[i] = false;
		KEYSHELD[i] = false;
	}

	//imgui setup


	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	bool global_running = true;

	SDL_Window* window = SDL_CreateWindow("Circuit Engine 2025", 1600, 900, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	// Setup SDL
	// [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
	{
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return -1;
	}

	// Create window with SDL_Renderer graphics context
	float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	
	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}
	
	SDL_SetRenderVSync(renderer, 1);
	if (renderer == nullptr)
	{
		SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)


	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
	//style.FontSizeBase = 20.0f;
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
	//IM_ASSERT(font != nullptr);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;
	#ifdef __EMSCRIPTEN__
		// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
		// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
		io.IniFilename = nullptr;
		EMSCRIPTEN_MAINLOOP_BEGIN
	#else

	loadDevices(renderer);

	while (!done)
#endif
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		// [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
		for (int i = 0; i < 322; i++)
		{
			KEYSDOWN[i] = false;
			KEYSUP[i] = false;
		}

		mouseJustPressed = false;
		mouseJustReleased = false;
		SDL_Event events;
		while (SDL_PollEvent(&events))
		{
			ImGui_ImplSDL3_ProcessEvent(&events);
			if (events.type == SDL_EVENT_QUIT)
				done = true;
			if (events.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && events.window.windowID == SDL_GetWindowID(window))
				done = true;

			switch (events.type)
			{
			case SDL_EVENT_QUIT:
				global_running = false;
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				mouseJustPressed = true;
				mouseDown = true;
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				if (!mouseJustPressed)
				{
					mouseJustReleased = true;
					mouseDown = false;
				}
				break;
			case SDL_EVENT_KEY_DOWN:
				if (!KEYSHELD[events.key.scancode]) { KEYSDOWN[events.key.scancode] = true; }
				KEYSHELD[events.key.scancode] = true;
				break;
			case SDL_EVENT_KEY_UP:
				if (KEYSHELD[events.key.scancode]) { KEYSUP[events.key.scancode] = true; }
				KEYSHELD[events.key.scancode] = false;
				break;
			}
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		

		// [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
		{
			SDL_Delay(10);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		                          // Create a window called "Hello, world!" and append into it.
		if (state.selectedDevice != nullptr) {
			ImGui::Begin("Properties");
			for (int p = 0; p < state.selectedDevice->propertyList.size(); p++) {
				std::string propertyName = state.selectedDevice->propertyList[p];
				//ImGui::Text(propertyName.data());    
				complex* complexRef = state.selectedDevice->getPropertyReference(propertyName);
				double* propertyRef = &(complexRef->real);
				double pmin = 0.0;
				double pmax = 100.0;
				ImGui::InputDouble(propertyName.data(), propertyRef, 1, 100);
			}

			ImGui::End();
				
		}
		

		ImGui::Begin("Global");

		ImGui::Checkbox("Simulate", &simulating);

		ImGui::Separator();

		ImGui::Checkbox("Dragging", &state.dragging);
		ImGui::InputInt("State", (int*)&state.mode);

		ImGui::End();

		if (state.mode != state.MODE_PLACE) {
			ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_MenuBar);
			if (ImGui::BeginMenuBar()) {
				for (int catIndex = 0; catIndex < deviceLib.categories.size(); catIndex++)
				{
					std::string cat = deviceLib.categories[catIndex];
					vector<DevicePreset>& presets = deviceLib.library[cat];

					if (ImGui::BeginMenu(cat.c_str())) {
						for (int i = 0; i < presets.size(); i++)
						{
							DevicePreset preset = presets[i];
							SDL_Texture* tex = imageLoader.getImage(preset.name);
							double w = 50.0;
							double h = w * (tex->h / tex->w);
							if (ImGui::ImageButton(preset.name.c_str(), (ImTextureID)(intptr_t)tex, ImVec2(w, h), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 0.9)))
							{
								assetClicked(preset);
							}
						}
						ImGui::EndMenu();
					}
				}
				ImGui::EndMenuBar();
			}
			ImGui::End();
		}

		// Rendering
		ImGui::Render();

		

		SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		SDL_RenderClear(renderer);
		process(global_running, renderer, io);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}
	#ifdef __EMSCRIPTEN__
		EMSCRIPTEN_MAINLOOP_END;
	#endif

	// Cleanup
	// [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}