#include "gpu.h"
#include "raylib.h"
#include <chrono>
#include "roms.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "cpu.h"

static int m_screenWidth;
static int m_screenHeight;

#define UI_AREA_WIDTH 240

int scroll_idx = 0;
int active_item = 0;
int selected_rom = -1;

#define CHIP8_WHITE      CLITERAL(Color){ 153, 103, 0, 255 }   // White
#define CHIP8_BLACK      CLITERAL(Color){ 255, 204, 1, 255 }         // Black

std::string StringJoin(const std::vector<std::string> &strs, const std::string delim)
{
    if (strs.size() == 0) return "";
    std::vector<char> res;
    for (int i = 0; i < strs.size()-1; ++i)
    {
        for (auto c: strs[i]) res.push_back(c);
        for (auto c: delim) res.push_back(c);
    }
    for (auto c: strs[strs.size()-1]) res.push_back(c);
    return std::string{res.begin(), res.end()};
}


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
        if(m_hw->framebuffer[i] == 0 && m_hw->framebuffer[i] != m_hw->framebuffer_rendered[i])
        {
            // fading effect, draw the rendered pixel, give it a time to fade out
            DrawRectangle(x, y, m_hw->render_scaler, m_hw->render_scaler, m_hw->framebuffer_rendered[i] == 1 ? CHIP8_WHITE : CHIP8_BLACK);
        }else
        {
            DrawRectangle(x, y, m_hw->render_scaler, m_hw->render_scaler, m_hw->framebuffer[i] == 1 ? CHIP8_WHITE : CHIP8_BLACK);
        }

     

        m_hw->framebuffer_rendered[i] = m_hw->framebuffer[i];

        x += m_hw->render_scaler;
        if (x >= m_screenWidth)
        {
            x = 0;
            y += m_hw->render_scaler;
        }
    }

    // Draw UI
    GuiLabel(Rectangle{(float)m_screenWidth + 100, 0, UI_AREA_WIDTH - 100, 20}, "ROMs");
    GuiListViewEx(Rectangle{(float)m_screenWidth, 20, UI_AREA_WIDTH, (float)m_screenHeight - 20}, filenames, NUM_FILES, &scroll_idx, &active_item, NULL);
   
    if(active_item != selected_rom)
    {
        selected_rom = active_item;
        if(selected_rom >= 0 && selected_rom < NUM_FILES)
        {
            printf("Loading ROM: %s\n", filenames[selected_rom]);
            load_rom_from_bytes(file_data[selected_rom]);
        }
        
    }

    EndDrawing();
}

void gpu_close()
{
    CloseWindow();
}