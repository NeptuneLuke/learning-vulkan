#version 450


// The vector that stores the vertex positions (x,y)
// that forms the triangle.
vec2 positions[3] = vec2[](
    vec2(0.0, -1),
    vec2(1, 1),
    vec2(-1, 1)
);


// Main function is invoked for every vertex.
// gl_VertexIndex contains the index of the current vertex (index of the vector positions).
// gl_Position is the output vector.
void main() {

    // gl_Position is the positions (x,y) with the (z,w) coordinates added.
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}