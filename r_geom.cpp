#include "r_geom.h"

#include "m_vec3.h"
#include "m_const.h"

#include "u_vector.h"

namespace r {

geom::geom()
    : vbo(0)
    , ibo(0)
    , vao(0)
{
}

geom::~geom() {
    if (buffers[0])
        gl::DeleteBuffers(2, buffers);
    if (vao)
        gl::DeleteVertexArrays(1, &vao);
}

void geom::upload() {
    gl::GenVertexArrays(1, &vao);
    gl::GenBuffers(2, buffers);
}

bool quad::upload() {
    geom::upload();

    gl::BindVertexArray(vao);
    gl::BindBuffer(GL_ARRAY_BUFFER, vbo);

    static const GLfloat vertices[] = {
        -1.0f,-1.0f, 0.0f, 0.0f,  0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
         1.0f, 1.0f, 0.0f, 1.0f, -1.0f,
         1.0f,-1.0f, 0.0f, 1.0f,  0.0f,
    };

    static const GLubyte indices[] = {
        0, 1, 2, 0, 2, 3
    };

    gl::BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, ATTRIB_OFFSET(0)); // position
    gl::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, ATTRIB_OFFSET(3)); // uvs
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);

    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    return true;
}

void quad::render() {
    gl::BindVertexArray(vao);
    gl::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}

sphere::sphere()
    : m_indices(0)
{
}

bool sphere::upload() {
    geom::upload();

    constexpr size_t numVertices = (kStacks + 1) * (kSlices + 1);
    u::vector<m::vec3> vertices;
    vertices.resize(numVertices);

    const float ds = 1.0f / kSlices;
    const float dt = 1.0f / kStacks;
    float t = 1.0f;
    for (size_t i = 0; i < kStacks + 1; i++) {
        float s = 0.0f;
        const float rho = m::kPi * (1.0f - t);
        const float sinrho = i && i < kStacks ? sinf(rho) : 0.0f;
        const float cosrho = !i ? 1.0f : (i < kStacks ? cosf(rho) : -1.0f);
        for (size_t j = 0; j < kSlices + 1; j++) {
            float theta = j==kSlices ? 0 : 2*m::kPi*s;
            auto &v = vertices[i*(kSlices+1) + j];
            v = m::vec3(sinf(theta)*sinrho, cosf(theta)*sinrho, -cosrho);
            s += ds;
        }
        t -= dt;
    }

    m_indices = (kStacks - 1) * kSlices * 3 * 2;
    u::vector<GLushort> indices;
    indices.resize(m_indices);
    GLushort *curIndex = &indices[0];
    for (size_t i = 0; i < kStacks; i++) {
        for (size_t k = 0; k < kSlices; k++) {
            size_t j = i%2 ? kSlices-k-1 : k;
            if (i) {
                *curIndex++ = i*(kSlices+1)+j;
                *curIndex++ = (i+1)*(kSlices+1)+j;
                *curIndex++ = i*(kSlices+1)+j+1;
            }
            if (i + 1 < kStacks) {
                *curIndex++ = i*(kSlices+1)+j+1;
                *curIndex++ = (i+1)*(kSlices+1)+j;
                *curIndex++ = (i+1)*(kSlices+1)+j+1;
            }
        }
    }

    gl::BindVertexArray(vao);

    gl::BindBuffer(GL_ARRAY_BUFFER, vbo);
    gl::BufferData(GL_ARRAY_BUFFER, numVertices * sizeof(m::vec3), &vertices[0], GL_STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(m::vec3), ATTRIB_OFFSET(0)); // position
    gl::EnableVertexAttribArray(0);

    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

    return true;
}

void sphere::render() {
    gl::BindVertexArray(vao);
    gl::DrawElements(GL_TRIANGLES, m_indices, GL_UNSIGNED_SHORT, 0);
}

bool bbox::upload() {
    geom::upload();

    // 1x1x1 cube (centered on origin)
    static const GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f
    };

    static const GLubyte indices[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    gl::BindVertexArray(vao);

    gl::BindBuffer(GL_ARRAY_BUFFER, vbo);
    gl::BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    gl::VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    gl::EnableVertexAttribArray(0);

    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    return true;
}

void bbox::render() {
    gl::BindVertexArray(vao);
    gl::DrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, 0);
    gl::DrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)4);
    gl::DrawElements(GL_LINES, 8, GL_UNSIGNED_BYTE, (void*)8);
}

bool cube::upload() {
    geom::upload();

    static const GLfloat vertices[] = {
        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
        -1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
        -1.0,  1.0, -1.0,
         1.0,  1.0, -1.0,
    };

    static const GLubyte indices[] = {
        0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
    };

    gl::BindVertexArray(vao);

    gl::BindBuffer(GL_ARRAY_BUFFER, vbo);
    gl::BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ATTRIB_OFFSET(0));
    gl::EnableVertexAttribArray(0);

    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    return true;
}

void cube::render() {
    gl::BindVertexArray(vao);
    gl::DrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_BYTE, 0);
}

}
