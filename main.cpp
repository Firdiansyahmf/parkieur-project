#include <iostream>
using namespace std;

// Supaya kodenya auto clear (biar rapih)
#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#else 
    #define CLEAR_COMMAND "clear"
#endif
void clearScreen() {
    system(CLEAR_COMMAND);
};

int main()
{
    return 0;
}