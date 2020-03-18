#include "../include/celerytest_env3d.hpp"
#include "../include/celerytest_log.hpp"
using namespace celerytest;

env3d_shaderopaque::env3d_shaderopaque() : has_compiled(false) {}

env3d_shaderprogram::env3d_shaderprogram()
    : program(glCreateProgram()), has_linked(false), ticks(0), opaques({}) {}

void env3d_shaderopaque::compile(std::string_view &&src, types type) {
  log(severity::info, {"Creating and compiling a shader..."});
  switch (type) {
  case types::compute: {
    log(severity::info, {" ... compute"});
    shader = glCreateShader(GL_COMPUTE_SHADER);
    break;
  }
  case types::fragment: {
    log(severity::info, {" ... fragment"});
    shader = glCreateShader(GL_FRAGMENT_SHADER);
    break;
  }
  case types::geometry: {
    log(severity::info, {" ... geometry"});
    shader = glCreateShader(GL_GEOMETRY_SHADER);
    break;
  }
  case types::vertex: {
    log(severity::info, {" ... vertex"});
    shader = glCreateShader(GL_VERTEX_SHADER);
    break;
  }
  }

  log(severity::info, {" ... copy source"});
  auto size = src.size();
  raw_source = new char[size + 1];
  raw_source[size] = 0;
  auto i = 0;

  for (auto &&ch : src) {
    raw_source[i++] = ch;
  }

  log(severity::info, {" ... load source"});
  glShaderSource(shader, 1, &raw_source, nullptr);
  log(severity::info, {" ... compile"});
  glCompileShader(shader);

  auto compile_status = GLint(0);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

  if (compile_status != GL_TRUE) {
    auto log_length = GLsizei(0);
    auto message = new GLchar[4096];
    glGetShaderInfoLog(shader, 4096, &log_length, message);
    log(severity::error,
        {"Can't compile this shader, check BELOW for error log"});
    log(severity::error, {message});
    log(severity::error,
        {"Can't compile this shader, check ABOVE for error log\n"});
    delete[] message;
  }
  assert(compile_status == GL_TRUE);

  has_compiled = true;
  log(severity::info, {" ... success"});
}

void env3d_shaderprogram::link() {
  log(severity::info, {"Linking program"});
  assert(!has_linked);
  has_linked = true;

  opaques.reverse();
  for (auto &&opaque : opaques) {
    assert(opaque.has_compiled);
    glAttachShader(program, opaque.shader);
  }

  if (main_program) {
    log(severity::warn, {"Main shaderprogram mode enable for this one"});
    glBindFragDataLocation(program, 0, "color");
  }

  glLinkProgram(program);
  auto link_status = GLint(0);
  glGetProgramiv(program, GL_LINK_STATUS, &link_status);
  if (link_status != GL_TRUE) {
    auto log_length = GLsizei(0);
    auto message = new GLchar[4096];
    glGetProgramInfoLog(program, 4096, &log_length, message);
    log(severity::error,
        {"Can't link this program, check BELOW for error log"});
    log(severity::error, {message});
    log(severity::error,
        {"Can't link this program, check ABOVE for error log\n"});
    delete[] message;
  }
  assert(link_status == GL_TRUE);

  log(severity::info, {"Using this program"});
  
  glUseProgram(program);

  log(severity::info, {"Link success"});
}
env3d_shaderopaque::~env3d_shaderopaque() {
  if (has_compiled) {
    glDeleteShader(shader);
    delete[] raw_source;
  }
}
env3d_shaderprogram::~env3d_shaderprogram() {
  for (auto &&opaque : opaques) {
    if (opaque.has_compiled)
      glDetachShader(program, opaque.shader);
  }
  glDeleteProgram(program);
}

void env3d_shaderprogram::tick() { glUniform1ui(0, ticks++); }