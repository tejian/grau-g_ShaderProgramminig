#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 texCoord;

out vec4 frontColor;
out vec2 vtexCoord;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

void main()
{
	vtexCoord = texCoord;
    gl_Position = modelViewProjectionMatrix * vec4(vertex, 1.0);

    vec3 top; 
    vec3 bottom;
    float a;    
    
    float b = gl_Position.y/gl_Position.w + 1.0;
    b = b / 2.0;
    if(b>=0 && b < 0.25)
    {
		bottom = vec3(1.0,0.0,0.0);
		top = vec3(1.0,1.0,0.0);
		a = fract(b/0.25);
	}
    else if(b>=0.25 && b < 0.5)
    {
		top = vec3(0.0,1.0,0.0);
		bottom = vec3(1.0,1.0,0.0);
		a = fract(b/0.25);
	}
    
    else if(b>=0.5 && b < 0.75)
    {
		top = vec3(0.0,1.0,1.0);
		bottom = vec3(0.0,1.0,0.0);
		a = fract(b/0.25);
	}
    else if(b>=0.75 && b <= 1)
    {
		top = vec3(0.0,0.0,1.0);
		bottom = vec3(0.0,1.0,1.0);
		a = (b-0.25*3)/0.25;
	}
    
    vec3 gradientColor = mix(bottom, top, a);
    
    frontColor = vec4(gradientColor,1.0);
}
