#include <glad/glad.h>
#include <GLFW/glfw3.h>

const GLchar* vertex_shader_source = "#version 150 core\n"
    "in vec2 position;\n"
    "in vec3 color;\n"
    "in vec2 texcoord;\n"
    "out vec3 Color;\n"
    "out vec2 Texcoord;\n"
    "void main()\n"
    "{\n"
    "    Color = color;\n"
    "    Texcoord = texcoord;\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\0";

const GLchar* fragment_shader_source = "#version 150 core\n"
    "in vec3 Color;\n"
    "in vec2 Texcoord;\n"
    "out vec4 outColor;\n"
    "uniform sampler2D tex;\n"
    "void main(){\n"
    "    outColor = outColor = texture(tex, Texcoord);\n"
    "}\0";

    //"    outColor = outColor = texture(tex, Texcoord) * vec4(Color, 1.0);\n"

GLuint new_shader_program() {
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertexShader);

  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragmentShader);

  // Link the vertex and fragment shader into a shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);

  return shaderProgram;
}
