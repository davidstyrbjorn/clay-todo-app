#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib/clay_renderer_raylib.c"
#include "card.h"

#define COLOR_ORANGE (Clay_Color){225, 138, 50, 255}
#define COLOR_BLUE (Clay_Color){111, 173, 162, 255}
#define CARD_COLOR (Clay_Color){59, 92, 128, 255}
#define CARD_COLOR_FINAL (Clay_Color){0, 38, 86, 255}
#define BACKGROUND_COLOR (Clay_Color){230, 236, 241, 255}

const uint32_t FONT_ID_BODY_16 = 0;
const uint32_t FONT_ID_BODY_24 = 0;
const uint32_t FONT_ID_TITLE = 0;

Font fonts[2];

const Clay_Sizing FULL_SIZE = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};

Clay_RenderCommandArray create_layout(void)
{
    Clay_BeginLayout();
    // CLAY({.id = CLAY_ID("OuterContainer"), .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = FULL_SIZE}, .backgroundColor = BACKGROUND_COLOR})
    // {
    // //     CLAY({.id = CLAY_ID("Header"), .layout = {.childAlignment = {.y = CLAY_ALIGN_Y_CENTER, .x = CLAY_ALIGN_X_CENTER}, .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(80)}, .padding = {16, 16, 16, 16}, .childGap = 16}, .backgroundColor = CARD_COLOR_FINAL})
    // //     {
    // // //         CLAY_TEXT(CLAY_STRING("TODO APP"), CLAY_TEXT_CONFIG({.fontId = FONT_ID_TITLE, .fontSize = 48, .textColor = {255, 255, 255, 255}, .textAlignment = CLAY_TEXT_ALIGN_CENTER}));
    // //     }

    // //     CLAY({.id = CLAY_ID("ColumnContainer"), .layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT, .sizing = FULL_SIZE, .padding = {16, 16, 16, 16}, .childGap = 16}, .backgroundColor = COLOR_BLUE})
    // //     {
    // //         CLAY({.id = CLAY_ID("LeftColumn"), .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = FULL_SIZE, .padding = {16, 16, 16, 16}, .childGap = 16}, .backgroundColor = {150, 150, 255, 255}})
    // //         {
    // //         }
    // //         CLAY({.id = CLAY_ID("RightColumn"), .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = FULL_SIZE, .padding = {16, 16, 16, 16}, .childGap = 16}, .backgroundColor = {150, 150, 255, 255}})
    // //         {
    // //         }
    // //     }
    // }
    return Clay_EndLayout();
}

void update_draw_frame()
{
    // Retrieve some keyboard & mouse data from Raylib
    Vector2 mouseWheelDelta = GetMouseWheelMoveV();
    float mouseWheelX = mouseWheelDelta.x;
    float mouseWheelY = mouseWheelDelta.y;
    Vector2 mousePosition = GetMousePosition();

    // Update Clay internal state
    Clay_SetPointerState((Clay_Vector2){mousePosition.x, mousePosition.y}, IsMouseButtonDown(0));
    Clay_SetLayoutDimensions((Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});
    Clay_UpdateScrollContainers(true, (Clay_Vector2){mouseWheelX, mouseWheelY}, GetFrameTime());

    // Retrieve render command list
    Clay_RenderCommandArray renderCommands = create_layout();

    // Actually render the render command list
    BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(renderCommands, fonts);
    EndDrawing();
}

bool reinitializeClay = false;

void HandleClayErrors(Clay_ErrorData errorData)
{
    printf("Clay: %s", errorData.errorText.chars);
}

int main(void)
{
    // Initalise clay and raylib
    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
    Clay_Initialize(clayMemory, (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors, 0});
    Clay_Raylib_Initialize(680, 850, "TODO APP", FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

    // Initalise fonts
    fonts[FONT_ID_BODY_24] = LoadFontEx("resources/Jost.ttf", 48, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_24].texture, TEXTURE_FILTER_BILINEAR);
    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Jost.ttf", 32, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);
    fonts[FONT_ID_TITLE] = LoadFontEx("resources/Jost.ttf", 96, 0, 400);
    SetTextureFilter(fonts[FONT_ID_TITLE].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    while (!WindowShouldClose())
    {
        update_draw_frame();
    }

    return 0;
}
