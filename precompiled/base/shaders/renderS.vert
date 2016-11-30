void main()
{
	// calculate stoof
	vec4 vertex = gl_Vertex;
	
    // transform the vertex position
    gl_Position = gl_ModelViewProjectionMatrix * vertex;

    // transform the texture coordinates
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    // forward the vertex color
    gl_FrontColor = gl_Color;
}