#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <locale.h>
#endif

void FixLocale() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#else
    // On Linux/Ubuntu, this configures the C runtime to use the system's locale (usually UTF-8)
    setlocale(LC_ALL, "");
#endif
}