#include "input.h"
#include "raylib.h"
#include "hw.h"

static int key_map[16]{
        KEY_X,
        KEY_ONE,
        KEY_TWO,
        KEY_THREE,
        KEY_Q,
        KEY_W,
        KEY_E,
        KEY_A,
        KEY_S,
        KEY_D,
        KEY_Z,
        KEY_C,
        KEY_FOUR,
        KEY_R,
        KEY_F,
        KEY_V
};

int get_key_press_map(int key)
{
    if (key == 0)
        return -1;

    for (int i = 0; i < 16; ++i)
    {
        if (key == key_map[i])
            return i;
    }

    return -1;
}

void input_process()
{
        int key, cpuKey;
        
        // key press        
        cpuKey = get_key_press_map(GetKeyPressed());
        
        if (cpuKey > -1)
        {
            m_hw->key_press[cpuKey] = 1;
        }

        // key release
        for (int i = 0; i < 16; ++i)
        {
            if (m_hw->key_press[i] > 0 && IsKeyReleased(key_map[i]))
            {
                m_hw->key_press[i] = 0;
            }
        }
}