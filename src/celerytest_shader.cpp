#include "../include/celerytest_shader.hpp"
#include "../include/celerytest_log.hpp"
using namespace celerytest;

shader::shader(shader::type type, std::string_view &&source)
    : shader_type{type}, shader_source{source} {
  switch (type) {
  case shader::type::compute:
    idx = glCreateShader(GL_COMPUTE_SHADER);
    break;
  case shader::type::vertex:
    idx = glCreateShader(GL_VERTEX_SHADER);
    break;
  case shader::type::fragment:
    idx = glCreateShader(GL_FRAGMENT_SHADER);
    break;
  case shader::type::geometry:
    idx = glCreateShader(GL_GEOMETRY_SHADER);
    break;
  }
  log(severity::info, {"Allocating shader..."});
  raw = new char[shader_source.size() + 1];
  auto i = 0;
  for (auto &&ch : shader_source) {
    raw[i++] = ch;
  }
  raw[shader_source.size()] = 0;
  glShaderSource(idx, 1, &raw, NULL);
  log(severity::info, {"Compiling shader"});
  glCompileShader(idx);
  auto compile_status = GLint(0);
  glGetShaderiv(idx, GL_COMPILE_STATUS, &compile_status);
  if (compile_status != GL_TRUE) {
    auto log_length = GLsizei(0);
    auto message = new GLchar[4096];
    glGetShaderInfoLog(idx, 4096, &log_length, message);
    log(severity::error,
        {"Can't compile this shader, check BELOW for error log"});
    log(severity::error, {message});
    log(severity::error,
        {"Can't compile this shader, check ABOVE for error log\n"});
    delete[] message;
  }
  assert(compile_status == GL_TRUE);
}

shader::~shader() {
  glDeleteShader(idx);
  delete[] raw;
}
shader_chain::shader_chain() : shader_idxs{} {
  log(severity::info, {"Creating program lazily"});
  idx = glCreateProgram();
}
void shader_chain::link() {
  log(severity::info, {"Attaching program subshaders"});
  for (auto &&shader_idx : shader_idxs) {
    glAttachShader(idx, shader_idx);
  }
  glBindFragDataLocation(idx, 0, "color");
  log(severity::info, {"Linking program"});
  glLinkProgram(idx);
  auto link_status = GLint(0);
  glGetProgramiv(idx, GL_LINK_STATUS, &link_status);
  if (link_status != GL_TRUE) {
    auto log_length = GLsizei(0);
    auto message = new GLchar[4096];
    glGetProgramInfoLog(idx, 4096, &log_length, message);
    log(severity::error,
        {"Can't link this program, check BELOW for error log"});
    log(severity::error, {message});
    log(severity::error,
        {"Can't link this program, check ABOVE for error log\n"});
    delete[] message;
  }
  assert(link_status == GL_TRUE);

  glUseProgram(idx);
  log(severity::info, {"Using this program"});
}

shader_chain::~shader_chain() {
    glDeleteProgram(idx);
}