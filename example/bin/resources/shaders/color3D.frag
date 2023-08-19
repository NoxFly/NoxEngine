in vec3 color;

uniform vec3 objectColor;

out vec4 out_Color;

void main()
{
    out_Color = vec4(objectColor, 1.0);
}