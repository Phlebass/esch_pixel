#include "raylib.h"
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define V2(a, b)                                                               \
    (Vector2) { a, b }
#define V2O (Vector2){0, 0}

int main()
{

    // CHANGE PARAMETERS HERE

    const int winsize  = 800;
    const int scale    = 4; // the scale step in two concentric pictures
    const int num_pics = 2; // max number is 8 (change the shader)

    // pics should be "pics/filaname1.png" etc.
    const char *filename = "img";

    // CHANGE PARAMETERS ABOVE

    srand(time(NULL));
    InitWindow(winsize, winsize, "Hello!");

    Texture pics[num_pics];
    for (int i = 0; i < num_pics; i++) {
        pics[i] = LoadTexture(TextFormat("pics/%s%d.png", filename, i + 1));
    }

    const int imgsize = pics[0].width;

    const int period           = pow(scale, num_pics);
    float zoom                 = 1; // multiplicative
    const int big_texture_size = imgsize * pow(scale, num_pics - 1);
    const float render_scale   = (float)winsize / big_texture_size;

    // this is the "magic" tilting constant
    // note: you get other reasonable stuff using
    // gamma = 1 + n*log(period)/(2ipi)
    // n can be negative and for n=-1 the "spiral" twists in the opposite
    // direction
    const complex float gamma = 1 - log(period) / (2 * PI * I);
    const Vector2 gamma_vec   = (Vector2){crealf(gamma), cimagf(gamma)};

    // draw the pics on a canvas texture, one inside the other
    // this will remain constant, and used as a reference by the shader
    RenderTexture2D canvas =
        LoadRenderTexture(big_texture_size, big_texture_size);
    BeginTextureMode(canvas);
    int offset   = 0;
    int tmpscale = pow(scale, num_pics - 1);
    for (int i = 0; i < num_pics; i++) {
        DrawTextureEx(pics[i], V2(offset, offset), 0, tmpscale, WHITE);
        tmpscale /= scale;
        offset += imgsize * tmpscale * (scale - 1) / 2;
    }
    EndTextureMode();

#define SH_ZOOM 0
#define SH_ESCHER 1
    Shader shaders[2];
    shaders[SH_ZOOM]   = LoadShader(0, "shaders/shader_zoom.fs");
    shaders[SH_ESCHER] = LoadShader(0, "shaders/shader_escher.fs");

    int shader_index = SH_ZOOM;

#define SET_SHADER_INT(i, sh_name, var)                                        \
    SetShaderValue(shaders[i], GetShaderLocation(shaders[i], sh_name), &var,   \
                   SHADER_UNIFORM_INT)

    // set the constant values in the shader
    SET_SHADER_INT(SH_ZOOM, "period", period);
    SET_SHADER_INT(SH_ZOOM, "num_pics", num_pics);
    SET_SHADER_INT(SH_ZOOM, "scale_step", scale);

    SET_SHADER_INT(SH_ESCHER, "period", period);
    SetShaderValue(shaders[1], GetShaderLocation(shaders[1], "gamma"),
                   &gamma_vec, SHADER_UNIFORM_VEC2);

    // insert the images as a sheder uniform value
    // int pic_loc;
    // for (int i = 0; i < num_pics; i++) {
    //     pic_loc =
    //         GetShaderLocation(shaders[SH_ZOOM], TextFormat("textures[%d]",
    //         i));
    //     SetShaderValue(shaders[SH_ZOOM], pic_loc, &pics[i],
    //                    SHADER_UNIFORM_SAMPLER2D);
    // }

    // location of zoom uniform variable: the only variable one
    int zoom_shader_loc = GetShaderLocation(shaders[shader_index], "zoom");
    SetTargetFPS(30);
    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_C)) {
            shader_index    = (shader_index + 1) % 2;
            zoom_shader_loc = GetShaderLocation(shaders[shader_index], "zoom");
        }

        if (IsKeyDown(KEY_UP)) {
            zoom /= pow(scale, 1.0 / 32);
        } else if (IsKeyDown(KEY_DOWN)) {
            zoom *= pow(scale, 1.0 / 32);
        }
        if (zoom >= period) {
            zoom = 1;
        } else if (zoom < 1) {
            zoom = period;
        }
        SetShaderValue(shaders[shader_index], zoom_shader_loc, &zoom,
                       SHADER_UNIFORM_FLOAT);

        BeginDrawing();
        ClearBackground(GRAY);
        BeginShaderMode(shaders[shader_index]);
        DrawTextureEx(canvas.texture, V2O, 0, render_scale, WHITE);
        // DrawRectangle(0, 0, winsize, winsize, BLACK);
        EndShaderMode();
        EndDrawing();
    }

    UnloadShader(shaders[0]);
    UnloadShader(shaders[1]);
    UnloadRenderTexture(canvas);
    CloseWindow();

    return 0;
}