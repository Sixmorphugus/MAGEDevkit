// -----------------------------------
// FRAGMENT
// GRAYSCALE GLSL SHADER
// -----------------------------------

uniform sampler2D texture;
uniform vec2 textureSize;
uniform float clockTime;
uniform float multiplier;

void main(void)
{
	vec2 tc = gl_TexCoord[0].st;

	vec4 c = texture2D(texture, tc);
	vec4 origColor = c * gl_Color;
	
	origColor.g = origColor.g;
	origColor.r = mix(origColor.g, origColor.r, multiplier);
	origColor.b = mix(origColor.g, origColor.b, multiplier);

	gl_FragColor = origColor;
}