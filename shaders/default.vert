#version 330 core

// Position/Coordinates
layout (location = 0) in vec3 aPos;
// Normals
layout (location = 1) in vec3 aNormal;
// Texture coordinates
layout (location = 2) in vec3 aColor;

// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the fragment shader
// out vec2 texCoord;

out vec3 Normal;
// Outputs the current position of the fragment because the light calculations are made in world space
out vec3 FragPos;

uniform mat4 camMatrix;
uniform mat4 model;
uniform float scale;

void main()
{  
   vec3 scaledPos = vec3(aPos.x * scale, aPos.y * scale, aPos.z * scale);
   FragPos = vec3(model * vec4(scaledPos, 1.0f));

   // Outputs the positions/coordinates of all vertices
   gl_Position = camMatrix * vec4(FragPos, 1.0);

   // Assigns the colors from the Vertex Data to the "color"
   // color = aColor;

   // Assigns the color of the vertices from the Vertex Data to "Color"
   color = aColor;

   // Assigns the Normal coordinates from the Vertex Data to "aNormal"
   //Normal = mat3(transpose(inverse(model))) * aNormal; // for non-uniform scale
    Normal = vec3(model * vec4(aNormal, 0.0)); // works with rotations/translations because is orthonormal
}