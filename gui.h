#ifndef GUI_HDR
#define GUI_HDR
#include <stdint.h>
#include "u_string.h"
#include "u_stack.h"

namespace gui {

enum {
    kMouseButtonLeft = 1 << 0,
    kMouseButtonRight = 1 << 1
};

enum {
    kAlignLeft,
    kAlignCenter,
    kAlignRight
};

enum {
    kCommandRectangle,
    kCommandTriangle,
    kCommandLine,
    kCommandText,
    kCommandScissor,
    kCommandImage
};

struct box {
    int x;
    int y;
    int w;
    int h;
};

struct rectangle : box {
    int r; // Roundness
};

struct triangle : box { };
struct scissor : box { };

struct image : box {
    u::string path;
};

struct text {
    int x;
    int y;
    int align;
    u::string contents;
};

struct line {
    int x[2];
    int y[2];
    int r;
};

struct command {
    int type;
    int flags;
    uint32_t color;
    union {
        line asLine;
        rectangle asRectangle;
        scissor asScissor;
        triangle asTriangle;
    };
    text asText; // Because of u::string
    image asImage; // Because of u::string
};

struct queue {
    static constexpr size_t kCommandQueueSize = 5000;
    const u::stack<command, kCommandQueueSize> &operator()() const;
    void reset();
    void addScissor(int x, int y, int w, int h);
    void addRectangle(float x, float y, float w, float h, uint32_t color);
    void addLine(float x0, float y0, float x1, float y1, float r, uint32_t color);
    void addRectangle(float x, float y, float w, float h, float r, uint32_t color);
    void addTriangle(int x, int y, int w, int h, int flags, uint32_t color);
    void addText(int x, int y, int align, const u::string &contents, uint32_t color);
    void addImage(int x, int y, int w, int h, const u::string &path);
private:
    u::stack<command, kCommandQueueSize> m_commands;
};

inline void queue::reset() {
    m_commands.reset();
}

inline const u::stack<command, queue::kCommandQueueSize> &queue::operator()() const {
    return m_commands;
}

inline constexpr uint32_t RGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255) {
    return R | (G << 8) | (B << 16) | (A << 24);
}

/// Returns true when clicked, false otherwise
bool button(const u::string &contents, bool enabled = true);
/// Returns true when clicked, false otherwise
bool item(const u::string &contents, bool enabled = true);
/// Returns true when clicked, false otherwise
bool check(const u::string &contents, bool checked, bool enabled = true);
/// Returns true when clicked, false otherwise
bool collapse(const u::string &contents, const char *subtext, bool checked, bool enabled = true);
/// A label is left justified text
void label(const u::string &contents);
/// A value is right justified text
void value(const u::string &contents);
/// Returns true when updated, false otherwise, `value' is a reference to the value
/// which is changed, `min' and `'max' define the applicable range for the slider,
/// while `inc' specifies how much the value should increment when sliding or scrolling.
template <typename T>
bool slider(const u::string &contents, T &value, T min, T max, T inc, bool enabled = true);
/// Indent the widget space so that any widgets after the call will be indented
void indent();
/// Dedent the widget space so that any widgets after the vall will be dedented
void dedent();
/// Add vertical separation
void separator();
/// A heading (vertical separation + line)
void heading();
/// Construct a menu of name `contents' at position `x,y' of size `w,h', `value
/// is a reference to a variable which contains the amount of "scroll" of the
/// area if the contents exceed the vertical size specified by `h'.
bool areaBegin(const u::string &contents, int x, int y, int w, int h, int &value, float round = 6.0f);
/// Called when finished the menu, `inc' is the value of which to increase the
/// the `value' as previous referenced in the parent `areaBegin' call when
/// scrolling in the area, while autoScroll will enable automatic scrolling of
/// the area and hide the scroll bar
void areaFinish(int inc = 5, bool autoScroll = false);

// Primitive drawing
void drawLine(float x0, float y0, float x1, float y1, float r, uint32_t color);
void drawLine(float x0, float y0, float x1, float y1, float r, uint32_t color);
void drawRectangle(float x, float y, float w, float h, uint32_t color);
void drawRectangle(float x, float y, float w, float h, float r, uint32_t color);
void drawText(int x, int y, int align, const u::string &contents, uint32_t color);
void drawTriangle(int x, int y, int w, int h, int flags, uint32_t color);
void drawImage(int x, int y, int w, int h, const u::string &path);

const queue &commands();

void begin(int (&mouse)[4]);
void finish();

}
#endif
