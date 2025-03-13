#ifndef __CTX_H__
#define __CTX_H__

#include <stdbool.h>
#include <stddef.h>
#include <RGFW.h>
#include <microui.h>

typedef struct __CTX_H__Context Context;

Context* Context_init(const char* win_name, RGFW_rect win_rect, RGFW_windowFlags win_flags, unsigned char* font_data, unsigned int font_size, float color_clear[3]);
void Context_free(Context* const ctx);
mu_Context* Context_get(const Context* const ctx);
bool Context_is_open(const Context* const ctx);
RGFW_event* Context_event(const Context* const ctx);
bool Context_frame_start(Context* const ctx);
void Context_frame_end(Context* const ctx);
void Context_render(const Context* const ctx);

#endif // __CTX_H__