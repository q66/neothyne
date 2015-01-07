#include <stdarg.h>

#include <SDL2/SDL.h>

#include "engine.h"
#include "cvar.h"

#include "r_common.h"

#include "u_file.h"
#include "u_misc.h"

struct library {
    library(const char *name);
    ~library();
    bool load(bool (*callback)());
    void *operator()(const char *proc);
private:
    void *m_handle;
    const char *m_name;
};

static library gSDL("SDL2");

// Engine globals
static u::map<int, int> gKeyMap;
static u::string gUserPath;
static u::string gGamePath;
static size_t gScreenWidth = 0;
static size_t gScreenHeight = 0;
static size_t gRefreshRate = 0;
static SDL_Window *gScreen = nullptr;
static frameTimer gTimer;
static u::map<u::string, void (*)()> gBinds;
static u::string gTextInput;
static bool gTextInputting = false;
static mouseState gMouseState;

// maximum resolution is 15360x8640 (8640p) (16:9)
// default resolution is 1024x768 (XGA) (4:3)
static constexpr int kDefaultScreenWidth = 1024;
static constexpr int kDefaultScreenHeight = 768;
static constexpr size_t kRefreshRate = 60;

VAR(int, vid_vsync, "vertical syncronization", -1, kSyncRefresh, kSyncNone);
VAR(int, vid_fullscreen, "toggle fullscreen", 0, 1, 1);
VAR(int, vid_width, "resolution width", 0, 15360, 0);
VAR(int, vid_height, "resolution height", 0, 8640, 0);
VAR(int, vid_maxfps, "cap framerate", 0, 3600, 0);
VAR(u::string, vid_driver, "video driver");

typedef Uint32 (*PFNSDLGETTICKSPROC)();
typedef SDL_bool (*PFNSDLGETRELATIVEMOUSEMODEPROC)();
typedef void (*PFNSDLSETRELATIVEMOUSEMODEPROC)(SDL_bool);
typedef void (*PFNSDLDELAYPROC)(Uint32);
typedef Uint32 (*PFNSDLGETRELATIVEMOUSESTATEPROC)(int*, int*);
typedef void (*PFNSDLWARPMOUSEINWINDOWPROC)(SDL_Window*, int, int);
typedef void (*PFNSDLSETWINDOWTITLEPROC)(SDL_Window*, const char*);
typedef void (*PFNSDLSETWINDOWSIZEPROC)(SDL_Window*, int, int);
typedef int (*PFNSDLINITPROC)(Uint32);
typedef int (*PFNSDLGETDESKTOPDISPLAYMODEPROC)(int, SDL_DisplayMode*);
typedef int (*PFNSDLGETNUMVIDEODISPLAYSPROC)();
typedef int (*PFNSDLGETCURRENTDISPLAYMODEPROC)(int, SDL_DisplayMode*);
typedef int (*PFNSDLSHOWSIMPLEMESSAGEBOXPROC)(Uint32, const char*, const char*, SDL_Window*);
typedef void (*PFNSDLDESTROYWINDOWPROC)(SDL_Window*);
typedef SDL_Window* (*PFNSDLCREATEWINDOWPROC)(const char*, int, int, int, int, Uint32);
typedef void (*PFNSDLQUITPROC)();
typedef void (*PFNSDLSETMAINREADYPROC)();
typedef char* (*PFNSDLGETPREFPATHPROC)(const char*, const char *);
typedef void (*PFNSDLFREEPROC)(void*);
typedef int (*PFNSDLSHOWCURSORPROC)(int);
typedef int (*PFNSDLPOLLEVENTPROC)(SDL_Event*);
typedef void (*PFNSDLSTARTTEXTINPUTPROC)();
typedef void (*PFNSDLSTOPTEXTINPUTPROC)();
typedef const char* (*PFNSDLGETERRORPROC)();
typedef void* (*PFNSDLGLGETPROCADDRESSPROC)(const char*);
typedef void (*PFNSDLGLSWAPWINDOWPROC)(SDL_Window*);
typedef SDL_GLContext (*PFNSDLGLCREATECONTEXTPROC)(SDL_Window*);
typedef int (*PFNSDLGLSETATTRIBUTEPROC)(SDL_GLattr, int);
typedef int (*PFNSDLGLSETSWAPINTERVALPROC)(int);
typedef int (*PFNSDLGLLOADLIBRARYPROC)(const char*);
typedef const char* (*PFNSDLGETKEYNAMEPROC)(SDL_Keycode);

static PFNSDLGETTICKSPROC                SDL_GetTicks_              = nullptr;
static PFNSDLGETRELATIVEMOUSEMODEPROC    SDL_GetRelativeMouseMode_  = nullptr;
static PFNSDLSETRELATIVEMOUSEMODEPROC    SDL_SetRelativeMouseMode_  = nullptr;
static PFNSDLDELAYPROC                   SDL_Delay_                 = nullptr;
static PFNSDLGETRELATIVEMOUSESTATEPROC   SDL_GetRelativeMouseState_ = nullptr;
static PFNSDLWARPMOUSEINWINDOWPROC       SDL_WarpMouseInWindow_     = nullptr;
static PFNSDLSETWINDOWTITLEPROC          SDL_SetWindowTitle_        = nullptr;
static PFNSDLSETWINDOWSIZEPROC           SDL_SetWindowSize_         = nullptr;
static PFNSDLINITPROC                    SDL_Init_                  = nullptr;
static PFNSDLGETDESKTOPDISPLAYMODEPROC   SDL_GetDesktopDisplayMode_ = nullptr;
static PFNSDLGETNUMVIDEODISPLAYSPROC     SDL_GetNumVideoDisplays_   = nullptr;
static PFNSDLGETCURRENTDISPLAYMODEPROC   SDL_GetCurrentDisplayMode_ = nullptr;
static PFNSDLSHOWSIMPLEMESSAGEBOXPROC    SDL_ShowSimpleMessageBox_  = nullptr;
static PFNSDLDESTROYWINDOWPROC           SDL_DestroyWindow_         = nullptr;
static PFNSDLCREATEWINDOWPROC            SDL_CreateWindow_          = nullptr;
static PFNSDLQUITPROC                    SDL_Quit_                  = nullptr;
static PFNSDLSETMAINREADYPROC            SDL_SetMainReady_          = nullptr;
static PFNSDLGETPREFPATHPROC             SDL_GetPrefPath_           = nullptr;
static PFNSDLFREEPROC                    SDL_free_                  = nullptr;
static PFNSDLSHOWCURSORPROC              SDL_ShowCursor_            = nullptr;
static PFNSDLPOLLEVENTPROC               SDL_PollEvent_             = nullptr;
static PFNSDLSTARTTEXTINPUTPROC          SDL_StartTextInput_        = nullptr;
static PFNSDLSTOPTEXTINPUTPROC           SDL_StopTextInput_         = nullptr;
static PFNSDLGETERRORPROC                SDL_GetError_              = nullptr;
static PFNSDLGLGETPROCADDRESSPROC        SDL_GL_GetProcAddress_     = nullptr;
static PFNSDLGLSWAPWINDOWPROC            SDL_GL_SwapWindow_         = nullptr;
static PFNSDLGLCREATECONTEXTPROC         SDL_GL_CreateContext_      = nullptr;
static PFNSDLGLSETATTRIBUTEPROC          SDL_GL_SetAttribute_       = nullptr;
static PFNSDLGLSETSWAPINTERVALPROC       SDL_GL_SetSwapInterval_    = nullptr;
static PFNSDLGLLOADLIBRARYPROC           SDL_GL_LoadLibrary_        = nullptr;
static PFNSDLGETKEYNAMEPROC              SDL_GetKeyName_            = nullptr;

// A way to do dynamic loading of libraries at runtime
#ifdef _WIN32
#define _WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif
library::library(const char *name)
    : m_handle(nullptr)
    , m_name(name)
{
}

library::~library() {
    if (!m_handle)
        return;
#ifdef _WIN32
    FreeLibrary((HINSTANCE)m_handle);
#else
    dlclose(m_handle);
#endif
}

bool library::load(bool (*callback)()) {
#ifdef _WIN32
    m_handle = (void*)LoadLibrary(u::format("%s.dll", m_name).c_str());
#else
    m_handle = dlopen(u::format("lib%s.so", m_name).c_str(), RTLD_NOW);
#endif
    return m_handle && callback();
}

void *library::operator()(const char *proc) {
#ifdef _WIN32
    return (void*)GetProcAddress((HINSTANCE)m_handle, proc);
#else
    return dlsym(m_handle, proc);
#endif
}

static SDL_Window *getContext() {
    SDL_Init_(SDL_INIT_VIDEO);

    const u::string &videoDriver = vid_driver.get();
    if (videoDriver.size()) {
        if (SDL_GL_LoadLibrary_(videoDriver.c_str()) != 0) {
            u::print("Failed to load video driver: %s\n", SDL_GetError_());
            return nullptr;
        } else {
            u::print("Loaded video driver: %s\n", videoDriver);
        }
    }

    // Get the display mode resolution
    SDL_DisplayMode mode;
    if (SDL_GetDesktopDisplayMode_(0, &mode) != 0) {
        // Failed to get the desktop mode
        int displays = SDL_GetNumVideoDisplays_();
        if (displays < 1) {
            // Failed to even get a display, worst case we'll assume a default
            gScreenWidth = kDefaultScreenWidth;
            gScreenHeight = kDefaultScreenHeight;
        } else {
            // Try all the display devices until we find one that actually contains
            // display information
            for (int i = 0; i < displays; i++) {
                if (SDL_GetCurrentDisplayMode_(i, &mode) != 0)
                    continue;
                // Found a display mode
                gScreenWidth = mode.w;
                gScreenHeight = mode.h;
            }
        }
    } else {
        // Use the desktop display mode
        gScreenWidth = mode.w;
        gScreenHeight = mode.h;
    }

    if (gScreenWidth <= 0 || gScreenHeight <= 0) {
        // In this event we fall back to the default resolution
        gScreenWidth = kDefaultScreenWidth;
        gScreenHeight = kDefaultScreenHeight;
    }

    gRefreshRate = (mode.refresh_rate == 0)
        ? kRefreshRate : mode.refresh_rate;

    // Coming from a config
    if (vid_width != 0 && vid_height != 0) {
        gScreenWidth = size_t(vid_width.get());
        gScreenHeight = size_t(vid_height.get());
    }

    SDL_GL_SetAttribute_(SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE | SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute_(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute_(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute_(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute_(SDL_GL_DEPTH_SIZE, 24);

    uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (vid_fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;
    SDL_Window *window = SDL_CreateWindow_("Neothyne",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        gScreenWidth,
        gScreenHeight,
        flags
    );

    if (!window || !SDL_GL_CreateContext_(window)) {
        SDL_ShowSimpleMessageBox_(SDL_MESSAGEBOX_ERROR,
            "Neothyne: Initialization error",
            "OpenGL 3.3 or higher is required",
            nullptr);
        if (window)
            SDL_DestroyWindow_(window);
        SDL_Quit_();
    }

    return window;
}

// An accurate frame rate timer and capper
frameTimer::frameTimer()
    : m_deltaTime(0.0f)
    , m_lastFrameTicks(0)
    , m_currentTicks(0)
    , m_targetTicks(0)
    , m_frameMin(0)
    , m_frameMax(0)
    , m_frameAverage(0.0f)
    , m_framesPerSecond(0)
    , m_lock(false)
{
}

void frameTimer::lock() {
    m_lock = true;
}

void frameTimer::unlock() {
    m_lock = false;
}

void frameTimer::cap(float maxFps) {
    if (m_lock)
        return;
    m_maxFrameTicks = maxFps <= 0.0f
        ? -1 : (1000.0f / maxFps) - kDampenEpsilon;
}

void frameTimer::reset() {
    m_frameCount = 0;
    m_minTicks = 1000;
    m_maxTicks = 0;
    m_averageTicks = 0;
    m_lastSecondTicks = SDL_GetTicks_();
}

bool frameTimer::update() {
    m_frameCount++;
    m_targetTicks = m_maxFrameTicks != -1 ?
        m_lastSecondTicks + uint32_t(m_frameCount * m_maxFrameTicks) : 0;
    m_currentTicks = SDL_GetTicks_();
    m_averageTicks += m_currentTicks - m_lastFrameTicks;
    if (m_currentTicks - m_lastFrameTicks <= m_minTicks)
        m_minTicks = m_currentTicks - m_lastFrameTicks;
    if (m_currentTicks - m_lastFrameTicks >= m_maxTicks)
        m_maxTicks = m_currentTicks - m_lastFrameTicks;
    if (m_targetTicks && m_currentTicks < m_targetTicks) {
        uint32_t beforeDelay = SDL_GetTicks_();
        SDL_Delay_(m_targetTicks - m_currentTicks);
        m_currentTicks = SDL_GetTicks_();
        m_averageTicks += m_currentTicks - beforeDelay;
    }
    m_deltaTime = 0.001f * (m_currentTicks - m_lastFrameTicks);
    m_lastFrameTicks = m_currentTicks;
    if (m_currentTicks - m_lastSecondTicks >= 1000) {
        m_framesPerSecond = m_frameCount;
        m_frameAverage = m_averageTicks / m_frameCount;
        m_frameMin = m_minTicks;
        m_frameMax = m_maxTicks;
        reset();
        return true;
    }
    return false;
}

float frameTimer::mspf() const {
    return m_frameAverage;
}

int frameTimer::fps() const {
    return m_framesPerSecond;
}

float frameTimer::delta() const {
    return m_deltaTime;
}

uint32_t frameTimer::ticks() const {
    return m_currentTicks;
}

u::map<int, int> &neoKeyState(int key, bool keyDown, bool keyUp) {
    if (keyDown)
        gKeyMap[key]++;
    if (keyUp)
        gKeyMap[key] = 0;
    return gKeyMap;
}

void neoMouseDelta(int *deltaX, int *deltaY) {
    if (SDL_GetRelativeMouseMode_() == SDL_TRUE)
        SDL_GetRelativeMouseState_(deltaX, deltaY);
}

mouseState neoMouseState() {
    return gMouseState;
}

void *neoGetProcAddress(const char *proc) {
    return SDL_GL_GetProcAddress_(proc);
}

void neoBindSet(const u::string &what, void (*handler)()) {
    gBinds[what] = handler;
}

void (*neoBindGet(const u::string &what))() {
    if (gBinds.find(what) != gBinds.end())
        return gBinds[what];
    return nullptr;
}

void neoSwap() {
    SDL_GL_SwapWindow_(gScreen);
    gTimer.update();

    auto callBind = [](const char *what) {
        if (gBinds.find(what) != gBinds.end())
            gBinds[what]();
    };

    // Event dispatch
    char format[1024];
    SDL_Event e;
    while (SDL_PollEvent_(&e)) {
        switch (e.type) {
        case SDL_WINDOWEVENT:
            switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                neoResize(e.window.data1, e.window.data2);
                break;
            }
            break;
        case SDL_KEYDOWN:
            snprintf(format, sizeof(format), "%sDn", SDL_GetKeyName_(e.key.keysym.sym));
            callBind(format);
            neoKeyState(e.key.keysym.sym, true);
            // Engine console keys
            switch (e.key.keysym.sym) {
            case SDLK_BACKSPACE:
                if (gTextInputting)
                    gTextInput.pop_back();
                break;
            case SDLK_SLASH:
                gTextInputting = !gTextInputting;
                if (gTextInputting)
                    SDL_StartTextInput_();
                else
                    SDL_StopTextInput_();
                gTextInput = "";
                break;
            }
            break;
        case SDL_KEYUP:
            snprintf(format, sizeof(format), "%sUp", SDL_GetKeyName_(e.key.keysym.sym));
            callBind(format);
            neoKeyState(e.key.keysym.sym, false, true);
            break;
        case SDL_MOUSEMOTION:
            gMouseState.x = e.motion.x;
            gMouseState.y = neoHeight() - e.motion.y;
            break;
        case SDL_MOUSEWHEEL:
            gMouseState.wheel = e.wheel.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (e.button.button) {
            case SDL_BUTTON_LEFT:
                callBind("MouseDnL");
                gMouseState.button |= mouseState::kMouseButtonLeft;
                break;
            case SDL_BUTTON_RIGHT:
                callBind("MouseDnR");
                gMouseState.button |= mouseState::kMouseButtonRight;
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (e.button.button) {
            case SDL_BUTTON_LEFT:
                callBind("MouseUpL");
                gMouseState.button &= ~mouseState::kMouseButtonLeft;
                break;
            case SDL_BUTTON_RIGHT:
                callBind("MouseUpR");
                gMouseState.button &= ~mouseState::kMouseButtonRight;
                break;
            }
            break;
        case SDL_TEXTINPUT:
            gTextInput += e.text.text;
            break;
        }
    }
}

size_t neoWidth() {
    return gScreenWidth;
}

size_t neoHeight() {
    return gScreenHeight;
}

void neoRelativeMouse(bool state) {
    SDL_SetRelativeMouseMode_(state ? SDL_TRUE : SDL_FALSE);
}

bool neoRelativeMouse() {
    return SDL_GetRelativeMouseMode_() == SDL_TRUE;
}

void neoCenterMouse() {
    SDL_WarpMouseInWindow_(gScreen, gScreenWidth / 2, gScreenHeight / 2);
}

void neoSetWindowTitle(const char *title) {
    SDL_SetWindowTitle_(gScreen, title);
}

void neoResize(size_t width, size_t height) {
    SDL_SetWindowSize_(gScreen, width, height);
    gScreenWidth = width;
    gScreenHeight = height;
    gl::Viewport(0, 0, width, height);
    vid_width.set(width);
    vid_height.set(height);
}

void neoSetVSyncOption(int option) {
    switch (option) {
        case kSyncTear:
            if (SDL_GL_SetSwapInterval_(-1) == -1)
                neoSetVSyncOption(kSyncRefresh);
            break;
        case kSyncNone:
            SDL_GL_SetSwapInterval_(0);
            break;
        case kSyncEnabled:
            SDL_GL_SetSwapInterval_(1);
            break;
        case kSyncRefresh:
            SDL_GL_SetSwapInterval_(0);
            gTimer.unlock();
            gTimer.cap(gRefreshRate);
            gTimer.lock();
            break;
    }
    gTimer.reset();
}

const u::string &neoUserPath() { return gUserPath; }
const u::string &neoGamePath() { return gGamePath; }

void neoFatalError(const char *error) {
    SDL_ShowSimpleMessageBox_(SDL_MESSAGEBOX_ERROR, "Neothyne: Fatal error", error, nullptr);
    abort();
}

// So we don't need to depend on SDL_main we provide our own
#ifdef _WIN32
#include <ctype.h>
#include "u_vector.h"
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw) {
    (void)hInst;
    (void)hPrev;
    (void)szCmdLine;
    (void)sw;

    auto parseCommandLine = [](const char *src, u::vector<char *> &args) {
        char *buf = new char[strlen(src) + 1];
        char *dst = buf;
        for (;;) {
            while (isspace(*src))
                src++;
            if (!*src)
                break;
            args.push_back(dst);
            for (bool quoted = false; *src && (quoted || !isspace(*src)); src++) {
                if (*src != '"')
                    *dst++ = *src;
                else if (dst > buf && src[-1] == '\\')
                    dst[-1] = '"';
                else
                    quoted = !quoted;
            }
            *dst++ = '\0';
        }
        args.push_back(nullptr);
        return buf;
    };
    u::vector<char *> args;
    char *buf = parseCommandLine(GetCommandLine(), args);
    SDL_SetMainReady_();
    int status = SDL_main(args.size() - 1, &args[0]);
    delete[] buf;
    exit(status);
    return 0;
}
#endif

static void verifyPaths(int argc, char **argv) {
    // Check for command line '-gamedir'
    --argc;
    const char *directory = nullptr;
    for (int i = 0; i < argc - 1; i++) {
        if (!strcmp(argv[i + 1], "-gamedir") && argv[i + 2]) {
            directory = argv[i + 2];
            break;
        }
    }

    gGamePath = directory ? directory : u::format(".%cgame%c", PATH_SEP, PATH_SEP);
    if (gGamePath.find(PATH_SEP) == u::string::npos)
        gGamePath += PATH_SEP;

    // Get a path for the user
    char *get = SDL_GetPrefPath_("Neothyne", "");
    gUserPath = get;
    gUserPath.pop_back(); // Remove additional path separator
    SDL_free_(get);

    // Verify all the paths exist for the user directory. If they don't exist
    // create them.
    static const char *paths[] = {
        "screenshots", "cache"
    };

    for (size_t i = 0; i < sizeof(paths)/sizeof(*paths); i++) {
        u::string path = gUserPath + paths[i];
        if (u::exists(path, u::kDirectory))
            continue;
        u::mkdir(path);
    }
}

///
/// On Window the entry point is entered as such:
///     WinMain -> SDL_main -> main -> neoMain
/// For everywhere else:
///     main -> neoMain
///
extern int neoMain(frameTimer &timer, int argc, char **argv);
int main(int argc, char **argv) {
    // Load SDL
    if (!gSDL.load([]() -> bool {
        if (!(SDL_GetTicks_              = (PFNSDLGETTICKSPROC)gSDL("SDL_GetTicks")))                           return false;
        if (!(SDL_GetRelativeMouseMode_  = (PFNSDLGETRELATIVEMOUSEMODEPROC)gSDL("SDL_GetRelativeMouseMode")))   return false;
        if (!(SDL_SetRelativeMouseMode_  = (PFNSDLSETRELATIVEMOUSEMODEPROC)gSDL("SDL_SetRelativeMouseMode")))   return false;
        if (!(SDL_Delay_                 = (PFNSDLDELAYPROC)gSDL("SDL_Delay")))                                 return false;
        if (!(SDL_GetRelativeMouseState_ = (PFNSDLGETRELATIVEMOUSESTATEPROC)gSDL("SDL_GetRelativeMouseState"))) return false;
        if (!(SDL_WarpMouseInWindow_     = (PFNSDLWARPMOUSEINWINDOWPROC)gSDL("SDL_WarpMouseInWindow")))         return false;
        if (!(SDL_SetWindowTitle_        = (PFNSDLSETWINDOWTITLEPROC)gSDL("SDL_SetWindowTitle")))               return false;
        if (!(SDL_SetWindowSize_         = (PFNSDLSETWINDOWSIZEPROC)gSDL("SDL_SetWindowSize")))                 return false;
        if (!(SDL_Init_                  = (PFNSDLINITPROC)gSDL("SDL_Init")))                                   return false;
        if (!(SDL_GetDesktopDisplayMode_ = (PFNSDLGETDESKTOPDISPLAYMODEPROC)gSDL("SDL_GetDesktopDisplayMode"))) return false;
        if (!(SDL_GetNumVideoDisplays_   = (PFNSDLGETNUMVIDEODISPLAYSPROC)gSDL("SDL_GetNumVideoDisplays")))     return false;
        if (!(SDL_GetCurrentDisplayMode_ = (PFNSDLGETCURRENTDISPLAYMODEPROC)gSDL("SDL_GetCurrentDisplayMode"))) return false;
        if (!(SDL_ShowSimpleMessageBox_  = (PFNSDLSHOWSIMPLEMESSAGEBOXPROC)gSDL("SDL_ShowSimpleMessageBox")))   return false;
        if (!(SDL_DestroyWindow_         = (PFNSDLDESTROYWINDOWPROC)gSDL("SDL_DestroyWindow")))                 return false;
        if (!(SDL_CreateWindow_          = (PFNSDLCREATEWINDOWPROC)gSDL("SDL_CreateWindow")))                   return false;
        if (!(SDL_Quit_                  = (PFNSDLQUITPROC)gSDL("SDL_Quit")))                                   return false;
        if (!(SDL_SetMainReady_          = (PFNSDLSETMAINREADYPROC)gSDL("SDL_SetMainReady")))                   return false;
        if (!(SDL_GetPrefPath_           = (PFNSDLGETPREFPATHPROC)gSDL("SDL_GetPrefPath")))                     return false;
        if (!(SDL_free_                  = (PFNSDLFREEPROC)gSDL("SDL_free")))                                   return false;
        if (!(SDL_ShowCursor_            = (PFNSDLSHOWCURSORPROC)gSDL("SDL_ShowCursor")))                       return false;
        if (!(SDL_PollEvent_             = (PFNSDLPOLLEVENTPROC)gSDL("SDL_PollEvent")))                         return false;
        if (!(SDL_StartTextInput_        = (PFNSDLSTARTTEXTINPUTPROC)gSDL("SDL_StartTextInput")))               return false;
        if (!(SDL_StopTextInput_         = (PFNSDLSTOPTEXTINPUTPROC)gSDL("SDL_StopTextInput")))                 return false;
        if (!(SDL_GetError_              = (PFNSDLGETERRORPROC)gSDL("SDL_GetError")))                           return false;
        if (!(SDL_GL_GetProcAddress_     = (PFNSDLGLGETPROCADDRESSPROC)gSDL("SDL_GL_GetProcAddress")))          return false;
        if (!(SDL_GL_SwapWindow_         = (PFNSDLGLSWAPWINDOWPROC)gSDL("SDL_GL_SwapWindow")))                  return false;
        if (!(SDL_GL_CreateContext_      = (PFNSDLGLCREATECONTEXTPROC)gSDL("SDL_GL_CreateContext")))            return false;
        if (!(SDL_GL_SetAttribute_       = (PFNSDLGLSETATTRIBUTEPROC)gSDL("SDL_GL_SetAttribute")))              return false;
        if (!(SDL_GL_SetSwapInterval_    = (PFNSDLGLSETSWAPINTERVALPROC)gSDL("SDL_GL_SetSwapInterval")))        return false;
        if (!(SDL_GL_LoadLibrary_        = (PFNSDLGLLOADLIBRARYPROC)gSDL("SDL_GL_LoadLibrary")))                return false;
        if (!(SDL_GetKeyName_            = (PFNSDLGETKEYNAMEPROC)gSDL("SDL_GetKeyName")))                       return false;
        return true;
    })) {
        fprintf(stderr, "failed to load SDL2\n");
        return -1;
    }

    // Now try loading all the symbols

    // Must come after SDL is loaded
    gTimer.reset();
    gTimer.cap(frameTimer::kMaxFPS);

    verifyPaths(argc, argv);

    readConfig();

    gScreen = getContext();

    neoSetVSyncOption(vid_vsync);
    gTimer.cap(vid_maxfps.get());

    SDL_ShowCursor_(0);

    gl::init();

    // back face culling
    gl::FrontFace(GL_CW);
    gl::CullFace(GL_BACK);
    gl::Enable(GL_CULL_FACE);

    gl::Enable(GL_LINE_SMOOTH);
    gl::Hint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    const char *vendor = (const char *)gl::GetString(GL_VENDOR);
    const char *renderer = (const char *)gl::GetString(GL_RENDERER);
    const char *version = (const char *)gl::GetString(GL_VERSION);
    const char *shader = (const char *)gl::GetString(GL_SHADING_LANGUAGE_VERSION);

    u::print("Vendor: %s\nRenderer: %s\nDriver: %s\nShading: %s\n",
        vendor, renderer, version, shader);
    u::print("Game: %s\nUser: %s\n", neoGamePath(), neoUserPath());

    int status = neoMain(gTimer, argc, argv);
    SDL_Quit_();
    return status;
}
