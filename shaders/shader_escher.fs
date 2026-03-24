
#version 330

#define PI 3.1415926538
#define cx_mul(a, b) vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x)
#define cx_log(a) vec2(log(length(a)),atan(a.y,a.x))
#define cx_exp(a) vec2(exp(a.x)*cos(a.y), exp(a.x)*sin(a.y))

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

uniform int period; // scale^numpic multiplicative
uniform float zoom; // multiplicative
uniform vec2 gamma;

void main()
{
    vec2 flipped_frag_coord = vec2(fragTexCoord.x,1-fragTexCoord.y);
    vec2 cart_coord = (flipped_frag_coord*2-vec2(1.0,1.0));

    cart_coord = cx_exp(cx_mul(gamma,cx_log(cart_coord)))*zoom;

    while (max(abs(cart_coord.x), abs(cart_coord.y)) < 1.0/(period))
        cart_coord = cart_coord * period; 
    while (max(abs(cart_coord.x), abs(cart_coord.y)) > 1.0)    
        cart_coord = cart_coord / period;

    finalColor = texture(texture0, (cart_coord + vec2(1.0,1.0))/2 );
}