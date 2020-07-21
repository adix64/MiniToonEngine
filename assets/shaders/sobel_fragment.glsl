#version 330

uniform sampler2D normalTex;
uniform sampler2D charactersTex;
uniform sampler2D depthTex;

uniform int wWidth, wHeight;

vec2 texCoords;

vec3 luma=vec3(0.299, 0.587, 0.114);

vec2 CalcTexCoord(float offsetx, float offsety)
{
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(wWidth, wHeight);
}


void main()
{
	vec2 texCoord = CalcTexCoord(0, 0);
	
	vec4 characterNfo = texture2D(charactersTex, texCoord);
	if(characterNfo.x == 0)
	{
		gl_FragColor = vec4(1);
		return;	
	}

	// world_pos = texture2D(posTex, texCoord).xyz;
	vec3 world_normal = texture2D(normalTex, CalcTexCoord(0.0, 0.0)).xyz;
	
	//SOBEL EDGES ..............
	float Gx = 
		   	- texture2D(normalTex, CalcTexCoord(-1.0, -1.0)).x
			- texture2D(normalTex, CalcTexCoord( 0.0, -1.0)).x
			- texture2D(normalTex, CalcTexCoord(1.0, -1.0)).x
			+ texture2D(normalTex, CalcTexCoord(-1.0,  1.0)).x
			+ texture2D(normalTex, CalcTexCoord( 0.0,  1.0)).x
			+ texture2D(normalTex, CalcTexCoord(1.0,  1.0)).x;
	float Gy =
			- texture2D(normalTex, CalcTexCoord(-1.0, -1.0)).x
			- texture2D(normalTex, CalcTexCoord(-1.0,  0.0)).x
			- texture2D(normalTex, CalcTexCoord(-1.0,  1.0)).x
			+ texture2D(normalTex, CalcTexCoord( 1.0, -1.0)).x
	   		+ texture2D(normalTex, CalcTexCoord( 1.0,  0.0)).x
	   	  	+ texture2D(normalTex, CalcTexCoord( 1.0,  1.0)).x;
	// if(1 - sqrt(Gx * Gx + Gy * Gy) < 0.5)
	


	float contour = 1 - sqrt(Gx * Gx + Gy * Gy);
	
	
	// Gx = 
	// 	   	- texture2D(charactersTex, CalcTexCoord(-1.0, -1.0)).x
	// 		- texture2D(charactersTex, CalcTexCoord( 0.0, -1.0)).x
	// 		- texture2D(charactersTex, CalcTexCoord(	1.0, -1.0)).x
	// 		+ texture2D(charactersTex, CalcTexCoord(-1.0,  1.0)).x
	// 		+ texture2D(charactersTex, CalcTexCoord( 0.0,  1.0)).x
	// 		+ texture2D(charactersTex, CalcTexCoord(	1.0,  1.0)).x;
	// Gy =
	// 		- texture2D(charactersTex, CalcTexCoord(-1.0, -1.0)).x
	// 		- texture2D(charactersTex, CalcTexCoord(-1.0,  0.0)).x
	// 		- texture2D(charactersTex, CalcTexCoord(-1.0,  1.0)).x
	// 		+ texture2D(charactersTex, CalcTexCoord( 1.0, -1.0)).x
	//    		+ texture2D(charactersTex, CalcTexCoord( 1.0,  0.0)).x
	//    	  	+ texture2D(charactersTex, CalcTexCoord( 1.0,  1.0)).x;

	// contour = min(contour, (1 - sqrt(Gx * Gx + Gy * Gy)));


	Gx = 
		   	- texture2D(depthTex, CalcTexCoord(-1.0, -1.0)).x
			- texture2D(depthTex, CalcTexCoord( 0.0, -1.0)).x
			- texture2D(depthTex, CalcTexCoord(	1.0, -1.0)).x
			+ texture2D(depthTex, CalcTexCoord(-1.0,  1.0)).x
			+ texture2D(depthTex, CalcTexCoord( 0.0,  1.0)).x
			+ texture2D(depthTex, CalcTexCoord(	1.0,  1.0)).x;
	Gy =
			- texture2D(depthTex, CalcTexCoord(-1.0, -1.0)).x
			- texture2D(depthTex, CalcTexCoord(-1.0,  0.0)).x
			- texture2D(depthTex, CalcTexCoord(-1.0,  1.0)).x
			+ texture2D(depthTex, CalcTexCoord( 1.0, -1.0)).x
	   		+ texture2D(depthTex, CalcTexCoord( 1.0,  0.0)).x
	   	  	+ texture2D(depthTex, CalcTexCoord( 1.0,  1.0)).x;

	contour = min(contour, pow(1 - sqrt(Gx * Gx + Gy * Gy), 70));
	// contour	= pow((1 - sqrt(Gx * Gx + Gy * Gy)), 40);

	contour = contour * 2;//clamp(contour,0,0.25) * 4;
	contour += characterNfo.z;

	gl_FragColor = vec4(contour, contour, contour, 1);
	// color *= clamp(1 - sqrt(Gx * Gx + Gy * Gy) + texture2D(charactersTex, texCoord).z,0.5,1);//vec3(0.08382,0.08382,0.08382);
}