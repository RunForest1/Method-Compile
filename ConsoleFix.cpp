#ifdef _WIN32
    #include <windows.h>
#endif
void FixLocale(){
    SetConsoleOutputCP(CP_UTF8);  // Enable UTF-8 output
    SetConsoleCP(CP_UTF8);         // Enable UTF-8 input (optional)
}