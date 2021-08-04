#pragma once

class c_minhook_rehook
{
    uintptr_t original_addr, *jmp2_va {nullptr};
public:
    c_minhook_rehook() = default;
    c_minhook_rehook(uintptr_t address, uintptr_t new_func) {
        install(address, new_func);
    }

    // We don't need to call demolish in destructor
    // coz jmp2_va was removed when class was removed

    uintptr_t get_jmp_va(uintptr_t jmp_addr)
    {
        uintptr_t dest_rva {*(uint32_t *)(jmp_addr + 1u)};
        return jmp_addr + dest_rva + 4u - 0xFFFFFFFFu;
    }

    void install(uintptr_t address, uintptr_t new_func)
    {
        uintptr_t jmp_va {get_jmp_va(address)};

        jmp2_va = (uintptr_t *)(jmp_va + 6u);
        original_addr = *jmp2_va;
        *jmp2_va = new_func;
    }

    uintptr_t get_original() {
        return original_addr;
    }

    void demolish() {
        *jmp2_va = original_addr;
    }
};
