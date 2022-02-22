#version 330

#define PI 3.141592653

in vec2 texCoord;
in vec4 fragColor;

uniform float timeOfDay;
uniform float timeOfDayMax;
uniform vec2 sunPos;
uniform vec2 horizonPos;

uniform float screenHeight;
uniform float screenWidth;

out vec4 fragOut;

void main()
{

	// how to lerp again?
	float freq = 2.0f;	
	
	float timeToUse = 0.5f + 0.5f*cos((1.0f/timeOfDayMax) * timeOfDay * 2 * PI);
	// actually cosine will be a better choice here
	float distToHorizon = pow(texCoord.y, abs((horizonPos.y/screenHeight) - texCoord.y));
    	// fragOut = vec4((0.05f * timeToUse) - distToHorizon, 0.02f, 0.4f * timeToUse, 1.0f);
	fragOut = vec4(timeToUse * distToHorizon, 0.02f, 0.04f + timeToUse*0.25f, 1.0f);
}