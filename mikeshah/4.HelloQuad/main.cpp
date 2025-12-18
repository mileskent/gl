#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <vector>
#include <fstream>
#include <string>

int gScreenHeight = 480;
int gScreenWidth = 640;
SDL_Window * gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;
bool gQuit = false;
GLuint gVertexArrayObject = 0; // VAO
GLuint gVertexBufferObject = 0; // VBO
GLuint gVertexBufferObject2 = 0; // VBO
GLuint gGraphicsPipelineShaderProgram = 0; // Graphics pipeline handle
size_t triCount = 0;


std::string LoadShaderAsString(const std::string& filename) {
  std::string result = "";
  std::string line = "";
  std::ifstream file(filename.c_str());
  if (!file.is_open()) {
    std::cerr << "Shader load failed\n";
    exit(1);
  }
  while (std::getline(file, line)) {
    result += line + '\n';
  }
  file.close();
  return result;
}

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

void VertexSpecification () {
  // CPU land
  // note that all coordinates are in [-1,1]
  const std::vector<GLfloat> vertexPositions = {
    // x, y, z
    // t1
    -0.8f, -0.8f, 0.0f, // bl
    0.8f, -0.8f, 0.0f, // br
    -0.8f, 0.8f, 0.0f, // tl
    // t2
    -0.8f, 0.8f, 0.0f, // tl
    0.8f, 0.8f, 0.0f, // tr
    0.8f, -0.8f, 0.0f // br
  };

  // note that all coordinates are in [0,1]
  const std::vector<GLfloat> vertexColors = {
    // r, g, b
    // t1
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    // t2
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f
  };
  triCount = vertexPositions.size() / 3;
  
  // Start to set stuff up on gpu
  glGenVertexArrays(1, &gVertexArrayObject);
  glBindVertexArray(gVertexArrayObject);

  // position vbo
  glGenBuffers(1, &gVertexBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
  glBufferData(
    GL_ARRAY_BUFFER,
    vertexPositions.size() * sizeof(GLfloat),
    vertexPositions.data(), // get ptr from vec
    GL_STATIC_DRAW
  );
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0,
    3, // xyz
    GL_FLOAT,
    GL_FALSE, // normalized?
    0, // padding
    (void*) 0 // offset ptr, there is no offset
  );

  // color vbo
  glGenBuffers(1, &gVertexBufferObject2);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject2);
  glBufferData(
    GL_ARRAY_BUFFER,
    vertexColors.size() * sizeof(GLfloat),
    vertexColors.data(), // get ptr from vec
    GL_STATIC_DRAW
  );
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1,
    3, // xyz
    GL_FLOAT,
    GL_FALSE, // normalized?
    0, // padding
    (void*) 0 // offset ptr, there is no offset
  );

  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
  // glDisableVertexAttribArray(1);
}

GLuint CompileShader (GLuint type, const std::string& shaderSrc) {
  GLuint shaderObject;
  if (type == GL_VERTEX_SHADER) {
    shaderObject = glCreateShader(GL_VERTEX_SHADER);
  } else if (type == GL_FRAGMENT_SHADER) {
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }
  const char * c_src = shaderSrc.c_str();
  glShaderSource(shaderObject, 1, &c_src, nullptr);
  glCompileShader(shaderObject);
  return shaderObject;
}

GLuint CreateShaderProgram(
  const std::string& vShaderSrc,
  const std::string& fShaderSrc) {
  GLuint programObject = glCreateProgram();  
  GLuint vShader = CompileShader(GL_VERTEX_SHADER, vShaderSrc);
  GLuint fShader = CompileShader(GL_FRAGMENT_SHADER, fShaderSrc);
  glAttachShader(programObject, vShader);
  glAttachShader(programObject, fShader);
  glLinkProgram(programObject);
  glValidateProgram(programObject);
  // glDetachShader
  // glDeleteShader
  return programObject;
}

void CreateGraphicsPipeline () {
  // Vertex shader executes once per vertex
  // In charge of vertex positions
  const std::string gVertexShaderSrc = LoadShaderAsString("shaders/vert.glsl");
   
  // Fragment shader executes once per fragment
  // Determines the colors of each fragment
  const std::string gFragmentShaderSrc = LoadShaderAsString("shaders/frag.glsl");

  gGraphicsPipelineShaderProgram = CreateShaderProgram(
    gVertexShaderSrc, gFragmentShaderSrc
  ); 

  glUseProgram(gGraphicsPipelineShaderProgram);
  glUniform2f(glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_resolution"), gScreenWidth, gScreenHeight);
  glUseProgram(0);
}

void Input () {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      gQuit = true;
    }
  }
}

// Setting OpenGL state
void PreDraw () {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glViewport(0,0, gScreenWidth, gScreenHeight);
  glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glUseProgram(gGraphicsPipelineShaderProgram);
}

void Draw () {
  glBindVertexArray(gVertexArrayObject);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
  glDrawArrays(GL_TRIANGLES, 0, triCount);
}

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
  VertexSpecification();
  CreateGraphicsPipeline();
  MainLoop();
  CleanUp();
  return 0;
}
