#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// uniforms
uniform int num_pics;
uniform int scale_step;
uniform int period; // scale^numpic multiplicative
uniform float zoom; // multiplicative

uniform sampler2D textures[8];

void main()
{
    vec2 flipped_frag_coord = vec2(fragTexCoord.x, 1-fragTexCoord.y);
    vec2 cart_coord = (flipped_frag_coord*2-vec2(1.0,1.0))*zoom;

    while (max(abs(cart_coord.x), abs(cart_coord.y)) < 1.0/(period))
        cart_coord = cart_coord * period; 
    while (max(abs(cart_coord.x), abs(cart_coord.y)) > 1.0)    
        cart_coord = cart_coord / period;

    finalColor = texture(texture0, (cart_coord + vec2(1.0,1.0))/2 );
}