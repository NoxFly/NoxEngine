in vec4 color;
in vec2 coordTexture;

uniform sampler2D tex;

out vec4 out_Color;

void main()
{
    vec4 tex = texture(tex, coordTexture);

    out_Color = tex;
}