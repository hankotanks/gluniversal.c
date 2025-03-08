#include <GL/glew.h>
#include <RGFW.h>
#include <RFont.h>
#include <stdio.h>
#include "draw.h"

int main() {
    RGFW_window* win = RGFW_createWindow("a window", RGFW_RECT(0, 0, 800, 600), RGFW_windowCenter | RGFW_windowNoResize);
    if(glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return 1;
    }
    while (RGFW_window_shouldClose(win) == RGFW_FALSE) {
        while (RGFW_window_checkEvent(win)) {
            if (win->event.type == RGFW_quit) break;
            if (win->event.type == RGFW_mouseButtonPressed && win->event.button == RGFW_mouseLeft) {
                printf("You clicked at x: %d, y: %d\n", win->event.point.x, win->event.point.y);
            }
        }
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        draw_triangle();
        RGFW_window_swapBuffers(win);
    }
    RGFW_window_close(win);
    return 0;
}