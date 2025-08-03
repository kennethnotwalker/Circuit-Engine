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

Device* selectedDevice = nullptr;

void process(bool& running, SDL_Renderer* r)
{
	if (KEYSDOWN[SDL_Scancode::SDL_SCANCODE_SPACE])
	{
		simulating = !simulating;
	}
	float mouseX, mouseY = 0;

	SDL_GetMouseState(&mouseX, &mouseY);

	MVector mousePosition(2, mouseX, mouseY);
	if (mouseJustPressed)
	{
		heldPosition = mousePosition;
	}
	if (mouseDown && false)
	{
		SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
		SDL_FRect rect = { mouseX - 5, mouseY - 5, 10, 10 };

		MVector projectedPos = Wire::snap(heldPosition, mousePosition);
		SDL_RenderLine(r, heldPosition[0], heldPosition[1], projectedPos[0], projectedPos[1]);
	}
	if (mouseJustReleased && false)
	{
		Wire* wire = new Wire(heldPosition, Wire::snap(heldPosition, mousePosition));
		if (wire->connected)
		{
			wires.push_back(wire);
		}
		else
		{
			delete wire;
		}
	}
	if (true)
	{
		//cout << "checking!" << endl;
		for (int i = 0; i < devices.size(); i++)
		{
			Device* device = devices[i];
			double _w = 100;
			double _h = _w*device->texture->h/device->texture->w;
			double _a = device->rotation*3.1415926535/180.0;
			//rotate
			double w = abs(_w * cos(_a)) + abs(_h * sin(_a));
			double h = abs(_w * sin(_a)) + abs(_h * cos(_a));

			SDL_FRect selectionRect = { (float)(device->position[0] - w / 2),(float)(device->position[1] - h / 2), w, h };
			bool inX = mouseX > selectionRect.x && mouseX < selectionRect.x + selectionRect.w;
			bool inY = mouseY > selectionRect.y && mouseY < selectionRect.y + selectionRect.h;		
			
			if (inX && inY && mouseJustPressed)
			{
				selectedDevice = device;
				break;
			}
		}
	}

	std::vector<Node*> nodes = getNodeList();
	if (simulating) {
		ComplexMatrix* solver = new ComplexMatrix(nodes.size(), nodes.size() + 1);

		vector<int> equationIDs;
		int equationsInSolver = 0;

		for (int index = 0; index < nodes.size(); index++)
		{
			Node* node = nodes[index];
			std::vector<complex*> equations;
			bool forced = false;

			node->generateEquations(solver, equations, equationIDs, forced);

			for (int i = 0; i < equations.size(); i++)
			{
				if (equationsInSolver < solver->rows && solver->addLIRow(equationsInSolver, equations[i]))
				{
					equationsInSolver++;
				}
				delete[] equations[i];
			}

		}

		solver->print();

		ComplexMatrix* solution = solver->RREF();
		cout << "solved: " << endl;
		solution->print();

		for (int i = 0; i < solution->rows; i++)
		{
			int nonzeroes = 0;
			int solutionIndex = 0;

			for (int c = 0; c < solution->rows; c++)
			{
				if (abs(solution->get(i, c)) > 0.000001)
				{
					nonzeroes++;
				}
				if (abs(solution->get(i, c) - 1) < 0.00001)
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

		delete solution;
		delete solver;
	}

	for (int d = 0; d < devices.size(); d++)
	{
		devices[d]->render(r);
	}

	for (int n = 0; n < nodes.size(); n++)
	{
		Node* node = nodes[n];
		node->render(r);
	}

	
	return;
}

void loadDevices(SDL_Renderer* renderer)
{
	//load images
	imageLoader.loadImage(renderer, "symbol_R", "imgs/resistor.png");
	imageLoader.loadImage(renderer, "symbol_V", "imgs/vsource.png");
	imageLoader.loadImage(renderer, "symbol_G", "imgs/ground.png");

	//load devices
	Device* g = new Device(MVector(2, 640.0, 400.0), 0, 1, "symbol_G", imageLoader); g->rotation = 180;
	Device* v = new Device(g->position + MVector(2, 0, -100.0), 2, 2, "symbol_V", imageLoader); v->setProperty("voltage", 10 + 7 * _i);
	Device* r = new Device(v->position + MVector(2, 50.0, -50.0), 1, 2, "symbol_R", imageLoader); r->rotation = 90; r->setProperty("resistance", 5);
	Device* r2 = new Device(r->position + MVector(2, 100.0, 0.0), 1, 2, "symbol_R", imageLoader); r2->rotation = 90; r2->setProperty("resistance", 10);
	Device* r3 = new Device(r->position + MVector(2, 50.0, -50.0), 1, 2, "symbol_R", imageLoader); r3->rotation = 0; r3->setProperty("resistance", 15);
	Device* g2 = new Device(r3->position + MVector(2, 0.0, -100.0), 0, 1, "symbol_G", imageLoader); g2->rotation = 0;
	Device* v2 = new Device(r2->position + MVector(2, 50.0, 50.0), 2, 2, "symbol_V", imageLoader); v2->setProperty("voltage", 15);

	connectJunction(g->terminals[0], v->terminals[0]);
	connectJunction(v->terminals[1], r->terminals[0]);
	connectJunction(r->terminals[1], r2->terminals[0]);
	connectJunction(r->terminals[1], r3->terminals[0]);
	connectJunction(r3->terminals[1], g2->terminals[0]);
	connectJunction(r2->terminals[1], v2->terminals[1]);
	connectJunction(v2->terminals[0], g->terminals[0]);

	devices.push_back((Device*)g);
	devices.push_back((Device*)g2);
	devices.push_back((Device*)v);
	devices.push_back((Device*)v2);
	devices.push_back((Device*)r);
	devices.push_back((Device*)r2);
	devices.push_back((Device*)r3);
}


int main(void)
{
	for (int i = 0; i < 322; i++)
	{
		KEYSDOWN[i] = false;
		KEYSUP[i] = false;
		KEYSHELD[i] = false;
	}

	complex a = 3 + 5*_i;

	cout << a << endl;

	//imgui setup


	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	bool global_running = true;

	SDL_Window* window = SDL_CreateWindow("Circuit Engine 2025", 1280, 720, 0);
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

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Properties");                          // Create a window called "Hello, world!" and append into it.
			if (selectedDevice != nullptr) {
				for (int p = 0; p < selectedDevice->propertyList.size(); p++) {
					std::string propertyName = selectedDevice->propertyList[p];
					//ImGui::Text(propertyName.data());    
					complex* complexRef = selectedDevice->getPropertyReference(propertyName);
					double* propertyRef = &(complexRef->real);
					double pmin = 0.0;
					double pmax = 100.0;
					ImGui::SliderScalar(propertyName.data(), ImGuiDataType_Double, propertyRef, &pmin, &pmax);
				}
				
			}
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();

		

		SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		SDL_RenderClear(renderer);
		process(global_running, renderer);
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