#ifndef BLOODDONATION_H
#define BLOODDONATION_H

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
using namespace std;

// ==================== VALIDATION FUNCTIONS ====================
bool isValidName(string name);
bool isValidAge(int age);
bool isValidBloodGroup(string bg);
bool isValidContact(string contact);
bool isValidDate(string date);
bool isValidUnits(int units);
bool isAlpha(char c);
bool isDigit(char c);

class BloodStockBST;

// ==================== DONOR LINKED LIST ====================
struct Donor {
    string name;
    int age;
    string bloodGroup;
    string city;
    string contact;
    string lastDonationDate;
    Donor* next;
};

class DonorList {
private:
    Donor* head;
public:
    DonorList();
    ~DonorList();
    void insertDonor(string name, int age, string bg, string city, string contact, string date);
    void deleteDonor(string name);
    Donor* searchByName(string name);
    void searchByBloodGroup(string bg);
    void displayAll();
    void sortDonors();
    void saveToFile(string filename);
    void loadFromFile(string filename);
    Donor* getHead() { return head; }
};

// ==================== DONATION REQUEST QUEUE ====================
struct Request {
    string patientName;
    string bloodGroup;
    int unitsNeeded;
    string hospital;
    Request* next;
};

class RequestQueue {
private:
    Request* front;
    Request* rear;
public:
    RequestQueue();
    ~RequestQueue();
    void enqueue(string patient, string bg, int units, string hospital);
    bool processNext(class BloodStockBST& stock);
    void dequeue();
    void display();
    void saveToFile(string filename);
    void loadFromFile(string filename);
};

// ==================== DONATION HISTORY STACK ====================
struct HistoryRecord {
    string donorName;
    string bloodGroup;
    string date;
    HistoryRecord* next;
};

class HistoryStack {
private:
    HistoryRecord* top;
public:
    HistoryStack();
    ~HistoryStack();
    void push(string donor, string bg, string date);
    void pop();
    void display();
    void saveToFile(string filename);
    void loadFromFile(string filename);
};

// ==================== BLOOD STOCK BST ====================
struct StockNode {
    string bloodGroup;
    int unitsAvailable;
    StockNode* left;
    StockNode* right;
};

class BloodStockBST {
private:
    StockNode* root;
    StockNode* insertNode(StockNode* node, string bg, int units);
    void inorderTraversal(StockNode* node);
    StockNode* searchNode(StockNode* node, string bg);
    void destroyTree(StockNode* node);
public:
    BloodStockBST();
    ~BloodStockBST();
    void insert(string bg, int units);
    void updateUnits(string bg, int units);
    int getUnits(string bg);
    bool consumeUnits(string bg, int units);
    void searchStock(string bg);
    void displayStock();
    void saveToFile(string filename);
    bool loadFromFile(string filename);
    void initializeStock();
};

// ==================== BLOOD BANK GRAPH ====================
class BloodBankGraph {
private:
    static const int MAX_BANKS = 10;
    string bankNames[MAX_BANKS];
    int adjMatrix[MAX_BANKS][MAX_BANKS];
    int numBanks;
public:
    BloodBankGraph();
    void addBank(string name);
    void addRoute(int from, int to, int distance);
    void displayGraph();
    void findClosestBank(int currentBank);
    int getBankIndex(string name);
};

// ==================== MAIN SYSTEM CLASS ====================
class BloodDonationSystem {
private:
    DonorList donors;
    RequestQueue requests;
    HistoryStack history;
    BloodStockBST stock;
    BloodBankGraph graph;
public:
    BloodDonationSystem();
    void run();
    void mainMenu();
    void donorMenu();
    void requestMenu();
    void stockMenu();
    void graphMenu();
    void loadAllData();
    void saveAllData();
    void compatibilityMenu();
};

#endif