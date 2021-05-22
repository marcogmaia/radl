#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    // Texel color fetching from texture sampler
    // vec4 texelColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;
    // vec4 texelColor = texture(texture0, fragTexCoord);
    vec4 texelColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;
    // Convert texel color to grayscale using NTSC conversion weights
    // float gray = dot(texelColor.rgb, vec3(0.4, 0.6745, 0.2314));
 

    // Calculate final fragment color
    if(texelColor.r == 1.0 && texelColor.b == 1.0)
         finalColor = vec4(0.1059, 0.2039, 0.4784, 0.459);
    else
    finalColor = texelColor;
}