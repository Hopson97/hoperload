
#version 450 core

layout (location = 0) in vec3 inVertexCoord;
layout (location = 1) in vec3 inTexCoord;
layout (location = 2) in float light;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;
uniform float waterLevel;

out vec3 passTextureCoord;
out float passLight;
out vec3 passFragPosition;


void main()
{   
    vec4 worldPos = modelMatrix * vec4(inVertexCoord, 1.0);

    gl_Position = projectionViewMatrix * worldPos;
    passTextureCoord = inTexCoord;

    passLight = light;
    passFragPosition = vec3(worldPos);
}