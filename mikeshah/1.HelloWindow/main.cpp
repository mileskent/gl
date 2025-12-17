#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>

int gScreenHeight = 480;
int gScreenWidth = 640;
SDL_Window * gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;
bool gQuit = false;

void GetGLInfo() {
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void InitializeProgram() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL Fail to Init Vid Subsystem!\n";
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  gGraphicsApplicationWindow = SDL_CreateWindow("Window Title", 
      0, 
      0, 
      gScreenWidth,
      gScreenHeight,
      SDL_WINDOW_OPENGL
  );
  if (!gGraphicsApplicationWindow) {
    std::cerr << "SDL_Window not Created!\n";
    exit(1);
  }

  gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
  if (!gOpenGLContext) {
    std::cerr << "OpenGL Context Not Available!\n";
    exit(1);
  }

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    std::cerr << "Glad not Initialized!\n";
    exit(1);
  }

  GetGLInfo();
}

void Input () {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      gQuit = true;
    }
  }
}

void PreDraw () {}

void Draw () {}

void UpdateScreen () {
  SDL_GL_SwapWindow(gGraphicsApplicationWindow);
}

void MainLoop() {
  while (!gQuit)  {
    Input();
    PreDraw();
    Draw();
    UpdateScreen();
  }
}

void CleanUp() {
  SDL_DestroyWindow(gGraphicsApplicationWindow);
  SDL_Quit();
}

int main (void) {
  InitializeProgram();
  MainLoop();
  CleanUp();
  return 0;
}
