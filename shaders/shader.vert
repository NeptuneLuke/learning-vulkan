#version 460

// frag_colors is the output vector that
// will be colored in shader.frag
layout(location = 0) out vec3 frag_colors;

// The vector that stores the vertex positions (x,y)
// that forms the triangle.
vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

// The vector that stores the vertex colors (RGB).
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

// Main function is invoked for every vertex.
// gl_VertexIndex contains the index of the current vertex (index of the vector positions).
// gl_Position is the output vector.
void main() {

    // gl_Position is the positions (x,y) with the (z,w) coordinates added.
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);

    // colors every index of frag_colors which is passed to
    // shader.frag
    frag_colors = colors[gl_VertexIndex];
}