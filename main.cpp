#include <iostream>
using namespace std;

// fungsi clearScreen
#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#else 
    #define CLEAR_COMMAND "clear"
#endif
void clearScreen() {
    system(CLEAR_COMMAND);
};

// menuUtama
void menuUtama() {
    string pilihan;
    do {
        cout << "\n--- SISTEM PARKIR BASEMENT MALL ---\n";
        cout << "1. Kendaraan Masuk\n";
        cout << "2. Kendaraan Keluar\n";
        cout << "3. Tampilkan Parkir\n";
        cout << "4. Pengaturan\n";
        cout << "0. Keluar\n";
        cout << "Pilih: ";
        cin >> pilihan;

        if (pilihan == "1") {
            clearScreen();
            // tambahKendaraan();
        } else if (pilihan == "2") {
            clearScreen();
        } else if (pilihan == "3") {
            clearScreen();
            // tampilkanParkir();
        } else if (pilihan == "4") {
            clearScreen();
            // pengaturan();
        } else if (pilihan == "0") {
            clearScreen();
            cout << "Keluar dari program.\n";
        } else {
            clearScreen();
            cout << "Pilihan tidak valid. Silakan pilih antara 0 - 4.\n";
        }

    } while (pilihan != "0");
};

int main()
{
    menuUtama();
    return 0;
}