
#include <iostream>
#include <minhook/MinHook.h>

#include "minhook_rehook.hpp"

using msgbox_t = int(WINAPI *)(HWND, LPCSTR, LPCSTR, UINT);

msgbox_t orig_msgbox;

// Чей-то hook
int __cdecl hooked_msgbox(HWND hwnd, LPCSTR text, LPCSTR caption, UINT type)
{
    printf("hooked_msgbox: %s\n", text);

    return 0; // Подавляю вызов оригинала, это окно меня раздражает
    //return orig_msgbox(hwnd, text, caption, type);
}

// Наш rehook
int __cdecl rehooked_msgbox(HWND hwnd, LPCSTR text, LPCSTR caption, UINT type)
{
    printf("rehooked_msgbox: %s\n", text);

    return reinterpret_cast<msgbox_t>(msgbox_rehook.get_original())(hwnd, text, caption, type);
}

uint32_t get_jmp_rva(uintptr_t src, uintptr_t dst)
{
    return (src - dst - 5);
}

int main()
{
    MH_Initialize();

    // Хукаю MessageBox
    MH_CreateHook(&MessageBoxA, &hooked_msgbox, (LPVOID *)&orig_msgbox);
    MH_EnableHook(&MessageBoxA);
    
    // Вывожу адрес чтобы чекать в CE
    printf("shit: %p\n", MessageBoxA);

    // Устанавливаю rehook на hook
    msgbox_rehook.install((uintptr_t)MessageBoxA, (uintptr_t)rehooked_msgbox);
  
    // И вызываю саму функцию
    MessageBoxA(0, "hello world", "test", MB_OK);
  
    // Снимаю rehook с hook'а
    msgbox_rehook.demolish();
  
    std::system("pause");
    
    MH_Uninitialize();
}
