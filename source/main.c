/*
 * HL-DEMO — original FPS ray-caster in the spirit of Half-Life for old 3DS.
 * All assets/art are original; no Valve content is used.
 */

#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "poster.h"

#define SCR_W 400
#define SCR_H 240
#define MAP_W 24
#define MAP_H 24

#define TEX_W 64
#define TEX_H 64

#define NUM_TEX 9

#define FOV 66.0

#define COLOR_RGB(r,g,b)  (((u32)(r) << 16) | ((u32)(g) << 8) | (u32)(b))

typedef struct {
    double x, y, dx, dy, pa;
} Player;

/* map layout */
static int worldMap[MAP_W][MAP_H] = {
    {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,9,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,8},
    {8,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,8},
    {8,0,0,1,0,4,4,0,1,0,0,0,0,0,1,0,2,2,2,0,1,0,0,8},
    {8,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,8},
    {8,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,3,0,0,3,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
    {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
};

static u32 texture[NUM_TEX][TEX_W * TEX_H];

/* original procedural textures (no copyrighted assets) */
static void genTextures(void) {
    for (int t = 0; t < NUM_TEX; t++) {
        for (int y = 0; y < TEX_H; y++) {
            for (int x = 0; x < TEX_W; x++) {
                u8 r, g, b;
                switch (t) {
                    case 0: { /* metal panels */
                        r = 60 + (x * 7 % 30); g = 66; b = 72;
                        if ((x % 32) < 2 || (y % 32) < 2) { r = 30; g = 34; b = 38; }
                    } break;
                    case 1: { /* hazard stripes */
                        int band = (x + y) % 48;
                        r = band < 24 ? 190 : 200; g = band < 24 ? 170 : 180; b = 30;
                    } break;
                    case 2: { /* lab tiles */
                        if ((x/8)%2 == (y/8)%2) { r = 210; g = 214; b = 220; }
                        else                    { r = 150; g = 156; b = 165; }
                    } break;
                    case 3: { /* door metal */
                        r = 120; g = 124; b = 130;
                        if ((y % 32) < 2) { r = 200; g = 204; b = 210; }
                    } break;
                    case 4: { /* glass panel */
                        int p = ((x / 16) + (y / 16)) % 2;
                        r = p ? 120 : 90; g = 150; b = 170;
                    } break;
                    case 8: { /* concrete wall */
                        r = 96 + ((x * 31 + y * 17) % 12);
                        g = 92 + ((x * 17 + y * 29) % 12);
                        b = 88 + ((x * 23 + y * 11) % 12);
                    } break;
                    default: { /* safety red/blue panels */
                        r = (t % 2) ? 180 : 60;
                        g = 60;
                        b = (t % 2) ? 60 : 170;
                    } break;
                }
                texture[t][y * TEX_W + x] = COLOR_RGB(r, g, b);
            }
        }
    }
}

static void putPixel(u8 *fb, int px, int py, u32 color) {
    if (px < 0 || px >= SCR_W || py < 0 || py >= SCR_H) return;
    int pitch = SCR_W;
    fb[(py * pitch + px) * 3]     = color & 0xFF;
    fb[(py * pitch + px) * 3 + 1] = (color >> 8) & 0xFF;
    fb[(py * pitch + px) * 3 + 2] = (color >> 16) & 0xFF;
}

int main(void) {
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);
    gfxSetDoubleBuffering(GFX_TOP, true);

    genTextures();

    Player p;
    p.x = 12.0; p.y = 12.0; p.pa = 0.3; p.dx = cos(p.pa); p.dy = sin(p.pa);

    printf("HL-DEMO (raycaster FPS)\n");
    printf("Circle/WASD-equip: move\n");
    printf("A: interact, R: run\n");
    printf("Press A to start...\n");
    while (aptMainLoop()) {
        hidScanInput();
        u32 k = hidKeysDown();
        if (k & KEY_A) break;
        if (k & KEY_START) return 0;
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForEvent(GSPGPU_EVENT_VBlank0);
    }
    consoleClear();

    while (aptMainLoop()) {
        hidScanInput();
        u32 keys = hidKeysHeld();
        u32 down = hidKeysDown();

        double move = (keys & KEY_L) ? 0.06 : 0.03;
        if (keys & KEY_CPAD_RIGHT) p.pa += 0.045;
        if (keys & KEY_CPAD_LEFT)  p.pa -= 0.045;
        if (keys & KEY_CPAD_UP)    { p.x += p.dx * move; p.y += p.dy * move; }
        if (keys & KEY_CPAD_DOWN)  { p.x -= p.dx * move; p.y -= p.dy * move; }
        if (keys & KEY_A)          { /* interact (placeholder) */ }
        p.dx = cos(p.pa); p.dy = sin(p.pa);

        /* collision: keep inside walkable cells */
        if (worldMap[(int)p.x][(int)p.y] > 0) { p.x -= p.dx * move; p.y -= p.dy * move; }
        if (p.x < 1) p.x = 1; if (p.x > MAP_W-2) p.x = MAP_W-2;
        if (p.y < 1) p.y = 1; if (p.y > MAP_H-2) p.y = MAP_H-2;

        u8 *fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

        double planeX = -p.dy * tan(FOV * M_PI / 360.0);
        double planeY =  p.dx * tan(FOV * M_PI / 360.0);

        for (int x = 0; x < SCR_W; x++) {
            double cameraX = 2.0 * x / (double)SCR_W - 1.0;
            double rdx = p.dx + planeX * cameraX;
            double rdy = p.dy + planeY * cameraX;

            int mapX = (int)p.x;
            int mapY = (int)p.y;
            double deltaDistX = fabs(rdx) < 1e-9 ? 1e9 : fabs(1.0/rdx);
            double deltaDistY = fabs(rdy) < 1e-9 ? 1e9 : fabs(1.0/rdy);
            int stepX, stepY;
            double sideDistX, sideDistY;
            int hit = 0, side = 0;

            if (rdx < 0) { stepX = -1; sideDistX = (p.x - mapX) * deltaDistX; }
            else         { stepX =  1; sideDistX = (mapX + 1.0 - p.x) * deltaDistX; }
            if (rdy < 0) { stepY = -1; sideDistY = (p.y - mapY) * deltaDistY; }
            else         { stepY =  1; sideDistY = (mapY + 1.0 - p.y) * deltaDistY; }

            while (!hit) {
                if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
                else                       { sideDistY += deltaDistY; mapY += stepY; side = 1; }
                if (worldMap[mapX][mapY] > 0) hit = 1;
            }

            double perpDist = side ? sideDistY - deltaDistY : sideDistX - deltaDistX;

            int lineH = (int)(SCR_H / perpDist);
            int drawStart = -lineH / 2 + SCR_H / 2;
            if (drawStart < 0) drawStart = 0;
            int drawEnd = lineH / 2 + SCR_H / 2;
            if (drawEnd >= SCR_H) drawEnd = SCR_H - 1;

            int texN = worldMap[mapX][mapY] & 0x7;
            int isPoster = (worldMap[mapX][mapY] == 9);
            double wallX = side ? p.x + perpDist * rdx : p.y + perpDist * rdy;
            wallX -= floor(wallX);
            int texX = (int)(wallX * TEX_W);
            if ((side == 0 && rdx > 0) || (side == 1 && rdy < 0)) texX = TEX_W - texX - 1;

            for (int y = drawStart; y <= drawEnd; y++) {
                int d = y * 256 - SCR_H * 128 + lineH * 128;
                int texY = ((d * TEX_H) / lineH) / 256;
                u32 color;
                if (isPoster)
                    color = poster_tex[(texY % POSTER_H) * POSTER_W + (texX % POSTER_W)];
                else
                    color = texture[texN][texY * TEX_W + texX];
                if (side == 1) color = ((color & 0xFEFEFE) >> 1);
                putPixel(fb, x, y, color);
            }

            /* floor and ceiling shading */
            for (int y = drawEnd + 1; y < SCR_H; y++) {
                u32 c = 40; putPixel(fb, x, y, COLOR_RGB(c, c, c));
            }
            for (int y = 0; y < drawStart; y++) {
                u32 c = 18; putPixel(fb, x, y, COLOR_RGB(c, c, c));
            }
        }

        /* crosshair */
        int cx = SCR_W/2 - 1, cy = SCR_H/2 - 1;
        for (int i = -3; i <= 3; i++) { putPixel(fb, cx+i, cy, COLOR_RGB(255,255,255)); putPixel(fb, cx, cy+i, COLOR_RGB(255,255,255)); }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForEvent(GSPGPU_EVENT_VBlank0);

        if (down & KEY_START) break;
    }

    gfxExit();
    return 0;
}
