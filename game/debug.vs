#version 150

in vec4 position;
in vec4 tint;

uniform mat4 modelview;
uniform mat4 projection;

out vec4 ftint;

void main()
{
    gl_Position = projection * modelview * position;
    ftint = tint;
}
