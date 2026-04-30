#include <windows.h>

void FixLocale(){
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
}