layout (location = 0) in vec3 in_Vertex;
layout (location = 1) in vec3 in_Color;

uniform mat4 MVP;

out vec3 color;

void main()
{
    gl_Position = MVP * vec4(in_Vertex, 1.0);
    color = in_Color;
}