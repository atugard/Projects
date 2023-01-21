#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "./common/shader_utils.h"
#include <glm/gtc/type_ptr.hpp>

//global vars
GLint uniform_m_transform;
GLuint program;
GLuint vbo_triangle, vbo_triangle_colors;
GLint attribute_coord3d, attribute_v_color;
GLint uniform_fade;

struct attributes {
  GLfloat coord3d[3];
  GLfloat v_color[3];
};
using namespace std;

bool init_resources(){
  struct attributes triangle_attributes[] = {
    {{ 0.0,  0.8, 0.0}, {1.0, 1.0, 0.0}},
    {{-0.8, -0.8, 0.0}, {0.0, 0.0, 1.0}},
    {{ 0.8, -0.8, 0.0}, {1.0, 0.0, 0.0}}
  };
  

  glGenBuffers(1, &vbo_triangle);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_attributes), triangle_attributes, GL_STATIC_DRAW);

  //define vertex and fragment shaders
  GLuint vs, fs;
  if ((vs = create_shader("triangle.v.glsl", GL_VERTEX_SHADER))   == 0) return false;
  if ((fs = create_shader("triangle.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false;  

  //link program
  GLint link_ok = GL_FALSE;
  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    cerr << "Error in glLinkProgram" << endl;
    return false;
  }
  
  //bind uniform vars
  // const char* uniform_name_1;
  // uniform_name_1 = "fade";
  // uniform_fade = glGetUniformLocation(program, uniform_name_1);
  // if (uniform_fade == -1) {
  //   cerr << "Could not bind uniform " << uniform_name_1 << endl;
  //   return false;
  // }
  const char* uniform_name_2;
  uniform_name_2 = "m_transform";
  uniform_m_transform = glGetUniformLocation(program, uniform_name_2);
  if (uniform_m_transform == -1) {
    cerr << "Could not bind uniform " << uniform_name_2 << endl;
    return false;
  }
  
  //bind attributes
  const char* attribute_name = "coord3d";
  attribute_coord3d = glGetAttribLocation(program, attribute_name);
  if (attribute_coord3d == -1) {
    cerr << "Could not bind attribute " << attribute_name << endl;
    return false;
  }
  attribute_name = "v_color";
  attribute_v_color = glGetAttribLocation(program, attribute_name);
  if (attribute_v_color == -1) {
    cerr << "Could not bind attribute " << attribute_name << endl;
    return false;
  }
  
  return true;
}

void render(SDL_Window* window){
  //clear background
  glClearColor(1,1,1,1);
  glClear(GL_COLOR_BUFFER_BIT);
  
  glUseProgram(program);

  glEnableVertexAttribArray(attribute_coord3d);
  glEnableVertexAttribArray(attribute_v_color);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
  glVertexAttribPointer(
			attribute_coord3d,                         // attribute
			3,                                         // num of elements per vertex. our coords are (x,y), so 2
			GL_FLOAT,                                  // the type of each element
			GL_FALSE,                                  // take our values as is
			sizeof(struct attributes),                 // next coord3d appears every 6 floats.
			0                                          // offset of first element.
			);
  glVertexAttribPointer(
			attribute_v_color, // attribute
			3,                 // number of elements per vertex, here (r,g,b)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			sizeof(struct attributes),  // stride
			(GLvoid*) offsetof(struct attributes, v_color)  // offset
			);
  
  glDrawArrays(GL_TRIANGLES, 0 , 3);

  //disable attributes
  glDisableVertexAttribArray(attribute_v_color);
  glDisableVertexAttribArray(attribute_coord3d);  
  
  /* Display the result */
  SDL_GL_SwapWindow(window);
}

void free_resources(){
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_triangle);
}

void logic() {
  glUseProgram(program);
  
  //matrix transformation
  float move = sinf(SDL_GetTicks() / 1000.0 * (2*3.14) / 5); // -1<->+1 every 5 seconds
  float angle = SDL_GetTicks() / 1000.0 * 45;  // 45° per second
  glm::vec3 axis_z(0, 0, 1);
  glm::mat4 m_transform = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_z)
    * glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, 0.0));

  glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, glm::value_ptr(m_transform));
  
  // alpha 0->1->0 every 5 seconds
  float cur_fade = sinf(SDL_GetTicks() / 1000.0 * (2*3.14) / 5) / 2 + 0.5;

  glUniform1f(uniform_fade, cur_fade);
}

void mainLoop(SDL_Window* window){
  while(true){
    SDL_Event ev;
    while(SDL_PollEvent(&ev)){
      if(ev.type == SDL_QUIT)
	return;
    }
    logic();
    render(window);
  }
}

int main(int argc, char* argv[]){
  //SDL-related initialising functions
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow("My Second Triangle",
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					640, 480,
					SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  if (window == NULL) {
    cerr << "Error: can't create window: " << SDL_GetError() << endl;
    return EXIT_FAILURE;
  }
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  
  // Enable transparency
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
  
  if (SDL_GL_CreateContext(window) == NULL) {
    cerr << "Error: SDL_GL_CreateContext: " << SDL_GetError() << endl;
    return EXIT_FAILURE;
  }
  SDL_GL_CreateContext(window);

  //extension wrangler initialization
  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK){
    cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
    return EXIT_FAILURE;
  }

  if (!GLEW_VERSION_2_0) {
		cerr << "Error: your graphic card does not support OpenGL 2.0" << endl;
		return EXIT_FAILURE;
  }
  
  //if the program can't initialize resources, exit with code EXIT_FAILURE
  if(!init_resources())
     return EXIT_FAILURE;
  
  //enable alpha
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  mainLoop(window);

  free_resources();
  return EXIT_SUCCESS;
     
}
