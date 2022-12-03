layout (location = 0) in vec3 in_Vertex;
layout (location = 1) in vec4 in_Color;
layout (location = 2) in vec2 in_TexCoord0;

uniform mat4 MVP;

out vec4 color;
out vec2 coordTexture;

void main()
{
    gl_Position = MVP * vec4(in_Vertex, 1.0);
    color = in_Color;
    coordTexture = in_TexCoord0;
}