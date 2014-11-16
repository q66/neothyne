#include "r_geom.h"

#include "m_vec3.h"
#include "m_const.h"

#include "u_vector.h"

namespace r {

geom::geom()
    : m_vbo(0)
    , m_ibo(0)
    , m_vao(0)
{
    gl::GenVertexArrays(1, &m_vao);
    gl::GenBuffers(2, m_buffers);
}

geom::~geom() {
    if (m_buffers[0])
        gl::DeleteBuffers(2, m_buffers);
    if (m_vao)
        gl::DeleteVertexArrays(1, &m_vao);
}

void geom::render() {
    gl::BindVertexArray(m_vao);
    gl::DrawElements(m_mode, m_count, m_type, 0);
}

bool quad::upload() {
    gl::BindVertexArray(m_vao);
    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);

    static const GLfloat vertices[] = {
        -1.0f,-1.0f, 0.0f, 0.0f,  0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
         1.0f, 1.0f, 0.0f, 1.0f, -1.0f,
         1.0f,-1.0f, 0.0f, 1.0f,  0.0f,
    };

    static const GLubyte indices[] = { 0, 1, 2, 0, 2, 3 };

    gl::BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, ATTRIB_OFFSET(0)); // position
    gl::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, ATTRIB_OFFSET(3)); // uvs
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);

    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    m_mode = GL_TRIANGLES;
    m_count = 6;
    m_type = GL_UNSIGNED_BYTE;

    return true;
}

bool sphere::upload() {
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

    constexpr size_t numIndices = (kStacks - 1) * kSlices * 3 * 2;
    u::vector<GLushort> indices;
    indices.resize(numIndices);
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

    gl::BindVertexArray(m_vao);

    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferData(GL_ARRAY_BUFFER, numVertices * sizeof(m::vec3), &vertices[0], GL_STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(m::vec3), ATTRIB_OFFSET(0)); // position
    gl::EnableVertexAttribArray(0);

    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

    m_mode = GL_TRIANGLES;
    m_count = numIndices;
    m_type = GL_UNSIGNED_SHORT;

    return true;
}

}