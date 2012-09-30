#ifdef VERTEX

attribute vec4 parameterAndSide;
uniform float thickness;
uniform mat4 positionMatrix;
uniform mat4 normalMatrix;

uniform float taperStart;
uniform float taperDelta;

void main() {
	vec4 parameter = vec4(parameterAndSide.xyz, 1);
	gl_Position = gl_ModelViewProjectionMatrix * vec4((
		(positionMatrix * parameter) + 
		(thickness * parameterAndSide.w * (taperStart + parameterAndSide.z * taperDelta)) * normalize(normalMatrix * parameter)
	).xy, 0, 1);
}

#else

uniform vec4 color;

void main() {
	gl_FragColor = color;	
}

#endif
