// AssemblyApp.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>

int main()
{
    int a = 5, b = 7, resultado;
    __asm {
        mov eax, a
        add eax, b
        mov resultado, eax
    }
    std::cout << resultado << std::endl;
    return 0;
}
