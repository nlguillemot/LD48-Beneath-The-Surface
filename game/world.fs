#version 150

in vec2 ftexcoord0;

uniform sampler2D diffuseTexture;

out vec4 oColor;

void main()
{
    oColor = texture(diffuseTexture, ftexcoord0);
}
