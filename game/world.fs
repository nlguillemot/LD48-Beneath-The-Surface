#version 150

in vec2 ftexcoord0;

uniform sampler2D diffuseTexture;

out vec4 oColor;

void main()
{
    vec4 texel = texture(diffuseTexture, ftexcoord0);
    if (texel.a < 0.05)
        discard;
    oColor = texel;
}
