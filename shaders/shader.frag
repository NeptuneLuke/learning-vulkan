#version 460

// The triangle formed by the positions from
// the vertex shader fills a screen area with fragments.
// The fragment shader is invoked on each fragment to produce
// a color and depth value for the framebuffer.

// frag_colors is the input vector taken from
// shader.vert, which is only RGB
layout(location = 0) in vec3 frag_colors;

// output_color is the vector that outpust to screen,
// specifically to the framebuffer of index 0.
layout(location = 0) out vec4 output_color;


// Main function is invoked for every fragment.
void main() {

    // Set the output_color values for each fragment
    // (each pixel of the triangle) to be red -> (1,0,0,1) in RGBA.
    // output_color = vec4(1.0, 0.0, 0.0, 1.0);

    // gradient triangle
    output_color = vec4(frag_colors, 1.0);
}