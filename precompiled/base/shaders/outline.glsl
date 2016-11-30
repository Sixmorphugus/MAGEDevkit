// -----------------------------------
// FRAGMENT
// OUTLINE GLSL SHADER
// -----------------------------------

uniform sampler2D texture;
uniform vec2 textureSize;
uniform float clockTime;

void main(void)
{
	vec4 outlineColor = vec4(1.0, 1.0, 200.0 / 255.0, 1.0);
	
	vec2 off = 1.0 / textureSize;
	vec2 tc = gl_TexCoord[0].st;

	vec4 c = texture2D(texture, tc);
	vec4 n = texture2D(texture, vec2(tc.x, tc.y - off.y));
	vec4 e = texture2D(texture, vec2(tc.x + off.x, tc.y));
	vec4 s = texture2D(texture, vec2(tc.x, tc.y + off.y));
	vec4 w = texture2D(texture, vec2(tc.x - off.x, tc.y));
	
	vec4 origColor = c * gl_Color;

	float ua = 0.0;
	ua = mix(ua, 1.0, c.a);
	ua = mix(ua, 1.0, n.a);
	ua = mix(ua, 1.0, e.a);
	ua = mix(ua, 1.0, s.a);
	ua = mix(ua, 1.0, w.a);

	vec4 underColor = outlineColor * vec4(ua);

	gl_FragColor = underColor;
	gl_FragColor = mix(gl_FragColor, origColor, origColor.a);
}