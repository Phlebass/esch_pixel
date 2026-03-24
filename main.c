#include "raylib.h"
#include <complex.h>
#include <math.h>
#include <stdio.h>

#define V2(a, b)                                                               \
    (Vector2) { a, b }
#define V2O (Vector2){0, 0}

int main()
{
    //
    // All code here is dedicated to basic loading and rendering
    // the actual 'Escher magic' all happens is shaders/shader_escher.fs
    //

    FILE *fb = fopen("options", "r");
    if (!fb) {
        printf("Error: options file not found\n");
        return 1;
    }

    int winsize;
    int scale;    // the scale step in two concentric pictures
    int num_pics; // max number is 8 (change the shader)

    // pics should be "pics/filename/filaname1.png" etc.
    char filename[21];

    if (!fscanf(fb, "winsize = %d\n", &winsize) ||
        !fscanf(fb, "scale = %d\n", &scale) ||
        !fscanf(fb, "num_pics = %d\n", &num_pics) ||
        !fscanf(fb, "filename = %20s\n", filename)) {
        printf("Failure reading the options file!\n");
        return 1;
    }

    InitWindow(winsize, winsize, "Hello!");

    Texture pics[num_pics];
    for (int i = 0; i < num_pics; i++) {
        pics[i] = LoadTexture(
            TextFormat("pics/%s/%s%d.png", filename, filename, i + 1));
    }

    const int imgsize          = pics[0].width;
    const int period           = pow(scale, num_pics);
    float zoom                 = 1; // multiplicative
    const int big_texture_size = imgsize * pow(scale, num_pics - 1);
    const float render_scale   = (float)winsize / big_texture_size;

    // this is the "magic" tilting constant
    // note: you get other reasonable stuff using
    // gamma = 1 + n*log(period)/(2ipi)
    // n can be negative and the "spiral" twists in the opposite direction
    const complex float gamma = 1 + log(period) / (2 * PI * I);
    Vector2 gamma_vec         = (Vector2){crealf(gamma), cimagf(gamma)};

    // obtain one picture drawing the different pics inside each other
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

    int shader_index = SH_ESCHER;

#define SET_SHADER_INT(i, sh_name, var)                                        \
    SetShaderValue(shaders[i], GetShaderLocation(shaders[i], sh_name), &var,   \
                   SHADER_UNIFORM_INT)

    // set the constant values in the shader
    SET_SHADER_INT(SH_ZOOM, "period", period);
    SET_SHADER_INT(SH_ESCHER, "period", period);

    // location of zoom and gamma uniform variables: the only variable one
    int zoom_shader_loc  = GetShaderLocation(shaders[shader_index], "zoom");
    int gamma_shader_loc = GetShaderLocation(shaders[SH_ESCHER], "gamma");

    SetTargetFPS(30);
    while (!WindowShouldClose()) {

        // mode change
        if (IsKeyPressed(KEY_C)) {
            shader_index    = (shader_index + 1) % 2;
            zoom_shader_loc = GetShaderLocation(shaders[shader_index], "zoom");
        }

        // zoom change
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

        // change in the 'gamma' constant
        if (IsKeyDown(KEY_W)) {
            gamma_vec.y += 1.0 / 32;
        } else if (IsKeyDown(KEY_S)) {
            gamma_vec.y -= 1.0 / 32;
        } else if (IsKeyDown(KEY_A)) {
            gamma_vec.x -= 1.0 / 32;
        } else if (IsKeyDown(KEY_D)) {
            gamma_vec.x += 1.0 / 32;
        }
        SetShaderValue(shaders[1], gamma_shader_loc, &gamma_vec,
                       SHADER_UNIFORM_VEC2);

        // render
        BeginDrawing();
        ClearBackground(GRAY);
        BeginShaderMode(shaders[shader_index]);
        DrawTextureEx(canvas.texture, V2O, 0, render_scale, WHITE);
        EndShaderMode();
        EndDrawing();
    }

    UnloadShader(shaders[0]);
    UnloadShader(shaders[1]);
    UnloadRenderTexture(canvas);
    CloseWindow();

    return 0;
}