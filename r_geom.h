#ifndef R_GEOM_HDR
#define R_GEOM_HDR
#include "r_common.h"

namespace r {

struct geom {
    geom();
    ~geom();
    void upload();

    union {
        struct {
            GLuint vbo;
            GLuint ibo;
        };
        GLuint buffers[2];
    };
    GLuint vao;
};

struct quad : geom {
    bool upload();
    void render();
};

struct sphere : geom {
    sphere();
    bool upload();
    void render();

private:
    size_t m_indices;
    static constexpr size_t kSlices = 8;
    static constexpr size_t kStacks = 4;
};

struct bbox : geom {
    bool upload();
    void render();
};

struct cube : geom {
    bool upload();
    void render();
};

}

#endif
