#include <glad/glad.h>

const char *vertex_shader_source = "#version 450 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in float aShine;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "out float shineColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "   shineColor = aShine;\n"
    "}\0";

const char *fragment_shader_source = "#version 450 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "in float shineColor;\n"
    "uniform sampler2D tex;\n"
    "void main()\n"
    "{\n"
    "   FragColor = mix(texture(tex, TexCoord), vec4(1.0, 1.0, 1.0, 1.0), shineColor);\n"
    "}\0";

    //"uniform sampler2D overlayTexture;\n"
void new_shader_program(GLuint *shaderProgram) {
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex_shader_source, 0);
  glCompileShader(vertexShader);

  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragment_shader_source, 0);
  glCompileShader(fragmentShader);

  // Link the vertex and fragment shader into a shader program
  glAttachShader(*shaderProgram, vertexShader);
  glAttachShader(*shaderProgram, fragmentShader);
  glLinkProgram(*shaderProgram);

  glUseProgram(*shaderProgram);

  glUniform1i(glGetUniformLocation(*shaderProgram, "tex"), 0);
}
