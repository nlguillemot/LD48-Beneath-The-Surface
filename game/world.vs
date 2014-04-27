#version 150

in vec4 position;
in vec2 texcoord0;

uniform mat4 modelview;
uniform mat4 projection;

out vec2 ftexcoord0;

void main()
{
    ftexcoord0 = texcoord0;
    gl_Position = projection * modelview * position;
}
