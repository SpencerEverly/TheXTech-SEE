#ifndef LUA_GRAPHICS_HHH
#define LUA_GRAPHICS_HHH

void xtech_lua_textPrint(const std::string &text, float x, float y);
void xtech_lua_textPrint(const std::string &text, float type, float x, float y);
void xtech_lua_textPrintWP(const std::string &text, float x, float y, float priority);
void xtech_lua_textPrintWP(const std::string &text, float type, float x, float y, float priority);

void xtech_lua_graphics_loadImage(std::string directory, std::string img, int A);

void xtech_lua_graphics_drawImage(int A, double x, double y);
void xtech_lua_graphics_drawImage(int A, double x, double y, double width, double height);
void xtech_lua_graphics_drawImage(int A, double x, double y, double width, double height, int xOffset, int yOffset);
void xtech_lua_graphics_drawImage(int A, double x, double y, double width, double height, double opacity);
void xtech_lua_graphics_drawImage(int A, double x, double y, double width, double height, int xOffset, int yOffset, double opacity);

void xtech_lua_graphics_drawRectangle(int x, int y, int w, int h, float color1, float color2, float color3, float opacity, bool filled);
void xtech_lua_graphics_drawRectangle(int x, int y, int w, int h, float color1, float color2, float color3, bool filled);

void xtech_lua_graphics_drawScreen(float color1, float color2, float color3, float opacity, bool filled);
void xtech_lua_graphics_drawScreen(float color1, float color2, float color3, bool filled);

#endif // LUA_GRAPHICS_HHH
