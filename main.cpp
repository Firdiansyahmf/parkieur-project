#include <iostream>
#include <string>
#include <limits>    // numeric_limits
#include <iomanip>    // setw
#include <cctype>     // tolower, toupper, isalpha, isdigit, isalnum
#include <sstream>
#include <fstream>    // stringstream for getValidIntegerInput
#include <functional>


using namespace std;

#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#else
    #define CLEAR_COMMAND "clear"
#endif

void clearScreen() {
    system(CLEAR_COMMAND);
}

// --- KONFIGURASI SISTEM PARKIR ---
int MAX_FLOORS = 5;
int MAX_CAR_SPOTS_PER_FLOOR = 10;
int MAX_MOTOR_SPOTS_PER_FLOOR = 15;

struct Vehicle {
    string licensePlate;
    string entryTime;
    string type;
    int floor;
    int spot;

    Vehicle() : licensePlate(""), type(""), floor(0), spot(0), entryTime("") {}
    Vehicle(string lp, string t, int f, int s)
        : licensePlate(lp), type(t), floor(f), spot(s), entryTime("") {}
};

Vehicle*** parkingSpots = nullptr;

// --- IMPLEMENTASI QUEUE ---
const int MAX_QUEUE_CAPACITY = 100;
int CURRENT_QUEUE_SIZE = 5;

struct VehicleQueue {
    int front;
    int rear;
    int count;
    Vehicle vehicles[MAX_QUEUE_CAPACITY];

    VehicleQueue() : front(0), rear(-1), count(0) {}
};

VehicleQueue entryQueue;

bool isQueueEmpty(VehicleQueue& q) {
    return q.count == 0;
}

bool isQueueFull(VehicleQueue& q) {
    return q.count == CURRENT_QUEUE_SIZE;
}

void enqueue(VehicleQueue& q, const Vehicle& v) {
    if (isQueueFull(q)) {
        cout << "Maaf, antrean masuk sudah penuh. Kendaraan tidak bisa ditambahkan ke antrean.\n";
    } else {
        q.rear = (q.rear + 1) % CURRENT_QUEUE_SIZE;
        q.vehicles[q.rear] = v;
        q.count++;
        cout << "Kendaraan " << v.licensePlate << " (" << v.type << ") telah masuk antrean.\n";
    }
}

Vehicle dequeue(VehicleQueue& q) {
    if (isQueueEmpty(q)) {
        return Vehicle();
    } else {
        Vehicle dequeuedVehicle = q.vehicles[q.front];
        q.front = (q.front + 1) % CURRENT_QUEUE_SIZE;
        q.count--;
        cout << "Kendaraan " << dequeuedVehicle.licensePlate << " telah keluar dari antrean.\n";
        return dequeuedVehicle;
    }
}

void displayEntryQueue() {
    if (isQueueEmpty(entryQueue)) {
        cout << "Antrean masuk kosong.\n";
    } else {
        cout << "\n--- ISI ANTRIAN MASUK (" << entryQueue.count << "/" << CURRENT_QUEUE_SIZE << ") ---\n";
        int current = entryQueue.front;
        for (int i = 0; i < entryQueue.count; ++i) {
            cout << (i + 1) << ". " << entryQueue.vehicles[current].licensePlate
                 << " (" << entryQueue.vehicles[current].type << ")\n";
            current = (current + 1) % CURRENT_QUEUE_SIZE;
        }
        cout << "------------------------------------------\n";
    }
}

// --- IMPLEMENTASI BINARY SEARCH TREE (BST) ---
struct VehicleNode {
    string licensePlate;
    string type;
    int floor;
    int spot;
    VehicleNode *left, *right;

    VehicleNode(string lp, string t, int f, int s)
        : licensePlate(lp), type(t), floor(f), spot(s), left(nullptr), right(nullptr) {}

    ~VehicleNode() {
        delete left;
        left = nullptr;
        delete right;
        right = nullptr;
    }
};

VehicleNode* parkedVehiclesTree = nullptr;

VehicleNode* insertNode(VehicleNode* node, string licensePlate, string type, int floor, int spot) {
    if (node == nullptr) {
        return new VehicleNode(licensePlate, type, floor, spot);
    }
    if (licensePlate < node->licensePlate) {
        node->left = insertNode(node->left, licensePlate, type, floor, spot);
    }
    else if (licensePlate > node->licensePlate) {
        node->right = insertNode(node->right, licensePlate, type, floor, spot);
    }
    return node;
}

VehicleNode* searchNode(VehicleNode* node, string licensePlate) {
    if (node == nullptr || node->licensePlate == licensePlate) {
        return node;
    }
    if (licensePlate < node->licensePlate) {
        return searchNode(node->left, licensePlate);
    }
    else {
        return searchNode(node->right, licensePlate);
    }
}

VehicleNode* minValueNode(VehicleNode* node) {
    VehicleNode* current = node;
    while (current && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

VehicleNode* deleteNode(VehicleNode* root, string licensePlate) {
    if (root == nullptr) {
        return root;
    }

    if (licensePlate < root->licensePlate) {
        root->left = deleteNode(root->left, licensePlate);
    } else if (licensePlate > root->licensePlate) {
        root->right = deleteNode(root->right, licensePlate);
    } else {
        if (root->left == nullptr) {
            VehicleNode* temp = root->right;
            root->right = nullptr;
            delete root;
            return temp;
        } else if (root->right == nullptr) {
            VehicleNode* temp = root->left;
            root->left = nullptr;
            delete root;
            return temp;
        }

        VehicleNode* temp = minValueNode(root->right);

        root->licensePlate = temp->licensePlate;
        root->type = temp->type;
        root->floor = temp->floor;
        root->spot = temp->spot;

        root->right = deleteNode(root->right, temp->licensePlate);
    }
    return root;
}

// Fungsi inorder traversal untuk menampilkan kendaraan dari BST
void inorderTraversalBST(VehicleNode* node) {
    if (node == nullptr) {
        return;
    }

    inorderTraversalBST(node->left); // Kiri
    cout << "  Plat: " << node->licensePlate
         << ", Tipe: " << node->type
         << ", Lantai: B" << node->floor
         << ", Spot: " << node->spot << "\n"; // Root
    inorderTraversalBST(node->right); // Kanan
}

void deallocateParkingSpots() {
    if (parkingSpots != nullptr) {
        for (int i = 0; i < MAX_FLOORS; ++i) {
            if (parkingSpots[i] != nullptr) {
                delete[] parkingSpots[i][0];
                delete[] parkingSpots[i][1];
                delete[] parkingSpots[i];
            }
        }
        delete[] parkingSpots;
        parkingSpots = nullptr;
    }
}

void initializeParking() {
    if (parkedVehiclesTree != nullptr) {
        delete parkedVehiclesTree;
        parkedVehiclesTree = nullptr;
    }

    deallocateParkingSpots();

    entryQueue = VehicleQueue();

    parkingSpots = new Vehicle**[MAX_FLOORS];
    for (int i = 0; i < MAX_FLOORS; ++i) {
        parkingSpots[i] = new Vehicle*[2];
        parkingSpots[i][0] = new Vehicle[MAX_CAR_SPOTS_PER_FLOOR];
        parkingSpots[i][1] = new Vehicle[MAX_MOTOR_SPOTS_PER_FLOOR];
    }

    for (int f = 0; f < MAX_FLOORS; ++f) {
        for (int s = 0; s < MAX_CAR_SPOTS_PER_FLOOR; ++s) {
            parkingSpots[f][0][s].licensePlate = "";
        }
        for (int s = 0; s < MAX_MOTOR_SPOTS_PER_FLOOR; ++s) {
            parkingSpots[f][1][s].licensePlate = "";
        }
    }

    cout << "Sistem parkir telah diinisialisasi ulang dengan pengaturan baru.\n";
    cout << "Semua kendaraan yang sebelumnya terparkir telah dihapus!\n";
}

int getValidIntegerInput(const string& prompt, int min, int max) {
    int value;
    string inputLine;
    while (true) {
        cout << prompt;
        getline(cin, inputLine); // Baca seluruh baris sebagai string

        stringstream ss(inputLine);
        if (ss >> value && ss.eof()) { // Coba konversi dan pastikan tidak ada karakter sisa
            if (value < min || value > max) {
                cout << "Input di luar rentang. Mohon masukkan angka antara " << min << " dan " << max << ".\n";
            } else {
                return value;
            }
        } else {
            cout << "Input tidak valid. Mohon masukkan angka bilangan bulat.\n";
        }
    }
}

string getLineInput(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

bool isValidIndonesianLicensePlate(const string& plate) {
    if (plate.empty()) {
        return false;
    }
    string upperPlate = plate;
    for (char &c : upperPlate) {
        c = toupper(c);
    }
    
    size_t firstSpace = upperPlate.find(' ');
    if (firstSpace == string::npos || firstSpace == 0) return false;

    string part1 = upperPlate.substr(0, firstSpace);
    if (part1.length() < 1 || part1.length() > 2) return false;
    for (char c : part1) {
        if (!isalpha(c)) return false;
    }

    size_t secondSpace = upperPlate.find(' ', firstSpace + 1);
    string part2;

    if (secondSpace != string::npos) {
        part2 = upperPlate.substr(firstSpace + 1, secondSpace - (firstSpace + 1));
    } else {
        part2 = upperPlate.substr(firstSpace + 1);
    }

    if (part2.length() < 1 || part2.length() > 4) return false;
    for (char c : part2) {
        if (!isdigit(c)) return false;
    }

    if (secondSpace != string::npos) {
        string part3 = upperPlate.substr(secondSpace + 1);
        if (part3.length() < 1 || part3.length() > 3) return false;
        for (char c : part3) {
            if (!isalpha(c)) return false;
        }
    }

    for (char c : upperPlate) {
        if (!isalnum(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

void placeVehicle(const Vehicle& v) {
    int floorIndex = v.floor - 1;
    int spotIndex = v.spot - 1;
    int typeIndex = (v.type == "mobil") ? 0 : 1;

    parkingSpots[floorIndex][typeIndex][spotIndex] = v;
    parkedVehiclesTree = insertNode(parkedVehiclesTree, v.licensePlate, v.type, v.floor, v.spot);
    cout << "Kendaraan " << v.licensePlate << " (" << v.type << ") berhasil parkir di Lantai B"
                 << v.floor << ", Spot " << v.spot << ".\n";
}

string entryTime;
void saveParkedVehiclesToCSV() {
    ofstream file("parkir_data.csv");
    file << "Plat,Tipe,Lantai,Spot,JamMasuk\n";
    function<void(VehicleNode*)> saveRecursive = [&](VehicleNode* node) {
        if (!node) return;
        saveRecursive(node->left);
        file << node->licensePlate << "," << node->type << "," << node->floor << "," << node->spot << "," 
             << parkingSpots[node->floor - 1][(node->type == "mobil" ? 0 : 1)][node->spot - 1].entryTime << "\n";
        saveRecursive(node->right);
    };
    saveRecursive(parkedVehiclesTree);
    file.close();
}

void loadParkedVehiclesFromCSV() {
    ifstream file("parkir_data.csv");
    if (!file.is_open()) return;
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string plat, tipe, lantaiStr, spotStr, jamMasuk;
        getline(ss, plat, ',');
        getline(ss, tipe, ',');
        getline(ss, lantaiStr, ',');
        getline(ss, spotStr, ',');
        getline(ss, jamMasuk, ',');
        int floor = stoi(lantaiStr);
        int spot = stoi(spotStr);
        Vehicle v(plat, tipe, floor, spot);
        v.entryTime = jamMasuk;
        placeVehicle(v);
    }
    file.close();
}

void vehicleEntry() {
    string vehicleType;
    string licensePlate;
    int typeIndex = -1;
    int maxSpots = 0;

    cout << "\n--- KENDARAAN MASUK ---\n";
    do {
        cout << "Jenis kendaraan (mobil/motor): ";
        getline(cin, vehicleType);
        for (char &c : vehicleType) {
            c = tolower(c);
        }

        if (vehicleType == "mobil") {
            typeIndex = 0;
            maxSpots = MAX_CAR_SPOTS_PER_FLOOR;
            break;
        } else if (vehicleType == "motor") {
            typeIndex = 1;
            maxSpots = MAX_MOTOR_SPOTS_PER_FLOOR;
            break;
        } else {
            cout << "Jenis kendaraan tidak valid. Mohon masukkan 'mobil' atau 'motor'.\n";
        }
    } while (true);

    do {
        licensePlate = getLineInput("Masukkan plat nomor kendaraan (contoh: B 1234 ABC atau B 1234): ");
        if (!isValidIndonesianLicensePlate(licensePlate)) {
            cout << "Format plat nomor tidak valid. Mohon ikuti format standar plat nomor Indonesia (misal: B 1234 ABC atau B 1234).\n";
        } else {
            for (char &c : licensePlate) {
                c = toupper(c);
            }
            break;
        }
    } while (true);

    if (searchNode(parkedVehiclesTree, licensePlate) != nullptr) {
        cout << "Plat nomor " << licensePlate << " sudah terdaftar di tempat parkir.\n";
        cout << "Tekan ENTER untuk melanjutkan...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    bool parked = false;
    for (int f = 0; f < MAX_FLOORS; ++f) {
        for (int s = 0; s < maxSpots; ++s) {
            if (parkingSpots[f][typeIndex][s].licensePlate == "") {
                
    
    do {
        entryTime = getLineInput("Masukkan jam masuk (format HH:MM): ");
        if (entryTime.length() != 5 || entryTime[2] != ':' ||
            !isdigit(entryTime[0]) || !isdigit(entryTime[1]) ||
            !isdigit(entryTime[3]) || !isdigit(entryTime[4])) {
            cout << "Format waktu tidak valid. Gunakan format HH:MM.\n";
        } else {
            break;
        }
    } while (true);
    Vehicle newVehicle(licensePlate, vehicleType, f + 1, s + 1);
    newVehicle.entryTime = entryTime;
                placeVehicle(newVehicle);
                saveParkedVehiclesToCSV();
                parked = true;
                break;
            }
        }
        if (parked) break;
    }

    if (!parked) {
        Vehicle newVehicle(licensePlate, vehicleType, 0, 0); // floor and spot 0 indicate in queue
        newVehicle.entryTime = entryTime;
        enqueue(entryQueue, newVehicle);
        cout << "Maaf, parkir " << vehicleType << " penuh di semua lantai. Kendaraan dimasukkan ke antrean.\n";
    }
    cout << "Tekan ENTER untuk melanjutkan...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void vehicleExit() {
    string licensePlateToExit;
    cout << "\n--- KENDARAAN KELUAR ---\n";
    do {
        cout << "Masukkan plat nomor kendaraan yang keluar (contoh: B 1234 ABC atau B 1234) atau 'batal' untuk kembali: ";
        getline(cin, licensePlateToExit);
        
        string lowerCaseInput = licensePlateToExit;
        for (char &c : lowerCaseInput) {
            c = tolower(c);
        }

        if (lowerCaseInput == "batal") {
            cout << "Proses keluar kendaraan dibatalkan.\n";
            cout << "Tekan ENTER untuk melanjutkan...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (!isValidIndonesianLicensePlate(licensePlateToExit)) {
            cout << "Format plat nomor tidak valid. Mohon ikuti format standar plat nomor Indonesia (misal: B 1234 ABC atau B 1234).\n";
        } else {
            for (char &c : licensePlateToExit) {
                c = toupper(c);
            }
            break;
        }
    } while (true);

    VehicleNode* vehicleNode = searchNode(parkedVehiclesTree, licensePlateToExit);

    if (vehicleNode != nullptr) {
        int floorIndex = vehicleNode->floor - 1;
        int spotIndex = vehicleNode->spot - 1;
        int typeIndex = (vehicleNode->type == "mobil") ? 0 : 1;

        
    string exitTime;
    do {
        exitTime = getLineInput("Masukkan jam keluar (format HH:MM): ");
        if (exitTime.length() != 5 || exitTime[2] != ':' ||
            !isdigit(exitTime[0]) || !isdigit(exitTime[1]) ||
            !isdigit(exitTime[3]) || !isdigit(exitTime[4])) {
            cout << "Format waktu tidak valid. Gunakan format HH:MM.\n";
        } else {
            break;
        }
    } while (true);

    auto parseTime = [](const string& timeStr) -> int {
        int hours = stoi(timeStr.substr(0, 2));
        int minutes = stoi(timeStr.substr(3, 2));
        return hours * 60 + minutes;
    };

    int entryMinutes = parseTime(parkingSpots[floorIndex][typeIndex][spotIndex].entryTime);
    int exitMinutes = parseTime(exitTime);
    int durationMinutes = exitMinutes - entryMinutes;
    if (durationMinutes <= 0) durationMinutes = 1;

    int hours = (durationMinutes + 59) / 60;
    int rate = (vehicleNode->type == "mobil") ? 4000 : 2000;
    int total = hours * rate;

    cout << "Durasi parkir: " << hours << " jam\n";
    cout << "Biaya parkir: Rp" << total << "\n";

        parkingSpots[floorIndex][typeIndex][spotIndex].licensePlate = "";
        parkingSpots[floorIndex][typeIndex][spotIndex].type = "";
        parkingSpots[floorIndex][typeIndex][spotIndex].floor = 0;
        parkingSpots[floorIndex][typeIndex][spotIndex].spot = 0;

        parkedVehiclesTree = deleteNode(parkedVehiclesTree, licensePlateToExit);
        saveParkedVehiclesToCSV();
        cout << "Kendaraan dengan plat nomor " << licensePlateToExit
                     << " telah keluar dari Lantai B" << (floorIndex + 1)
                     << ", Spot " << (spotIndex + 1) << ".\n";

        if (!isQueueEmpty(entryQueue)) {
            cout << "\nAda kendaraan dalam antrean. Mencoba menempatkan kendaraan berikutnya...\n";
            Vehicle nextVehicle = dequeue(entryQueue);
            
            int nextTypeIndex = (nextVehicle.type == "mobil") ? 0 : 1;
            int nextMaxSpots = (nextVehicle.type == "mobil") ? MAX_CAR_SPOTS_PER_FLOOR : MAX_MOTOR_SPOTS_PER_FLOOR;

            bool placedFromQueue = false;
            for (int f = 0; f < MAX_FLOORS; ++f) {
                for (int s = 0; s < nextMaxSpots; ++s) {
                    if (parkingSpots[f][nextTypeIndex][s].licensePlate == "") {
                        nextVehicle.floor = f + 1;
                        nextVehicle.spot = s + 1;
                        placeVehicle(nextVehicle);
                        placedFromQueue = true;
                        break;
                    }
                }
                if (placedFromQueue) break;
            }

            if (!placedFromQueue) {
                cout << "Spot kosong tidak ditemukan untuk kendaraan dari antrean " << nextVehicle.licensePlate << ". Kendaraan dikembalikan ke antrean.\n";
                enqueue(entryQueue, nextVehicle); // Re-enqueue if no spot found
            }
        }
    } else {
        cout << "Plat nomor " << licensePlateToExit << " tidak ditemukan di tempat parkir.\n";
    }
    cout << "Tekan ENTER untuk melanjutkan...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void displayParkingStatus() {
    cout << "\n--- STATUS PARKIR SAAT INI ---\n";

    // Tampilan berdasarkan lokasi fisik (tetap dipertahankan)
    cout << "\n--- STATUS PARKIR BERDASARKAN LOKASI --- \n";
    for (int f = 0; f < MAX_FLOORS; ++f) {
        cout << "\nLantai B" << (f + 1) << ":\n";
        
        cout << "    Mobil (" << MAX_CAR_SPOTS_PER_FLOOR << " slot):\n";
        bool carSpotOccupied = false;
        for (int s = 0; s < MAX_CAR_SPOTS_PER_FLOOR; ++s) {
            if (parkingSpots[f][0][s].licensePlate != "") {
                cout << "      Spot " << setw(2) << (s + 1) << ": " << parkingSpots[f][0][s].licensePlate << "\n";
                carSpotOccupied = true;
            }
        }
        if (!carSpotOccupied) {
            cout << "      Semua spot mobil kosong.\n";
        }

        cout << "    Motor (" << MAX_MOTOR_SPOTS_PER_FLOOR << " slot):\n";
        bool motorSpotOccupied = false;
        for (int s = 0; s < MAX_MOTOR_SPOTS_PER_FLOOR; ++s) {
            if (parkingSpots[f][1][s].licensePlate != "") {
                cout << "      Spot " << setw(2) << (s + 1) << ": " << parkingSpots[f][1][s].licensePlate << "\n";
                motorSpotOccupied = true;
            }
        }
        if (!motorSpotOccupied) {
            cout << "      Semua spot motor kosong.\n";
        }
    }

    // Tampilan berdasarkan inorder traversal dari BST
    cout << "\n--- KENDARAAN TERPARKIR (Berdasarkan Plat Nomor, urutan Alfabetis) ---\n";
    if (parkedVehiclesTree == nullptr) {
        cout << "Belum ada kendaraan terparkir.\n";
    } else {
        inorderTraversalBST(parkedVehiclesTree);
    }

    displayEntryQueue();
    cout << "\nTekan ENTER untuk melanjutkan...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void settingsMenu() {
    string settingsChoice;
    do {
        clearScreen();
        cout << "\n--- PENGATURAN SISTEM PARKIR ---\n";
        cout << "Kapasitas saat ini:\n";
        cout << "    Jumlah Lantai         : " << MAX_FLOORS << "\n";
        cout << "    Spot Mobil per Lantai : " << MAX_CAR_SPOTS_PER_FLOOR << "\n";
        cout << "    Spot Motor per Lantai : " << MAX_MOTOR_SPOTS_PER_FLOOR << "\n";
        cout << "    Ukuran Antrean Masuk  : " << CURRENT_QUEUE_SIZE << " (Max: " << MAX_QUEUE_CAPACITY << ")\n";
        cout << "\n1. Ubah Jumlah Lantai\n";
        cout << "2. Ubah Kapasitas Spot Mobil per Lantai\n";
        cout << "3. Ubah Kapasitas Spot Motor per Lantai\n";
        cout << "4. Ubah Ukuran Antrean Masuk\n";
        cout << "0. Kembali ke Menu Utama\n";
        cout << "Pilih: ";
        cin >> settingsChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        bool waitForEnter = true;

        if (settingsChoice == "1") {
            int newFloors = getValidIntegerInput("Masukkan jumlah lantai baru (min 1, max 5): ", 1, 5);
            if (newFloors != MAX_FLOORS) {
                if (parkedVehiclesTree != nullptr) {
                    cout << "PERINGATAN: Mengubah jumlah lantai akan menginisialisasi ulang sistem dan menghapus semua kendaraan terparkir!\n";
                    cout << "Apakah Anda yakin ingin melanjutkan? (y/n): ";
                    char confirm;
                    cin >> confirm;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (tolower(confirm) != 'y') {
                        cout << "Operasi dibatalkan.\n";
                        continue;
                    }
                }
                MAX_FLOORS = newFloors;
                cout << "Jumlah lantai diubah menjadi " << MAX_FLOORS << ".\n";
                initializeParking();
    loadParkedVehiclesFromCSV();
            } else {
                cout << "Jumlah lantai tidak berubah.\n";
            }
        } else if (settingsChoice == "2") {
            int newCarSpots = getValidIntegerInput("Masukkan kapasitas spot mobil per lantai baru (min 1, max 50): ", 1, 50);
            if (newCarSpots != MAX_CAR_SPOTS_PER_FLOOR) {
                    if (parkedVehiclesTree != nullptr) {
                        cout << "PERINGATAN: Mengubah kapasitas spot mobil akan menginisialisasi ulang sistem dan menghapus semua kendaraan terparkir!\n";
                        cout << "Apakah Anda yakin ingin melanjutkan? (y/n): ";
                        char confirm;
                        cin >> confirm;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (tolower(confirm) != 'y') {
                            cout << "Operasi dibatalkan.\n";
                            continue;
                        }
                    }
                MAX_CAR_SPOTS_PER_FLOOR = newCarSpots;
                cout << "Kapasitas spot mobil per lantai diubah menjadi " << MAX_CAR_SPOTS_PER_FLOOR << ".\n";
                initializeParking();
    loadParkedVehiclesFromCSV();
            } else {
                cout << "Kapasitas spot mobil per lantai tidak berubah.\n";
            }
        } else if (settingsChoice == "3") {
            int newMotorSpots = getValidIntegerInput("Masukkan kapasitas spot motor per lantai baru (min 1, max 50): ", 1, 50);
            if (newMotorSpots != MAX_MOTOR_SPOTS_PER_FLOOR) {
                    if (parkedVehiclesTree != nullptr) {
                        cout << "PERINGATAN: Mengubah kapasitas spot motor akan menginisialisasi ulang sistem dan menghapus semua kendaraan terparkir!\n";
                        cout << "Apakah Anda yakin ingin melanjutkan? (y/n): ";
                        char confirm;
                        cin >> confirm;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (tolower(confirm) != 'y') {
                            cout << "Operasi dibatalkan.\n";
                            continue;
                        }
                    }
                MAX_MOTOR_SPOTS_PER_FLOOR = newMotorSpots;
                cout << "Kapasitas spot motor per lantai diubah menjadi " << MAX_MOTOR_SPOTS_PER_FLOOR << ".\n";
                initializeParking();
    loadParkedVehiclesFromCSV();
            } else {
                cout << "Kapasitas spot motor per lantai tidak berubah.\n";
            }
        } else if (settingsChoice == "4") {
            int newQueueSize = getValidIntegerInput("Masukkan ukuran antrean masuk baru (min 1, max " + to_string(MAX_QUEUE_CAPACITY) + "): ", 1, MAX_QUEUE_CAPACITY);
            if (newQueueSize != CURRENT_QUEUE_SIZE) {
                if (newQueueSize < entryQueue.count) {
                    cout << "Ukuran antrean baru lebih kecil dari jumlah kendaraan yang sedang antre (" << entryQueue.count << ").\n";
                    cout << "Operasi dibatalkan. Kosongkan antrean terlebih dahulu atau gunakan ukuran yang lebih besar.\n";
                } else {
                    CURRENT_QUEUE_SIZE = newQueueSize;
                    cout << "Ukuran antrean masuk diubah menjadi " << CURRENT_QUEUE_SIZE << ".\n";
                }
            } else {
                cout << "Ukuran antrean tidak berubah.\n";
            }
        } else if (settingsChoice == "0") {
            waitForEnter = false;
        } else {
            cout << "Pilihan tidak valid. Silakan pilih antara 0 - 4.\n";
        }
        if (waitForEnter && settingsChoice != "0") {
            cout << "Tekan ENTER untuk melanjutkan...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (settingsChoice != "0");
}

void menuUtama() {
    string pilihan;
    initializeParking();
    loadParkedVehiclesFromCSV();
    cout << "Tekan ENTER untuk melanjutkan...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    do {
        clearScreen();
        cout << "\n--- SISTEM PARKIR BASEMENT MALL ---\n";
        cout << "1. Kendaraan Masuk\n";
        cout << "2. Kendaraan Keluar\n";
        cout << "3. Tampilkan Status Parkir\n";
        cout << "4. Pengaturan\n";
        cout << "0. Keluar\n";
        cout << "Pilih: ";
        cin >> pilihan;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (pilihan == "1") {
            clearScreen();
            vehicleEntry();
        } else if (pilihan == "2") {
            clearScreen();
            vehicleExit();
        } else if (pilihan == "3") {
            clearScreen();
            displayParkingStatus();
        } else if (pilihan == "4") {
            clearScreen();
            settingsMenu();
        } else if (pilihan == "0") {
            clearScreen();
            cout << "Terima kasih telah menggunakan sistem parkir.\n";
        } else {
            clearScreen();
            cout << "Pilihan tidak valid. Silakan pilih antara 0 - 4.\n";
            cout << "Tekan ENTER untuk melanjutkan...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (pilihan != "0");
}

int main() {
    menuUtama();
    deallocateParkingSpots();
    delete parkedVehiclesTree; // Ensures all nodes are deleted.
    return 0;
}