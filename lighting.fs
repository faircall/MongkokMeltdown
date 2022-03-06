#version 330

#define PI 3.141592653

in vec2 texCoord;
in vec4 fragColor;


uniform sampler2D tex;

uniform float timeOfDay;
uniform float timeOfDayMax;

out vec4 fragOut;

void main()
{
	float timeToUse = 0.5f + 0.5f*cos((1.0f/timeOfDayMax) * timeOfDay * 2 * PI);

	vec4 sourceColor = texture(tex, texCoord);

	
	
	   // sourceColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	   //sourceColor = vec4(sourceColor.r - 0.5f*timeTouse, sourceColor.g - 0.5f*timeTouse, sourceColor.b - 0.5f*timeTouse, sourceColor.a);
	   
	//sourceColor = vec4(timeToUse, 0.0f, 0.0f, sourceColor.a);
	   

    
	fragOut = vec4(timeToUse*sourceColor.r, timeToUse*sourceColor.g, timeToUse*sourceColor.b, sourceColor.a);
}