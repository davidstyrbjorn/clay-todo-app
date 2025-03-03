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

CardList card_list;

Clay_ElementDeclaration
card_item_config(Card *card, bool hovered)
{
    char card_id[16];
    snprintf(card_id, sizeof(card_id), "card%s", card->text);
    Clay_String string = {
        .chars = card_id,
        .length = strlen(card_id)};
    return (Clay_ElementDeclaration){
        .id = Clay__HashString(string, 0, 0),
        .layout = {
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(80)},
            .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER},
        },
        .backgroundColor = CARD_COLOR_FINAL,
        .cornerRadius = 16,
    };
}

Clay_ElementDeclaration column_header_config = (Clay_ElementDeclaration){
    .layout = {
        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(80)},
        .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER},
    },
};

Clay_TextElementConfig card_text_config = (Clay_TextElementConfig){
    .textColor = {255, 255, 255, 255},
    .fontId = FONT_ID_BODY_24,
    .fontSize = 48,
    .textAlignment = CLAY_TEXT_ALIGN_CENTER,
};

void card_element(int card_index)
{
    Card *card = card_list_get(&card_list, card_index);
    Clay_String string = {
        .chars = card->text,
        .length = strlen(card->text)};
    CLAY(card_item_config(card, Clay_Hovered()))
    {
        CLAY_TEXT(string, CLAY_TEXT_CONFIG(card_text_config));
    }
}

void create_card_list(CardState state)
{
    for (int i = 0; i < card_list.count; i++)
    {
        Card *card = card_list_get(&card_list, i);
        if (card->state == state)
        {
            card_element(i);
        }
    }
}

Clay_RenderCommandArray create_layout(void)
{
    Clay_BeginLayout();
    CLAY({.id = CLAY_ID("OuterContainer"), .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}}, .backgroundColor = BACKGROUND_COLOR})
    {
        CLAY({.id = CLAY_ID("Header"), .layout = {.childAlignment = {.y = CLAY_ALIGN_Y_CENTER, .x = CLAY_ALIGN_X_CENTER}, .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(80)}, .padding = {16, 16, 16, 16}, .childGap = 16}, .backgroundColor = BACKGROUND_COLOR})
        {
            CLAY_TEXT(CLAY_STRING("TODO APP"), CLAY_TEXT_CONFIG({.fontId = FONT_ID_TITLE, .fontSize = 48, .textColor = {0, 0, 0, 255}, .textAlignment = CLAY_TEXT_ALIGN_CENTER}));
        }

        CLAY({.id = CLAY_ID("ColumnContainer"), .layout = {.layoutDirection = CLAY_LEFT_TO_RIGHT, .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}, .padding = {16, 16, 16, 16}, .childGap = 16}, .backgroundColor = BACKGROUND_COLOR})
        {
            CLAY({.id = CLAY_ID("LeftColumn"), .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}, .padding = {16, 16, 16, 16}, .childGap = 16}, .backgroundColor = {150, 150, 255, 255}})
            {
                CLAY(column_header_config)
                {
                    CLAY_TEXT(CLAY_STRING("TODO"), CLAY_TEXT_CONFIG(card_text_config));
                }
                create_card_list(CARD_TODO);
            }
            CLAY({.id = CLAY_ID("RightColumn"), .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}, .padding = {16, 16, 16, 16}, .childGap = 16}, .backgroundColor = {150, 150, 255, 255}})
            {
                CLAY(column_header_config)
                {
                    CLAY_TEXT(CLAY_STRING("DONE"), CLAY_TEXT_CONFIG(card_text_config));
                }
                create_card_list(CARD_DONE);
            }
        }
    }
    return Clay_EndLayout();
}

bool debugEnabled = false;

void button_callbacks(void)
{
    if (IsMouseButtonPressed(0))
    {
        char card_id[16];
        for (int i = 0; i < card_list.count; i++)
        {
            Card *card = card_list_get(&card_list, i);
            snprintf(card_id, sizeof(card_id), "card%s", card->text);
            Clay_String string = {
                .chars = card_id,
                .length = strlen(card_id)};
            if (Clay_PointerOver(Clay__HashString(string, 0, 0)))
            {
                if (card->state == CARD_TODO)
                {
                    card_list_move(&card_list, i, CARD_DONE);
                }
                else
                {
                    card_list_remove(&card_list, i);
                }
            }
        }
    }
}

void update_draw_frame()
{
    if (IsKeyPressed(KEY_D))
    {
        debugEnabled = !debugEnabled;
        Clay_SetDebugModeEnabled(debugEnabled);
    }

    // Update our application state
    button_callbacks();
    card_list_update(&card_list, GetFrameTime());

    // 1. Retrieve peripheral data from Raylib
    Vector2 mouseWheelDelta = GetMouseWheelMoveV();
    float mouseWheelX = mouseWheelDelta.x;
    float mouseWheelY = mouseWheelDelta.y;
    Vector2 mousePosition = GetMousePosition();

    // 2. Send it to Clay
    Clay_SetPointerState((Clay_Vector2){mousePosition.x, mousePosition.y}, IsMouseButtonDown(0));
    Clay_SetLayoutDimensions((Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});
    Clay_UpdateScrollContainers(true, (Clay_Vector2){mouseWheelX, mouseWheelY}, GetFrameTime());

    // 1. Create a layout, note nothing is being rendered here! It's all just data right now
    Clay_RenderCommandArray renderCommands = create_layout();

    // 2. Send the layout data to a renderer, in this case our Raylib renderer
    BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(renderCommands, fonts);
    EndDrawing();
}

void HandleClayErrors(Clay_ErrorData errorData)
{
    printf("Clay: %s\n", errorData.errorText.chars);
}

void load_fonts()
{
    fonts[FONT_ID_BODY_24] = LoadFontEx("resources/Jost.ttf", 48, 0, 400);
    if (fonts[FONT_ID_BODY_24].texture.id == 0)
    {
        printf("Failed to load font Jost.ttf!\n");
        exit(0);
    }
    SetTextureFilter(fonts[FONT_ID_BODY_24].texture, TEXTURE_FILTER_BILINEAR);

    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Jost.ttf", 32, 0, 400);
    if (fonts[FONT_ID_BODY_24].texture.id == 0)
    {
        printf("Failed to load font Jost.ttf!\n");
        exit(0);
    }
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);

    fonts[FONT_ID_TITLE] = LoadFontEx("resources/Jost.ttf", 96, 0, 400);
    if (fonts[FONT_ID_TITLE].texture.id == 0)
    {
        printf("Failed to load font Jost.ttf!\n");
        exit(0);
    }
    SetTextureFilter(fonts[FONT_ID_TITLE].texture, TEXTURE_FILTER_BILINEAR);

    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);
}

int main(void)
{
    // Initalise clay and raylib
    uint64_t clayMemorySize = Clay_MinMemorySize();
    Clay_Arena clayArena = Clay_CreateArenaWithCapacityAndMemory(clayMemorySize, malloc(clayMemorySize));
    Clay_Initialize(clayArena, (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()}, (Clay_ErrorHandler){HandleClayErrors, NULL});
    Clay_Raylib_Initialize(680, 850, "TODO APP", FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);

    load_fonts();
    card_list_init(&card_list);

// Put in some examples cards
#if 1
    card_list_add(&card_list, "Koda", CARD_TODO);
    card_list_add(&card_list, "Sova", CARD_TODO);
    card_list_add(&card_list, "Springa", CARD_TODO);
    card_list_add(&card_list, "Jobba", CARD_DONE);
    card_list_add(&card_list, "Kolla", CARD_DONE);
    for (int i = 0; i < card_list.count; i++)
    {
        card_list_get(&card_list, i)->alive_time = ALIVE_TIME_CAP;
    }
#endif

    while (!WindowShouldClose())
    {
        update_draw_frame();
    }

    card_list_free(&card_list);

    return 0;
}
