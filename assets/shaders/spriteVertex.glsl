#version 330                                                                        

in vec3 Position;
in vec2 TexCoordinates;

out vec2 texcoords;

uniform mat4 MVP;

void main()
{          
	texcoords = TexCoordinates.xy;
    gl_Position = MVP * vec4(Position, 1);
}
