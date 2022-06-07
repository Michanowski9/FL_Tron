#include "WindowsKeyboard.h"

const bool WindowsKeyboard::GetKeyState(int key)
{
    return GetAsyncKeyState(key);
}