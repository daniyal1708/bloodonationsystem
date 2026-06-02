#include "BloodDonation.h"
#include <iomanip>
#include <sstream>
#include <vector>
#include <functional>
#include <limits>
#include <algorithm>
#include <cctype>

string toUpper(string s) {
    for (char& c : s) {
        c = toupper(static_cast<unsigned char>(c));
    }
    return s;
}

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? string() : s.substr(start, end - start + 1);
}

string normalizeBloodGroup(string bg) {
    bg = trim(bg);
    bg = toUpper(bg);
    bg.erase(remove(bg.begin(), bg.end(), ' '), bg.end());
    return bg;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseConsole() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

vector<string> getCompatibleDonorGroups(const string& patientBg) {
    vector<string> groups;
    if (!isValidBloodGroup(patientBg)) return groups;

    if (patientBg == "A+") {
        groups = {"A+", "A-", "O+", "O-"};
    }
    else if (patientBg == "A-") {
        groups = {"A-", "O-"};
    }
    else if (patientBg == "B+") {
        groups = {"B+", "B-", "O+", "O-"};
    }
    else if (patientBg == "B-") {
        groups = {"B-", "O-"};
    }
    else if (patientBg == "AB+") {
        groups = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    }
    else if (patientBg == "AB-") {
        groups = {"A-", "B-", "AB-", "O-"};
    }
    else if (patientBg == "O+") {
        groups = {"O+", "O-"};
    }
    else if (patientBg == "O-") {
        groups = {"O-"};
    }

    return groups;
}

// ==================== VALIDATION FUNCTIONS ====================
bool isAlpha(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}

bool isValidName(string name) {
    if (name.empty() || name.length() < 2 || name.length() > 50) {
        cout << "\n✗ Name must be 2-50 characters long!" << endl;
        return false;
    }

    for (int i = 0; i < name.length(); i++) {
        if (!isAlpha(name[i]) && name[i] != ' ') {
            cout << "\n✗ Name can only contain letters and spaces!" << endl;
            return false;
        }
    }
    return true;
}

bool isValidAge(int age) {
    if (age < 18 || age > 65) {
        cout << "\n✗ Age must be between 18 and 65!" << endl;
        return false;
    }
    return true;
}

bool isValidBloodGroup(string bg) {
    if (bg == "A+" || bg == "A-" || bg == "B+" || bg == "B-" ||
        bg == "O+" || bg == "O-" || bg == "AB+" || bg == "AB-") {
        return true;
    }
    cout << "\n✗ Invalid blood group! Valid: A+, A-, B+, B-, O+, O-, AB+, AB-" << endl;
    return false;
}

bool isValidContact(string contact) {
    if (contact.length() < 10 || contact.length() > 15) {
        cout << "\n✗ Contact must be 10-15 characters long!" << endl;
        return false;
    }

    for (int i = 0; i < contact.length(); i++) {
        if (!isDigit(contact[i]) && contact[i] != '-' && contact[i] != '+' && contact[i] != ' ') {
            cout << "\n✗ Contact can only contain digits, +, -, and spaces!" << endl;
            return false;
        }
    }
    return true;
}

bool isValidDate(string date) {
    // Simple format  DD/MM/YYYY 
    if (date.length() != 10) {
        cout << "\n✗ Date must be in format DD/MM/YYYY or DD-MM-YYYY!" << endl;
        return false;
    }

    if ((date[2] != '/' && date[2] != '-') || (date[5] != '/' && date[5] != '-')) {
        cout << "\n✗ Date must be in format DD/MM/YYYY or DD-MM-YYYY!" << endl;
        return false;
    }

    for (int i = 0; i < date.length(); i++) {
        if (i != 2 && i != 5) {
            if (!isDigit(date[i])) {
                cout << "\n✗ Invalid date format!" << endl;
                return false;
            }
        }
    }
    return true;
}

bool isValidUnits(int units) {
    if (units <= 0 || units > 100) {
        cout << "\n✗ Units must be between 1 and 100!" << endl;
        return false;
    }
    return true;
}

// ==================== DONOR LINKED LIST IMPLEMENTATION ====================
DonorList::DonorList() : head(nullptr) {}

DonorList::~DonorList() {
    Donor* current = head;
    while (current != nullptr) {
        Donor* temp = current;
        current = current->next;
        delete temp;
    }
}

void DonorList::insertDonor(string name, int age, string bg, string city, string contact, string date) {
    // Validation
    if (!isValidName(name)) return;
    if (!isValidAge(age)) return;
    if (!isValidBloodGroup(bg)) return;
    if (!isValidName(city)) return;
    if (!isValidContact(contact)) return;
    if (!isValidDate(date)) return;

    Donor* newDonor = new Donor;
    newDonor->name = name;
    newDonor->age = age;
    newDonor->bloodGroup = bg;
    newDonor->city = city;
    newDonor->contact = contact;
    newDonor->lastDonationDate = date;
    newDonor->next = head;
    head = newDonor;
    cout << "\n✓ Donor added successfully!" << endl;
}

void DonorList::deleteDonor(string name) {
    if (!isValidName(name)) return;

    if (head == nullptr) {
        cout << "\n✗ No donors in the system!" << endl;
        return;
    }

    if (head->name == name) {
        Donor* temp = head;
        head = head->next;
        delete temp;
        cout << "\n✓ Donor deleted successfully!" << endl;
        return;
    }

    Donor* current = head;
    while (current->next != nullptr && current->next->name != name) {
        current = current->next;
    }

    if (current->next == nullptr) {
        cout << "\n✗ Donor not found!" << endl;
    }
    else {
        Donor* temp = current->next;
        current->next = current->next->next;
        delete temp;
        cout << "\n✓ Donor deleted successfully!" << endl;
    }
}

Donor* DonorList::searchByName(string name) {
    if (!isValidName(name)) return nullptr;

    Donor* current = head;
    while (current != nullptr) {
        if (current->name == name) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void DonorList::searchByBloodGroup(string bg) {
    if (!isValidBloodGroup(bg)) return;

    Donor* current = head;
    bool found = false;

    cout << "\n========== DONORS WITH BLOOD GROUP " << bg << " ==========\n";
    while (current != nullptr) {
        if (current->bloodGroup == bg) {
            cout << "\nName: " << current->name;
            cout << "\nAge: " << current->age;
            cout << "\nCity: " << current->city;
            cout << "\nContact: " << current->contact;
            cout << "\nLast Donation: " << current->lastDonationDate;
            cout << "\n" << string(50, '-') << endl;
            found = true;
        }
        current = current->next;
    }

    if (!found) {
        cout << "No donors found with blood group " << bg << endl;
    }
}

void DonorList::displayAll() {
    if (head == nullptr) {
        cout << "\n✗ No donors in the system!" << endl;
        return;
    }

    cout << "\n========== ALL DONORS ==========\n";
    Donor* current = head;
    int count = 1;

    while (current != nullptr) {
        cout << "\n[" << count++ << "] " << current->name;
        cout << " | Age: " << current->age;
        cout << " | Blood: " << current->bloodGroup;
        cout << " | City: " << current->city;
        cout << " | Contact: " << current->contact;
        cout << " | Last Donation: " << current->lastDonationDate << endl;
        current = current->next;
    }
}

void DonorList::sortDonors() {
    if (head == nullptr || head->next == nullptr) return;

    bool swapped;
    Donor* ptr1;
    Donor* lptr = nullptr;

    do {
        swapped = false;
        ptr1 = head;

        while (ptr1->next != lptr) {
            if (ptr1->name > ptr1->next->name) {
                // Swap data
                string tempName = ptr1->name;
                int tempAge = ptr1->age;
                string tempBG = ptr1->bloodGroup;
                string tempCity = ptr1->city;
                string tempContact = ptr1->contact;
                string tempDate = ptr1->lastDonationDate;

                ptr1->name = ptr1->next->name;
                ptr1->age = ptr1->next->age;
                ptr1->bloodGroup = ptr1->next->bloodGroup;
                ptr1->city = ptr1->next->city;
                ptr1->contact = ptr1->next->contact;
                ptr1->lastDonationDate = ptr1->next->lastDonationDate;

                ptr1->next->name = tempName;
                ptr1->next->age = tempAge;
                ptr1->next->bloodGroup = tempBG;
                ptr1->next->city = tempCity;
                ptr1->next->contact = tempContact;
                ptr1->next->lastDonationDate = tempDate;

                swapped = true;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);

    cout << "\n✓ Donors sorted by name!" << endl;
}

void DonorList::saveToFile(string filename) {
    ofstream file(filename);
    if (!file) {
        cout << "\n✗ Error opening file!" << endl;
        return;
    }

    Donor* current = head;
    while (current != nullptr) {
        file << current->name << "|" << current->age << "|"
            << current->bloodGroup << "|" << current->city << "|"
            << current->contact << "|" << current->lastDonationDate << "\n";
        current = current->next;
    }
    file.close();
    cout << "\n✓ Donors saved to file!" << endl;
}

void DonorList::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file) return;

    string line, name, bg, city, contact, date;
    int age;
    char delim;

    while (getline(file, name, '|')) {
        file >> age >> delim;
        getline(file, bg, '|');
        getline(file, city, '|');
        getline(file, contact, '|');
        getline(file, date);

        // Skip validation on load to allow existing data
        Donor* newDonor = new Donor;
        newDonor->name = name;
        newDonor->age = age;
        newDonor->bloodGroup = bg;
        newDonor->city = city;
        newDonor->contact = contact;
        newDonor->lastDonationDate = date;
        newDonor->next = head;
        head = newDonor;
    }
    file.close();
}

// ==================== REQUEST QUEUE IMPLEMENTATION ====================
RequestQueue::RequestQueue() : front(nullptr), rear(nullptr) {}

RequestQueue::~RequestQueue() {
    while (front != nullptr) {
        Request* temp = front;
        front = front->next;
        delete temp;
    }
}

void RequestQueue::enqueue(string patient, string bg, int units, string hospital) {
    // Validation
    if (!isValidName(patient)) return;
    if (!isValidBloodGroup(bg)) return;
    if (!isValidUnits(units)) return;
    if (hospital.empty()) {
        cout << "\n✗ Hospital name cannot be empty!" << endl;
        return;
    }

    Request* newRequest = new Request;
    newRequest->patientName = patient;
    newRequest->bloodGroup = bg;
    newRequest->unitsNeeded = units;
    newRequest->hospital = hospital;
    newRequest->next = nullptr;

    if (rear == nullptr) {
        front = rear = newRequest;
    }
    else {
        rear->next = newRequest;
        rear = newRequest;
    }
    cout << "\n✓ Request added to queue!" << endl;
}

bool RequestQueue::processNext(BloodStockBST& stock) {
    if (front == nullptr) {
        cout << "\n✗ No requests in queue!" << endl;
        return false;
    }

    if (!stock.consumeUnits(front->bloodGroup, front->unitsNeeded)) {
        cout << "\n✗ Not enough blood stock to fulfill the request for: " << front->patientName << endl;
        return false;
    }

    Request* temp = front;
    cout << "\n✓ Request fulfilled for: " << front->patientName << endl;
    cout << "   Blood Group: " << front->bloodGroup << " | Units: " << front->unitsNeeded << endl;
    front = front->next;

    if (front == nullptr) {
        rear = nullptr;
    }

    delete temp;
    return true;
}

void RequestQueue::dequeue() {
    if (front == nullptr) {
        cout << "\n✗ No requests in queue!" << endl;
        return;
    }

    Request* temp = front;
    cout << "\n✓ Processing request for: " << front->patientName << endl;
    front = front->next;

    if (front == nullptr) {
        rear = nullptr;
    }

    delete temp;
}

void RequestQueue::display() {
    if (front == nullptr) {
        cout << "\n✗ No pending requests!" << endl;
        return;
    }

    cout << "\n========== PENDING REQUESTS ==========\n";
    Request* current = front;
    int count = 1;

    while (current != nullptr) {
        cout << "\n[" << count++ << "] Patient: " << current->patientName;
        cout << "\n    Blood Group: " << current->bloodGroup;
        cout << "\n    Units Needed: " << current->unitsNeeded;
        cout << "\n    Hospital: " << current->hospital;
        cout << "\n" << string(50, '-') << endl;
        current = current->next;
    }
}

void RequestQueue::saveToFile(string filename) {
    ofstream file(filename);
    if (!file) return;

    Request* current = front;
    while (current != nullptr) {
        file << current->patientName << "|" << current->bloodGroup << "|"
            << current->unitsNeeded << "|" << current->hospital << "\n";
        current = current->next;
    }
    file.close();
}

void RequestQueue::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file) return;

    string patient, bg, hospital;
    int units;
    char delim;

    while (getline(file, patient, '|')) {
        getline(file, bg, '|');
        file >> units >> delim;
        getline(file, hospital);

        // Skip validation on load
        Request* newRequest = new Request;
        newRequest->patientName = patient;
        newRequest->bloodGroup = bg;
        newRequest->unitsNeeded = units;
        newRequest->hospital = hospital;
        newRequest->next = nullptr;

        if (rear == nullptr) {
            front = rear = newRequest;
        }
        else {
            rear->next = newRequest;
            rear = newRequest;
        }
    }
    file.close();
}

// ==================== HISTORY STACK IMPLEMENTATION ====================
HistoryStack::HistoryStack() : top(nullptr) {}

HistoryStack::~HistoryStack() {
    while (top != nullptr) {
        HistoryRecord* temp = top;
        top = top->next;
        delete temp;
    }
}

void HistoryStack::push(string donor, string bg, string date) {
    // Validation
    if (!isValidName(donor)) return;
    if (!isValidBloodGroup(bg)) return;
    if (!isValidDate(date)) return;

    HistoryRecord* newRecord = new HistoryRecord;
    newRecord->donorName = donor;
    newRecord->bloodGroup = bg;
    newRecord->date = date;
    newRecord->next = top;
    top = newRecord;
    cout << "\n✓ Donation recorded in history!" << endl;
}

void HistoryStack::pop() {
    if (top == nullptr) {
        cout << "\n✗ No history records!" << endl;
        return;
    }

    HistoryRecord* temp = top;
    cout << "\n✓ Removed: " << top->donorName << " - " << top->bloodGroup
        << " on " << top->date << endl;
    top = top->next;
    delete temp;
}

void HistoryStack::display() {
    if (top == nullptr) {
        cout << "\n✗ No donation history!" << endl;
        return;
    }

    cout << "\n========== DONATION HISTORY (Recent First) ==========\n";
    HistoryRecord* current = top;
    int count = 1;

    while (current != nullptr) {
        cout << "[" << count++ << "] " << current->donorName
            << " | " << current->bloodGroup
            << " | " << current->date << endl;
        current = current->next;
    }
}

void HistoryStack::saveToFile(string filename) {
    ofstream file(filename);
    if (!file) return;

    HistoryRecord* current = top;
    while (current != nullptr) {
        file << current->donorName << "|" << current->bloodGroup
            << "|" << current->date << "\n";
        current = current->next;
    }
    file.close();
}

void HistoryStack::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file) return;

    string donor, bg, date;

    while (getline(file, donor, '|')) {
        getline(file, bg, '|');
        getline(file, date);

        // Skip validation on load
        HistoryRecord* newRecord = new HistoryRecord;
        newRecord->donorName = donor;
        newRecord->bloodGroup = bg;
        newRecord->date = date;
        newRecord->next = top;
        top = newRecord;
    }
    file.close();
}

// ==================== BLOOD STOCK BST IMPLEMENTATION ====================
BloodStockBST::BloodStockBST() : root(nullptr) {}

BloodStockBST::~BloodStockBST() {
    destroyTree(root);
}

void BloodStockBST::destroyTree(StockNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

StockNode* BloodStockBST::insertNode(StockNode* node, string bg, int units) {
    if (node == nullptr) {
        StockNode* newNode = new StockNode;
        newNode->bloodGroup = bg;
        newNode->unitsAvailable = units;
        newNode->left = newNode->right = nullptr;
        return newNode;
    }

    if (bg < node->bloodGroup) {
        node->left = insertNode(node->left, bg, units);
    }
    else if (bg > node->bloodGroup) {
        node->right = insertNode(node->right, bg, units);
    }
    else {
        node->unitsAvailable = units;
    }

    return node;
}

void BloodStockBST::insert(string bg, int units) {
    if (!isValidBloodGroup(bg)) return;
    if (units < 0) {
        cout << "\n✗ Units cannot be negative!" << endl;
        return;
    }

    root = insertNode(root, bg, units);
}

void BloodStockBST::updateUnits(string bg, int units) {
    if (!isValidBloodGroup(bg)) return;

    StockNode* node = searchNode(root, bg);
    if (node != nullptr) {
        node->unitsAvailable += units;
        if (node->unitsAvailable < 0) {
            cout << "\n⚠ Warning: Stock is negative! Current units: " << node->unitsAvailable << endl;
        }
        else {
            cout << "\n✓ Stock updated! New units: " << node->unitsAvailable << endl;
        }
    }
    else {
        cout << "\n✗ Blood group not found!" << endl;
    }
}

int BloodStockBST::getUnits(string bg) {
    if (!isValidBloodGroup(bg)) return -1;

    StockNode* node = searchNode(root, bg);
    return node ? node->unitsAvailable : -1;
}

bool BloodStockBST::consumeUnits(string bg, int units) {
    if (!isValidBloodGroup(bg)) return false;
    if (units <= 0) {
        cout << "\n✗ Units must be greater than zero!" << endl;
        return false;
    }

    StockNode* node = searchNode(root, bg);
    if (node == nullptr) {
        cout << "\n✗ Blood group not found!" << endl;
        return false;
    }

    if (node->unitsAvailable < units) {
        cout << "\n✗ Insufficient stock for " << bg << ". Available: " << node->unitsAvailable << ", needed: " << units << endl;
        return false;
    }

    node->unitsAvailable -= units;
    cout << "\n✓ Stock consumed. Remaining " << bg << " units: " << node->unitsAvailable << endl;
    return true;
}

StockNode* BloodStockBST::searchNode(StockNode* node, string bg) {
    if (node == nullptr || node->bloodGroup == bg) {
        return node;
    }

    if (bg < node->bloodGroup) {
        return searchNode(node->left, bg);
    }
    return searchNode(node->right, bg);
}

void BloodStockBST::searchStock(string bg) {
    if (!isValidBloodGroup(bg)) return;

    StockNode* node = searchNode(root, bg);
    if (node != nullptr) {
        cout << "\n========== BLOOD STOCK INFO ==========";
        cout << "\nBlood Group: " << node->bloodGroup;
        cout << "\nUnits Available: " << node->unitsAvailable << endl;
    }
    else {
        cout << "\n✗ Blood group not found in stock!" << endl;
    }
}

void BloodStockBST::inorderTraversal(StockNode* node) {
    if (node != nullptr) {
        inorderTraversal(node->left);
        cout << node->bloodGroup << ": " << node->unitsAvailable << " units" << endl;
        inorderTraversal(node->right);
    }
}

void BloodStockBST::displayStock() {
    if (root == nullptr) {
        cout << "\n✗ No stock data available!" << endl;
        return;
    }

    cout << "\n========== BLOOD STOCK (Sorted) ==========\n";
    inorderTraversal(root);
}

void BloodStockBST::initializeStock() {
    insert("A+", 50);
    insert("A-", 30);
    insert("B+", 40);
    insert("B-", 25);
    insert("O+", 60);
    insert("O-", 35);
    insert("AB+", 20);
    insert("AB-", 15);
}

void BloodStockBST::saveToFile(string filename) {
    ofstream file(filename);
    if (!file) return;

    function<void(StockNode*)> writeNode = [&](StockNode* node) {
        if (node == nullptr) return;
        writeNode(node->left);
        file << node->bloodGroup << "|" << node->unitsAvailable << "\n";
        writeNode(node->right);
    };
    writeNode(root);
    file.close();
}

bool BloodStockBST::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file) return false;

    // Clear existing stock tree if present
    destroyTree(root);
    root = nullptr;

    string line;
    bool loaded = false;

    while (getline(file, line)) {
        if (line.empty()) continue;
        string bg;
        int units;
        char delim;
        stringstream ss(line);
        if (!getline(ss, bg, '|')) continue;
        if (!(ss >> units)) continue;
        if (!isValidBloodGroup(bg)) continue;
        insert(bg, units);
        loaded = true;
    }

    file.close();
    return loaded;
}

// ==================== BLOOD BANK GRAPH IMPLEMENTATION ====================
BloodBankGraph::BloodBankGraph() : numBanks(0) {
    for (int i = 0; i < MAX_BANKS; i++) {
        for (int j = 0; j < MAX_BANKS; j++) {
            adjMatrix[i][j] = 0;
        }
    }
}

void BloodBankGraph::addBank(string name) {
    if (name.empty()) {
        cout << "\n✗ Bank name cannot be empty!" << endl;
        return;
    }

    if (numBanks < MAX_BANKS) {
        bankNames[numBanks] = name;
        numBanks++;
        cout << "\n✓ Blood bank added!" << endl;
    }
    else {
        cout << "\n✗ Maximum banks reached!" << endl;
    }
}

void BloodBankGraph::addRoute(int from, int to, int distance) {
    if (from < 0 || to < 0 || from >= numBanks || to >= numBanks) {
        cout << "\n✗ Invalid bank indices!" << endl;
        return;
    }

    if (distance <= 0) {
        cout << "\n✗ Distance must be positive!" << endl;
        return;
    }

    adjMatrix[from][to] = distance;
    adjMatrix[to][from] = distance;
    cout << "\n✓ Route added!" << endl;
}

void BloodBankGraph::displayGraph() {
    cout << "\n========== BLOOD BANK NETWORK ==========\n";
    cout << "\nBanks:\n";
    for (int i = 0; i < numBanks; i++) {
        cout << "[" << i << "] " << bankNames[i] << endl;
    }

    cout << "\nRoutes (Distances in km):\n";
    for (int i = 0; i < numBanks; i++) {
        for (int j = i + 1; j < numBanks; j++) {
            if (adjMatrix[i][j] > 0) {
                cout << bankNames[i] << " <-> " << bankNames[j]
                    << " : " << adjMatrix[i][j] << " km" << endl;
            }
        }
    }
}

void BloodBankGraph::findClosestBank(int currentBank) {
    if (currentBank < 0 || currentBank >= numBanks) {
        cout << "\n✗ Invalid bank index!" << endl;
        return;
    }

    int minDist = 99999;
    int closestBank = -1;

    for (int i = 0; i < numBanks; i++) {
        if (i != currentBank && adjMatrix[currentBank][i] > 0) {
            if (adjMatrix[currentBank][i] < minDist) {
                minDist = adjMatrix[currentBank][i];
                closestBank = i;
            }
        }
    }

    if (closestBank != -1) {
        cout << "\n✓ Closest bank to " << bankNames[currentBank] << " is ";
        cout << bankNames[closestBank] << " (" << minDist << " km)" << endl;
    }
    else {
        cout << "\n✗ No connected banks found!" << endl;
    }
}

int BloodBankGraph::getBankIndex(string name) {
    for (int i = 0; i < numBanks; i++) {
        if (bankNames[i] == name) return i;
    }
    return -1;
}

// ==================== MAIN SYSTEM IMPLEMENTATION ====================
BloodDonationSystem::BloodDonationSystem() {
    loadAllData();
}

void BloodDonationSystem::loadAllData() {
    donors.loadFromFile("donors.txt");
    requests.loadFromFile("requests.txt");
    history.loadFromFile("history.txt");
    if (!stock.loadFromFile("blood_stock.txt")) {
        stock.initializeStock();
    }

    // Initialize sample blood banks
    graph.addBank("City Hospital Blood Bank");
    graph.addBank("Red Cross Center");
    graph.addBank("General Hospital");
    graph.addBank("Medical Center Blood Bank");
    graph.addRoute(0, 1, 5);
    graph.addRoute(0, 2, 8);
    graph.addRoute(1, 2, 3);
    graph.addRoute(1, 3, 6);
    graph.addRoute(2, 3, 4);
}

void BloodDonationSystem::saveAllData() {
    donors.saveToFile("donors.txt");
    requests.saveToFile("requests.txt");
    history.saveToFile("history.txt");
    stock.saveToFile("blood_stock.txt");
}

void BloodDonationSystem::donorMenu() {
    int choice;
    do {
        clearScreen();
        cout << "\n╔════════════════════════════════════╗";
        cout << "\n║      DONOR MANAGEMENT MENU         ║";
        cout << "\n╠════════════════════════════════════╣";
        cout << "\n║ 1. Add New Donor                   ║";
        cout << "\n║ 2. Delete Donor                    ║";
        cout << "\n║ 3. Search Donor by Name            ║";
        cout << "\n║ 4. Search by Blood Group           ║";
        cout << "\n║ 5. Display All Donors              ║";
        cout << "\n║ 6. Sort Donors by Name             ║";
        cout << "\n║ 0. Back to Main Menu               ║";
        cout << "\n╚════════════════════════════════════╝";
        cout << "\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n✗ Invalid input! Please enter a number." << endl;
            continue;
        }
        cin.ignore();

        string name, bg, city, contact, date;
        int age;

        switch (choice) {
        case 1:
            cout << "\nEnter Name: "; getline(cin, name);
            cout << "Enter Age: ";
            if (!(cin >> age)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\n✗ Invalid age input!" << endl;
                break;
            }
            cin.ignore();
            cout << "Enter Blood Group (A+/A-/B+/B-/O+/O-/AB+/AB-): "; getline(cin, bg);
            bg = normalizeBloodGroup(bg);
            cout << "Enter City: "; getline(cin, city);
            cout << "Enter Contact: "; getline(cin, contact);
            cout << "Enter Last Donation Date (DD/MM/YYYY): "; getline(cin, date);
            donors.insertDonor(name, age, bg, city, contact, date);
            break;
        case 2:
            cout << "\nEnter Donor Name to Delete: "; getline(cin, name);
            donors.deleteDonor(name);
            break;
        case 3:
            cout << "\nEnter Donor Name: "; getline(cin, name);
            {
                Donor* d = donors.searchByName(name);
                if (d) {
                    cout << "\n✓ Donor Found!";
                    cout << "\nName: " << d->name;
                    cout << "\nAge: " << d->age;
                    cout << "\nBlood Group: " << d->bloodGroup;
                    cout << "\nCity: " << d->city;
                    cout << "\nContact: " << d->contact;
                    cout << "\nLast Donation: " << d->lastDonationDate << endl;
                }
                else if (name.length() >= 2) {
                    cout << "\n✗ Donor not found!" << endl;
                }
            }
            break;
        case 4:
            cout << "\nEnter Blood Group: "; getline(cin, bg);
            bg = normalizeBloodGroup(bg);
            donors.searchByBloodGroup(bg);
            break;
        case 5:
            donors.displayAll();
            break;
        case 6:
            donors.sortDonors();
            break;
        case 0:
            break;
        default:
            cout << "\n✗ Invalid choice!" << endl;
        }
        if (choice != 0) pauseConsole();
    } while (choice != 0);
}

void BloodDonationSystem::requestMenu() {
    int choice;
    do {
        clearScreen();
        cout << "\n╔╔═══════════════════════════════════╗";
        cout << "\n║   DONATION REQUEST MANAGEMENT      ║";
        cout << "\n╠════════════════════════════════════╣";
        cout << "\n║ 1. Add New Request                 ║";
        cout << "\n║ 2. Process Next Request            ║";
        cout << "\n║ 3. View Pending Requests           ║";
        cout << "\n║ 4. Record Donation in History      ║";
        cout << "\n║ 5. View Donation History           ║";
        cout << "\n║ 0. Back to Main Menu               ║";
        cout << "\n╚════════════════════════════════════╝";
        cout << "\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n✗ Invalid input! Please enter a number." << endl;
            continue;
        }
        cin.ignore();

        string patient, bg, hospital, donor, date;
        int units;

        switch (choice) {
        case 1:
            cout << "\nEnter Patient Name: "; getline(cin, patient);
            cout << "Enter Blood Group (A+/A-/B+/B-/O+/O-/AB+/AB-): "; getline(cin, bg);
            bg = normalizeBloodGroup(bg);
            cout << "Enter Units Needed: ";
            if (!(cin >> units)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\n✗ Invalid units input!" << endl;
                break;
            }
            cin.ignore();
            cout << "Enter Hospital Name: "; getline(cin, hospital);
            requests.enqueue(patient, bg, units, hospital);
            break;
        case 2:
            requests.processNext(stock);
            break;
        case 3:
            requests.display();
            break;
        case 4:
            cout << "\nEnter Donor Name: "; getline(cin, donor);
            cout << "Enter Blood Group (A+/A-/B+/B-/O+/O-/AB+/AB-): "; getline(cin, bg);
            bg = normalizeBloodGroup(bg);
            cout << "Enter Donation Date (DD/MM/YYYY): "; getline(cin, date);
            history.push(donor, bg, date);
            break;
        case 5:
            history.display();
            break;
        case 0:
            break;
        default:
            cout << "\n✗ Invalid choice!" << endl;
        }
        if (choice != 0) pauseConsole();
    } while (choice != 0);
}

void BloodDonationSystem::stockMenu() {
    int choice;
    do {
        clearScreen();
        cout << "\n╔════════════════════════════════════╗";
        cout << "\n║        BLOOD STOCK MANAGEMENT      ║";
        cout << "\n╠════════════════════════════════════╣";
        cout << "\n║ 1. Display Blood Stock             ║";
        cout << "\n║ 2. Update Blood Stock              ║";
        cout << "\n║ 3. Search Blood Group Stock        ║";
        cout << "\n║ 4. Check Compatible Blood Groups    ║";
        cout << "\n║ 0. Back to Main Menu               ║";
        cout << "\n╚════════════════════════════════════╝";
        cout << "\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n✗ Invalid input! Please enter a number." << endl;
            continue;
        }
        cin.ignore();

        string bg;
        int units;

        switch (choice) {
        case 1:
            stock.displayStock();
            break;
        case 2:
            cout << "\nEnter Blood Group to update: "; getline(cin, bg);
            bg = normalizeBloodGroup(bg);
            cout << "Enter units to add/subtract: ";
            if (!(cin >> units)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\n✗ Invalid units input!" << endl;
                break;
            }
            cin.ignore();
            stock.updateUnits(bg, units);
            break;
        case 3:
            cout << "\nEnter Blood Group to search: "; getline(cin, bg);
            bg = normalizeBloodGroup(bg);
            stock.searchStock(bg);
            break;
        case 4:
            compatibilityMenu();
            break;
        case 0:
            break;
        default:
            cout << "\n✗ Invalid choice!" << endl;
        }
        if (choice != 0) pauseConsole();
    } while (choice != 0);
}

void BloodDonationSystem::graphMenu() {
    int choice;
    do {
        clearScreen();
        cout << "\n╔════════════════════════════════════╗";
        cout << "\n║      BLOOD BANK NETWORK MENU       ║";
        cout << "\n╠════════════════════════════════════╣";
        cout << "\n║ 1. Display Blood Banks             ║";
        cout << "\n║ 2. Find Closest Bank               ║";
        cout << "\n║ 0. Back to Main Menu               ║";
        cout << "\n╚════════════════════════════════════╝";
        cout << "\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n✗ Invalid input! Please enter a number." << endl;
            continue;
        }
        cin.ignore();

        switch (choice) {
        case 1:
            graph.displayGraph();
            break;
        case 2: {
            string bankName;
            cout << "\nEnter current bank name: "; getline(cin, bankName);
            int index = graph.getBankIndex(bankName);
            if (index >= 0) {
                graph.findClosestBank(index);
            }
            else {
                cout << "\n✗ Bank not found!" << endl;
            }
            break;
        }
        case 0:
            break;
        default:
            cout << "\n✗ Invalid choice!" << endl;
        }
        if (choice != 0) pauseConsole();
    } while (choice != 0);
}

void BloodDonationSystem::compatibilityMenu() {
    clearScreen();
    string bg;
    cout << "\nEnter patient blood group (A+/A-/B+/B-/O+/O-/AB+/AB-): ";
    getline(cin, bg);
    bg = normalizeBloodGroup(bg);
    vector<string> compatible = getCompatibleDonorGroups(bg);
    if (compatible.empty()) {
        cout << "\n✗ Invalid blood group. Please enter A+, A-, B+, B-, O+, O-, AB+, or AB-." << endl;
        pauseConsole();
        return;
    }

    cout << "\nCompatible donor blood groups for " << bg << ":\n";
    for (auto& group : compatible) {
        cout << "  - " << group << endl;
    }
    pauseConsole();
}

void BloodDonationSystem::mainMenu() {
    int choice;
    do {
        clearScreen();
        cout << "\n╔════════════════════════════════════╗";
        cout << "\n║      BLOOD DONATION SYSTEM         ║";
        cout << "\n╠════════════════════════════════════╣";
        cout << "\n║ 1. Donor Management               ║";
        cout << "\n║ 2. Donation Request Management    ║";
        cout << "\n║ 3. Blood Stock Management         ║";
        cout << "\n║ 4. Blood Bank Network             ║";
        cout << "\n║ 5. Blood Compatibility Checker    ║";
        cout << "\n║ 0. Save and Exit                  ║";
        cout << "\n╚════════════════════════════════════╝";
        cout << "\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n✗ Invalid input! Please enter a number." << endl;
            continue;
        }
        cin.ignore();

        switch (choice) {
        case 1:
            donorMenu();
            break;
        case 2:
            requestMenu();
            break;
        case 3:
            stockMenu();
            break;
        case 4:
            graphMenu();
            break;
        case 5:
            compatibilityMenu();
            break;
        case 0:
            cout << "\nSaving data and exiting..." << endl;
            break;
        default:
            cout << "\n✗ Invalid choice!" << endl;
        }
    } while (choice != 0);
}

void BloodDonationSystem::run() {
    mainMenu();
    saveAllData();
}

int main() {
    BloodDonationSystem system;
    system.run();
    return 0;
}
