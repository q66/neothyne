CXX = $(CC)
CXXFLAGS = \
	-std=c++11 \
	-Wall \
	-Wextra \
	-ffast-math \
	-fno-exceptions \
	-fno-rtti \
	-I. \
	-DDEBUG_GL \
	-O3

ENGINE_CXXFLAGS = \
	$(CXXFLAGS)

ENGINE_LDFLAGS = \
	-lm \
	-ldl

GAME_SOURCES = \
	game/menu.cpp \
	game/client.cpp \
	game/main.cpp \
	game/edit.cpp

MATH_SOURCES = \
	m_mat4.cpp \
	m_quat.cpp \
	m_vec3.cpp \
	m_plane.cpp

RENDERER_SOURCES = \
	r_billboard.cpp \
	r_common.cpp \
	r_gbuffer.cpp \
	r_model.cpp \
	r_method.cpp \
	r_pipeline.cpp \
	r_geom.cpp \
	r_skybox.cpp \
	r_ssao.cpp \
	r_texture.cpp \
	r_world.cpp \
	r_gui.cpp \
	r_light.cpp

UTIL_SOURCES = \
	u_new.cpp \
	u_file.cpp \
	u_sha512.cpp \
	u_string.cpp \
	u_zlib.cpp

ENGINE_SOURCES = \
	engine.cpp \
	kdmap.cpp \
	kdtree.cpp \
	world.cpp \
	mesh.cpp \
	texture.cpp \
	cvar.cpp \
	gui.cpp \
	$(UTIL_SOURCES) \
	$(MATH_SOURCES) \
	$(RENDERER_SOURCES)

GAME_OBJECTS = $(GAME_SOURCES:.cpp=.o) $(ENGINE_SOURCES:.cpp=.o)
GAME_BIN = neothyne
GAME_DIR = game

all: $(GAME_BIN)

$(GAME_BIN): $(GAME_OBJECTS)
	$(CXX) $(GAME_OBJECTS) $(ENGINE_LDFLAGS) -o $@

.cpp.o:
	$(CXX) -MD -c $(ENGINE_CXXFLAGS) $< -o $@
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d

clean:
	rm -f $(GAME_OBJECTS) $(GAME_OBJECTS:.o=.P)
	rm -f $(GAME_BIN)

-include *.P
