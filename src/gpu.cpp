#include "gpu.h"
#include "raylib.h"
#include <chrono>



#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

static int m_screenWidth;
static int m_screenHeight;

#define UI_AREA_WIDTH 240

bool show_message_box = false;

void gpu_init()
{
    m_screenWidth = m_hw->display_w * m_hw->render_scaler;
    m_screenHeight = m_hw->display_h * m_hw->render_scaler;
    
    InitWindow(m_screenWidth + UI_AREA_WIDTH, m_screenHeight, "Chip-8 Emulator");
}

void gpu_render()
{
    
    BeginDrawing();
    ClearBackground(WHITE);

    // Draw framebuffer
    int x = 0, y = 0;

    const int len = m_hw->display_w * m_hw->display_h;
    for (int i = 0; i < len; ++i)
    {

        DrawRectangle(x, y, m_hw->render_scaler, m_hw->render_scaler, m_hw->framebuffer[i] == 1 ? WHITE : BLACK);

        x += m_hw->render_scaler;
        if (x >= m_screenWidth)
        {
            x = 0;
            y += m_hw->render_scaler;
        }
    }

    // Draw UI

    if (GuiButton(Rectangle{ (float)m_screenWidth + 24, 24, 120, 30 }, "#191#Show Message"))
        {
            show_message_box = true;
        }

        if(show_message_box)
        {
         int r = GuiMessageBox(Rectangle{ GetScreenWidth()/2.0f - 125, GetScreenHeight()/2.0f - 50, 250, 100 }, "This is a message box", "Message Box",  "Nice;Cool");
            if (r >= 0)
            {
                show_message_box = false;
            }
    }


    EndDrawing();
}

void gpu_close()
{
    CloseWindow();
}