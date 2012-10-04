#ifdef VERTEX

attribute vec4 parameterAndSide;
uniform mat4 positionMatrix;
uniform mat4 normalMatrix;
uniform vec4 strokeVector;


void main() {
	vec4 parameter = vec4(parameterAndSide.xyz, 1);
	gl_Position = gl_ModelViewProjectionMatrix * vec4((
		(positionMatrix * parameter) + 
		(dot(strokeVector, parameter) * parameterAndSide.w) * normalize(normalMatrix * parameter)
	).xyz, 1);
}

#else

uniform vec4 color;

void main() {
	gl_FragColor = color;	
}

#endif
