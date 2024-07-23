#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

using namespace std;


const string FILE_PATH = "accounts.txt";

class Account {
private:
    int accID;
    int balance;
    string cusName;
    int uniqueID;

public:
    /*생성자*/
    Account(int _accID, const string& _cusName, int _balance, int _uniqueID)
        : accID(_accID), balance(_balance), cusName(move(_cusName)), uniqueID(_uniqueID) {}

    /*캡슐화*/
    int getAccID() const { return accID; }
    int getBalance() const { return balance; }
    const string& getCusName() const { return cusName; }
    int getUniqueID() const { return uniqueID; }

    /*입금 함수*/
    void deposit(int amount) {
        balance += amount;
    }

    /*출금 함수
    받아온 금액과 현재 가진 금액을 비교해서 맞으면 참 아니면 거짓을 리턴
    */
    bool withdraw(int amount) {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }

    /*계좌 출력 함수*/
    void displayInfo() const {
        cout << "계좌ID: " << accID << " 이름: " << cusName << " 잔액: " << balance << " 고유ID: " << uniqueID << endl;
    }

    /*파일 저장 함수*/
    void saveToFile(ofstream& file) const {
        file << accID << " " << cusName << " " << balance << " " << uniqueID << endl;
    }
};

class AccountManager {
private:
    
    vector<Account> accArr;

    /*랜덤한 고유 ID 생성 함수*/
    int generateUniqueID() const {
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<int> dis(1000, 9999); // 4자리 랜덤 숫자 생성
        int newID;
        bool idExists;
        do {
            newID = dis(gen); // 새로운 ID 생성
            idExists = any_of(accArr.begin(), accArr.end(), [newID](const Account& acc) {
                return acc.getUniqueID() == newID;
            });
        } while (idExists);

        return newID;
    }

    /*계좌 찾기 함수*/
    Account* findAccount(int accID) {
        auto it = find_if(accArr.begin(), accArr.end(), [accID](Account& acc) {
            return acc.getAccID() == accID;
        });
        return (it != accArr.end()) ? &(*it) : nullptr;
    }

    /*문자열 확인 함수*/
    bool isValidName(const string& name) {
    for (char c : name) {
        if (!isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
    }


public:
    /*계좌 개설 함수
    새로운 계좌 ID를 받고 중복된 ID를 확인하며
    이름과 입금할금액을 받고서 랜덤한고유ID를 받고
    이후 받은 정보들을 전부 파일에 저장
    */
    void makeAccount() {
    int accID, balance;
    string cusName;

    cout << "[계좌개설]" << endl;
    
    while (true) {
        bool idExists = false;
        cout << "ID입력(취소-1): ";
        cin >> accID;
        
        
        if (cin.fail()||accID < -1) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "유효한 정수를 입력하세요." << endl;
            continue;
        }
        if (accID == -1) {
            cout << "계좌 개설을 취소합니다." << endl;
            return; 
        }
        if (accID < 0) {
            cout << "양수인 ID를 입력하세요." << endl;
            continue;
        }

        
        for (const auto& acc : accArr) {
            if (acc.getAccID() == accID) {
                idExists = true;
                break;
            }
        }
        if (!idExists) {
            break;
        } else {
            cout << "이미 존재하는 계좌ID입니다. 다시 입력해주세요." << endl;
        }
    }

    cin.ignore(); 

    while (true) {
        cout << "이름: ";
        getline(cin, cusName);

        
        if (!isValidName(cusName)) {
            cout << "유효한 이름을 입력하세요. (문자만 허용)" << endl;
        } else {
            break;
        }
    }

    while (true) {
        cout << "입금할 금액: ";
        cin >> balance;
        if (cin.fail()||balance < -1) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "유효한 정수를 입력하세요." << endl;
        } else {
            break;
        }
    }

    int uniqueID = generateUniqueID();

    accArr.emplace_back(accID, cusName, balance, uniqueID);

    ofstream file(FILE_PATH, ios::app);
    if (file.is_open()) {
        accArr.back().saveToFile(file);
    } else {
        cout << "파일을 열 수 없습니다." << endl; // 파일 열기 실패 처리
    }
}


    /*입금하는 함수
    입금을 ID를 findAccount함수로 확인후에 ID가있으면
    입금액을 받은후 현재잔액과 함께 출력
    */
    void depositMoney() {
        int accID, amount;
        cout << "[입 금]" << endl;
        cout << "계좌ID: ";
        cin >> accID;

        Account* act = findAccount(accID);
        if (act == nullptr) {
            cout << "해당 계좌ID가 존재하지 않습니다." << endl;
            return;
        }

        cout << "입금액: ";
        cin >> amount;

        act->deposit(amount);
        cout << "입금이 완료되었습니다. 현재 잔액: " << act->getBalance() << endl;
    }

    /*출금하는 함수
    입금과같이 ID를 findAccount함수로 확인하고 ID가있으면
    출금액을 받은 이후 현재잔액과 함께 출력
    */
    void withdrawMoney() {
        int accID, amount;
        cout << "[출 금]" << endl;
        cout << "계좌ID: ";
        cin >> accID;

        Account* act = findAccount(accID);
        if (act == nullptr) {
            cout << "해당 계좌ID가 존재하지 않습니다." << endl;
            return;
        }

        cout << "출금액: ";
        cin >> amount;

        if (act->withdraw(amount)) {
            cout << "출금이 완료되었습니다. 현재 잔액: " << act->getBalance() << endl;
        } else {
            cout << "잔액이 부족합니다." << endl;
        }
    }

    /*계좌 정보를 출력하는 함수
    for문으로 전체정보를 받고 displayinfo 함수에서 정보를 출력
    */
    void showAllAccInfo() const {
        if (accArr.empty()) {
            cout << "등록된 계좌가 없습니다." << endl;
        } else {
            cout << "[계좌정보 전체 출력]" << endl;
            for (const auto& acc : accArr) {
                acc.displayInfo();
            }
        }
    }

    /*계좌 ID삭제 함수
    ID를 입력받은후에 auto it으로 ID가 있는지 확인후에
    있으면 그 ID만 삭제하고 함수종료
    */
    void deleteAccount() {
        int accID;
        cout << "[계좌삭제]" << endl;
        cout << "삭제할 계좌ID: ";
        cin >> accID;

        auto it = find_if(accArr.begin(), accArr.end(), [accID](const Account& acc) {
            return acc.getAccID() == accID;
        });

        if (it != accArr.end()) {
            accArr.erase(it);
            cout << "계좌 삭제 완료" << endl;
        } else {
            cout << "해당 계좌ID가 존재하지 않습니다." << endl;
        }
    }

    /*텍스트 파일을 불러오는 함수
    ifstream으로 복사가 되지않게 파일을 읽고서
    while문으로 ID, 이름, 금액, 고유ID을 파일에있는걸 읽어옴
    */
    void loadAccounts() {
        ifstream file(FILE_PATH);
        if (file.is_open()) {
            int accID, balance, uniqueID;
            string cusName;
            while (file >> accID >> cusName >> balance >> uniqueID) {
                accArr.emplace_back(accID, cusName, balance, uniqueID);
            }
        }
    }

    /*프로그램 종료시 텍스트로 저장하는 함수
    ofsteam으로 복사가 되지않게 파일을 저장하고서
    for문으로 전체정보를 찾아서 saveToFile 함수을 이용해서 저장
    */
    void saveAccounts() const {
        ofstream file(FILE_PATH);
        if (file.is_open()) {
            for (const auto& acc : accArr) {
                acc.saveToFile(file);
            }
        } else {
            cout << "파일을 열 수 없습니다." << endl;
        }
    }

    /*메뉴 출력 함수*/
    void showMenu() const {
        cout << "----Menu----" << endl;
        cout << "1. 계좌개설" << endl;
        cout << "2. 입 금" << endl;
        cout << "3. 출 금" << endl;
        cout << "4. 계좌정보 전체 출력" << endl;
        cout << "5. 계좌삭제" << endl;
        cout << "9. 프로그램 종료" << endl;
    }

    /*프로그램 메인 메뉴 구성 함수*/
    void processMenu() {
        int choice;
        while (true) {
            showMenu();
            cout << "선택: ";
            cin >> choice;
            switch (choice) {
                case 1:
                    makeAccount();
                    break;
                case 2:
                    depositMoney();
                    break;
                case 3:
                    withdrawMoney();
                    break;
                case 4:
                    showAllAccInfo();
                    break;
                case 5:
                    deleteAccount();
                    break;
                case 9:
                    saveAccounts();
                    cout << "프로그램을 종료합니다." << endl;
                    return;
                default:
                    cout << "잘못된 선택입니다. 다시 입력해주세요." << endl;
                    break;
            }
        }
    }
};

int main() {
    AccountManager manager;
    manager.loadAccounts();
    manager.processMenu();

    return 0;
}
