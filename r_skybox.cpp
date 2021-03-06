#include "cvar.h"
#include "world.h"

#include "r_skybox.h"
#include "r_pipeline.h"

#include "u_string.h"
#include "u_vector.h"
#include "u_misc.h"

namespace r {
///! method
bool skyboxMethod::init(const u::vector<const char *> &defines) {
    if (!method::init())
        return false;

    for (auto &it : defines)
        method::define(it);

    if (!addShader(GL_VERTEX_SHADER, "shaders/skybox.vs"))
        return false;
    if (!addShader(GL_FRAGMENT_SHADER, "shaders/skybox.fs"))
        return false;
    if (!finalize({ { 0, "position" } }))
        return false;

    m_WVPLocation = getUniformLocation("gWVP");
    m_worldLocation = getUniformLocation("gWorld");
    m_cubeMapLocation = getUniformLocation("gCubemap");
    m_skyColor = getUniformLocation("gSkyColor");
    m_fogLocation.color = getUniformLocation("gFog.color");
    m_fogLocation.density = getUniformLocation("gFog.density");

    return true;
}

void skyboxMethod::setWVP(const m::mat4 &wvp) {
    gl::UniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, wvp.ptr());
}

void skyboxMethod::setTextureUnit(int unit) {
    gl::Uniform1i(m_cubeMapLocation, unit);
}

void skyboxMethod::setWorld(const m::mat4 &worldInverse) {
    gl::UniformMatrix4fv(m_worldLocation, 1, GL_TRUE, worldInverse.ptr());
}

void skyboxMethod::setFog(const fog &f) {
    gl::Uniform3fv(m_fogLocation.color, 1, &f.color.x);
    gl::Uniform1f(m_fogLocation.density, f.density);
}

void skyboxMethod::setSkyColor(const m::vec3 &skyColor) {
    gl::Uniform3fv(m_skyColor, 1, &skyColor.x);
}

///! renderer
bool skybox::load(const u::string &skyboxName) {
    if (!m_cubemap.load(skyboxName + "_ft", skyboxName + "_bk", skyboxName + "_up",
                        skyboxName + "_dn", skyboxName + "_rt", skyboxName + "_lf"))
        return false;

    // Calculate the average color of the top of the skybox. We utilize this color
    // for vertical fog mixture that reaches into the sky if the map has fog at
    // all.
    const auto &tex = m_cubemap.get(texture3D::kUp);
    const auto &data = tex.data();

    uint32_t totals[3] = {0};
    const size_t stride = tex.width() * tex.bpp();
    for (size_t y = 0; y < tex.height(); y++) {
        for (size_t x = 0; x < tex.width(); x++) {
            for (size_t i = 0; i < 3; i++) {
                const size_t index = (y * stride) + x*tex.bpp() + i;
                totals[i] += data[index];
            }
        }
    }
    int reduce[3] = {0};
    for (size_t i = 0; i < 3; i++)
        reduce[i] = totals[i] / (tex.width() * tex.height());

    m_skyColor = m::vec3(reduce[0] / 255.0f,
                         reduce[1] / 255.0f,
                         reduce[2] / 255.0f);

    return true;
}

bool skybox::upload() {
    if (!m_cube.upload())
        return false;
    if (!m_cubemap.upload())
        return false;
    if (!m_methods[0].init())
        return false;
    if (!m_methods[1].init({"USE_FOG"}))
        return false;
    for (auto &it : m_methods) {
        it.enable();
        it.setTextureUnit(0);
        it.setSkyColor(m_skyColor);
    }
    return true;
}

void skybox::render(const pipeline &pl, const fog &f) {
    // Construct the matrix for the skybox
    pipeline wpl = pl;
    pipeline p;
    p.setWorld(pl.position());
    p.setPosition(pl.position());
    p.setRotation(pl.rotation());
    p.setPerspective(pl.perspective());

    skyboxMethod *renderMethod = nullptr;
    if (varGet<int>("r_fog")) {
        renderMethod = &m_methods[1];
        renderMethod->enable();
        renderMethod->setFog(f);
    } else {
        renderMethod = &m_methods[0];
        renderMethod->enable();
    }

    renderMethod->setWVP(p.projection() * p.view() * p.world());
    renderMethod->setWorld(wpl.world());

    // render skybox cube
    gl::DepthFunc(GL_LEQUAL);
    gl::CullFace(GL_FRONT);

    m_cubemap.bind(GL_TEXTURE0); // bind cubemap texture

    m_cube.render();

    gl::DepthFunc(GL_LESS);
    gl::CullFace(GL_BACK);
}

}
