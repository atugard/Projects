#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "./common/shader_utils.h"
#include <glm/gtc/type_ptr.hpp>

//global vars
GLuint ibo_cube_elements; //ibo = index buffer object
GLint uniform_m_transform;
GLuint program;
GLuint vbo_cube_vertices, vbo_cube_colors;
GLint attribute_coord3d, attribute_v_color;
GLint uniform_fade;
int screen_width=800, screen_height=600;

struct attributes {
  GLfloat coord3d[3];
  GLfloat v_color[3];
};
using namespace std;

bool init_resources(){
   GLfloat cube_vertices[] = {
    // front
    -1.0, -1.0,  1.0,
     1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    // back
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
     1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0
  };
  glGenBuffers(1, &vbo_cube_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

   GLfloat cube_colors[] = {
    // front colors
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    // back colors
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0
  };
   glGenBuffers(1, &vbo_cube_colors);
   glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
   glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);

   GLushort cube_elements[] = {
     // front
     0, 1, 2,
     2, 3, 0,
     // right
     1, 5, 6,
     6, 2, 1,
     // back
     7, 6, 5,
     5, 4, 7,
     // left
     4, 0, 3,
     3, 7, 4,
     // bottom
     4, 5, 1,
     1, 0, 4,
     // top
     3, 2, 6,
     6, 7, 3
   };
  glGenBuffers(1, &ibo_cube_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
  
  GLuint vs, fs;
  if ((vs = create_shader("cube.v.glsl", GL_VERTEX_SHADER))   == 0) return false;
  if ((fs = create_shader("cube.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false;  

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
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  glUseProgram(program);
  
  //vertices
  glEnableVertexAttribArray(attribute_coord3d);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
  glVertexAttribPointer(
			attribute_coord3d,                         // attribute
			3,                                         // num of elements per vertex. our coords are (x,y), so 2
			GL_FLOAT,                                  // the type of each element
			GL_FALSE,                                  // take our values as is
			0,                 // next coord3d appears every 6 floats.
			0                                          // offset of first element.
			);
  
  //colors
  glEnableVertexAttribArray(attribute_v_color);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_colors);
  glVertexAttribPointer(
			attribute_v_color, // attribute
			3,                 // number of elements per vertex, here (r,g,b)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,  // stride
			0  // offset
			);

  /* Push each element in buffer_vertices to the vertex shader */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
  int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);


  glDisableVertexAttribArray(attribute_coord3d);
  glDisableVertexAttribArray(attribute_v_color);

  SDL_GL_SwapWindow(window);
}

void free_resources(){
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_cube_vertices);
  glDeleteBuffers(1, &vbo_cube_colors);
  glDeleteBuffers(1, &ibo_cube_elements);
}
 float aspectaxis()
  {
    float outputzoom = 1.0f;
    float aspectorigin = 16.0f / 9.0f;
    int aspectconstraint = 1;
    switch (aspectconstraint)
      {
      case 1:
	if ((screen_width / screen_height) < aspectorigin)
	  {
	    outputzoom *= (((float)screen_width / screen_height) / aspectorigin);
	      }
	else
	  {
	    outputzoom *= ((float)aspectorigin / aspectorigin);
	      }
	break;
      case 2:
	outputzoom *= (((float)screen_width / screen_height) / aspectorigin);
          break;
      default:
	outputzoom *= ((float)aspectorigin / aspectorigin);
	  }
    return outputzoom;
  }
float recalculatefov()
{
  return 2.0f * glm::atan(glm::tan(glm::radians(45.0f / 2.0f)) / aspectaxis());
}
void logic() {
  glUseProgram(program);
  
  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
  glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 projection = glm::perspective(recalculatefov(), 1.0f * screen_width / screen_height, 0.1f, 10.0f);
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
  

  glEnable(GL_DEPTH_TEST);    


  mainLoop(window);

  free_resources();
  return EXIT_SUCCESS;
     
}
