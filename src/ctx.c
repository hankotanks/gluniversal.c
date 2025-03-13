#include <GL/glew.h>
#include "ctx.h"
#include <stdlib.h>
#include <stdbool.h>
#include <RGFW.h>
#include <RFont.h>
#include <microui.h>

typedef struct {
    u32 size;
    RFont_font* inner;
} ContextFont;

ContextFont* ContextFont_init(unsigned char* font_data, unsigned int font_size) {
    ContextFont* font = malloc(sizeof *font);
    if(font == NULL) return NULL;
    font->size = (u32) font_size;
    font->inner = RFont_font_init_data((uint8_t*) font_data, 0);
    if(font->inner == NULL) {
        free(font);
        return NULL;
    }
    return font;
}

void ContextFont_free(ContextFont* const font) {
    RFont_font_free(font->inner);
    free(font);
}

void ContextFont_draw_text(const ContextFont* const font, char* text, float x, float y) {
    RFont_draw_text(font->inner, text, x, y, font->size);
}

int ContextFont_text_height(mu_Font font) {
    ContextFont* temp = (ContextFont*) font;
    RFont_area area = RFont_text_area(temp->inner, "Hello, world!", temp->size);
    return (int) area.h;
}

int ContextFont_text_width(mu_Font font, const char* text, int len) {
    ContextFont* temp = (ContextFont*) font;
    if(len < 0) return 0;
    RFont_area area = RFont_text_area_len(temp->inner, text, (size_t) len, temp->size, 0, 0.f);
    return (int) area.w;
}

struct __CTX_H__Context {
    RGFW_window* win;
    mu_Context* inner; 
    GLclampf color_clear[3];
};

Context* Context_init(const char* win_name, RGFW_rect win_rect, RGFW_windowFlags win_flags, unsigned char* font_data, unsigned int font_size, float color_clear[3]) {
    // allocate the Context
    Context* ctx = malloc(sizeof *ctx);
    if(ctx == NULL) return NULL;
    // RGFW
    ctx->win = RGFW_createWindow(win_name, win_rect, win_flags);
    if(ctx->win == NULL) {
        free(ctx);
        return NULL;
    }
    // RFont
    if((ctx->win->r.w * ctx->win->r.h) < 0) {
        RGFW_window_close(ctx->win);
        free(ctx);
        return NULL;
    };
    RFont_init((size_t) ctx->win->r.w, (size_t) ctx->win->r.h);
    ContextFont* font = ContextFont_init(font_data, font_size);
    // microui
    ctx->inner = malloc(sizeof *(ctx->inner));
    if(ctx->inner == NULL) {
        RGFW_window_close(ctx->win);
        ContextFont_free(font);
        free(ctx);
        return NULL;
    }
    mu_init(ctx->inner);
    ctx->inner->style->font = font;
    ctx->inner->style->size.y = font_size;
    ctx->inner->text_width = ContextFont_text_width;
    ctx->inner->text_height = ContextFont_text_height;
    ctx->color_clear[0] = (GLclampf) color_clear[0];
    ctx->color_clear[1] = (GLclampf) color_clear[1];
    ctx->color_clear[2] = (GLclampf) color_clear[2];
    glClearColor(ctx->color_clear[0], ctx->color_clear[1], ctx->color_clear[2], (GLclampf) 1.f);
    return ctx;
}

void Context_free(Context* const ctx) {
    RGFW_window_close(ctx->win);
    ContextFont_free((ContextFont*) ctx->inner->style->font);
    free(ctx->inner);
    free(ctx);
}

mu_Context* Context_get(const Context* const ctx) {
    return ctx->inner;
}

bool Context_is_open(const Context* const ctx) {
    return (RGFW_window_shouldClose(ctx->win) == RGFW_FALSE);
}

RGFW_event* Context_event(const Context* const ctx) {
    return RGFW_window_checkEvent(ctx->win);
}

bool Context_frame_start(Context* const ctx) {
    int mx, my, mb, kc;
    mx = (int) ctx->win->event.point.x;
    my = (int) ctx->win->event.point.y;
    mb = (int) ctx->win->event.button;
    switch(mb) {
        case RGFW_mouseLeft: mb = MU_MOUSE_LEFT; break;
        case RGFW_mouseRight: mb = MU_MOUSE_RIGHT; break;
        case RGFW_mouseScrollUp | RGFW_mouseScrollDown: mb = MU_MOUSE_MIDDLE; break;
    }
    kc = (int) ctx->win->event.key;
    while(RGFW_window_checkEvent(ctx->win)) {
        switch(ctx->win->event.type) {
            case RGFW_windowResized:
                if((ctx->win->r.w * ctx->win->r.h) < 0) return true;
                glViewport(0, 0, (GLsizei) ctx->win->r.w, (GLsizei) ctx->win->r.h);
                RFont_update_framebuffer((size_t) ctx->win->r.w, (size_t) ctx->win->r.h);
                break;
            case RGFW_mousePosChanged:
                mu_input_mousemove(ctx->inner, mx, my);
                break;
            case RGFW_keyPressed:
                mu_input_keydown(ctx->inner, kc);
                break;
            case RGFW_keyReleased:
                mu_input_keyup(ctx->inner, kc);
                break;
            case RGFW_mouseButtonPressed:
                if(mb == RGFW_mouseScrollUp || mb == RGFW_mouseScrollDown) {
                    mu_input_scroll(ctx->inner, 0, (ctx->win->event.scroll > 0.0) ? 1 : -1);
                } else {
                    mu_input_mousedown(ctx->inner, mx, my, mb);
                    printf("%d, %d\n", mx, my);
                }
                break;
            case RGFW_mouseButtonReleased:
                if(mb != RGFW_mouseScrollUp && mb != RGFW_mouseScrollDown) mu_input_mousedown(ctx->inner, mx, my, mb);
                break;
            default:
                // mu_input_text(mu_Context *ctx, const char *text)
                // TODO: text input
                break;
        }
    }
    glClear(GL_COLOR_BUFFER_BIT);
    mu_begin(ctx->inner);
    return false;
}

void Context_frame_end(Context* const ctx) {
    mu_end(ctx->inner);
    GLclampf r, g, b, a;
    GLint x, y;
    GLsizei w, h;
    mu_Command* cmd = NULL;
    while(mu_next_command(ctx->inner, &cmd)) {
        switch(cmd->type) {
            case MU_COMMAND_TEXT:
                ContextFont_draw_text((ContextFont*) ctx->inner->style->font, cmd->text.str, (float) cmd->text.pos.x, (float) cmd->text.pos.y);
                break;
            case MU_COMMAND_RECT:
                glEnable(GL_SCISSOR_TEST);
                x = (GLint) cmd->rect.rect.x;
                y = (GLint) ((int) ctx->win->r.h - cmd->rect.rect.h - cmd->rect.rect.y);
                w = (GLsizei) cmd->rect.rect.w;
                h = (GLsizei) cmd->rect.rect.h;
                glScissor(x, y, w, h);
                r = (GLclampf) (((float) cmd->rect.color.r) / 255.f);
                g = (GLclampf) (((float) cmd->rect.color.g) / 255.f);
                b = (GLclampf) (((float) cmd->rect.color.b) / 255.f);
                a = (GLclampf) (((float) cmd->rect.color.a) / 255.f);
                glClearColor(r, g, b, a);
                glClear(GL_COLOR_BUFFER_BIT);
                r = ctx->color_clear[0];
                g = ctx->color_clear[1];
                b = ctx->color_clear[2];
                glClearColor(r, g, b, (GLclampf) 1.f);
                glDisable(GL_SCISSOR_TEST);
                break;
            case MU_COMMAND_ICON:
                break;
            case MU_COMMAND_CLIP:
                // glEnable(GL_SCISSOR_TEST);
                // x = (GLint) cmd->rect.rect.x;
                // y = (GLint) cmd->rect.rect.y;
                // w = (GLsizei) cmd->rect.rect.w;
                // h = (GLsizei) cmd->rect.rect.h;
                // glScissor(x, y, w, h);
                // glClear(GL_COLOR_BUFFER_BIT);
                // glDisable(GL_SCISSOR_TEST);
                break;
        }
    }
    RGFW_window_swapBuffers(ctx->win);
}