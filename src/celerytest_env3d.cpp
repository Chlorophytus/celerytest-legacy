#include "../include/celerytest_env3d.hpp"
#include "../include/celerytest_log.hpp"
using namespace celerytest;

env3d_shaderprogram::env3d_shaderprogram()
    : program(glCreateProgram()), has_linked(false), ticks(0) {}

void env3d_shaderprogram::attach(std::string_view &&src,
                                 env3d_shaderprogram::types type) {
  log(severity::info, {"Attaching shader..."});
  sources.emplace_front(src);
  switch (type) {
  case types::compute: {
    log(severity::info, {" ... compute"});
    shaders.emplace_front(glCreateShader(GL_COMPUTE_SHADER));
    break;
  }
  case types::fragment: {
    log(severity::info, {" ... fragment"});
    shaders.emplace_front(glCreateShader(GL_FRAGMENT_SHADER));
    break;
  }
  case types::geometry: {
    log(severity::info, {" ... geometry"});
    shaders.emplace_front(glCreateShader(GL_GEOMETRY_SHADER));
    break;
  }
  case types::vertex: {
    log(severity::info, {" ... vertex"});
    shaders.emplace_front(glCreateShader(GL_VERTEX_SHADER));
    break;
  }
  }
  log(severity::info, {"Copying source for this shader"});
  auto raw = new char[sources.front().size() + 1];
  auto i = 0;
  for (auto &&ch : sources.front()) {
    raw[i++] = ch;
  }
  raw[sources.front().size()] = 0;
  glShaderSource(shaders.front(), 1, &raw, NULL);
  raws.emplace_front(raw);

  log(severity::info, {"Attachment success, compiling..."});
  glCompileShader(shaders.front());
  auto compile_status = GLint(0);
  glGetShaderiv(shaders.front(), GL_COMPILE_STATUS, &compile_status);
  if (compile_status != GL_TRUE) {
    auto log_length = GLsizei(0);
    auto message = new GLchar[4096];
    glGetShaderInfoLog(shaders.front(), 4096, &log_length, message);
    log(severity::error,
        {"Can't compile this shader, check BELOW for error log"});
    log(severity::error, {message});
    log(severity::error,
        {"Can't compile this shader, check ABOVE for error log\n"});
    delete[] message;
  }
  assert(compile_status == GL_TRUE);

  log(severity::info, {"Compile success"});
}

void env3d_shaderprogram::link() {
  log(severity::info, {"Linking program"});
  assert(!has_linked);
  has_linked = true;

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

env3d_shaderprogram::~env3d_shaderprogram() {
  for (auto &&shader : shaders) {
    glDetachShader(program, shader);
    glDeleteShader(shader);
  }
  glDeleteProgram(program);
  for (auto &&raw : raws) {
    delete[] raw;
  }
}

void env3d_shaderprogram::tick() { glUniform1ui(0, ticks++); }