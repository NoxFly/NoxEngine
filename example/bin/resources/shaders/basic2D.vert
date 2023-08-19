in vec2 in_Vertex;
in vec3 in_Color;
in vec2 in_TexCoord0;

out vec3 color;
out vec2 coordTexture;

void main()
{
    gl_Position = vec4(in_Vertex, 0.0, 1.0);

    color = in_Color;
    coordTexture = in_TexCoord0;
}