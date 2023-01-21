#ifndef _SHADER_UTILS_H
#define _SHADER_UTILS_H
#include <cstdlib>
#include <cstddef>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <iostream>
extern char* file_read(const char* filename);
extern void print_log(GLuint object);
extern GLuint create_shader(const char* filename, GLenum type);

#endif
