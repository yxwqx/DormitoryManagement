#ifndef DORMITORYMANAGER_H
#define DORMITORYMANAGER_H

#include <mysql.h>
#include <iostream>
#include <string>
#include <vector>
#include <QString>

using namespace std;

struct Dormitory {
    string dormitory_id;
    string building;
    string floor;
    string dorm_number;
    string type;
    string bed_id;
    string student_id;
    string student_name;
};

class DormitoryManager {
public:
    static DormitoryManager* getInstance() {
        static DormitoryManager instance;
        return &instance;
    }

    bool insert_dormitory(Dormitory& t);
    bool update_dormitory(Dormitory& t);
    bool delete_dormitory(const string& dormitory_id);
    int getLastDormitoryId();
    vector<Dormitory> get_dormitory(string condition = "");
    QString getBedIdInChinese(const string& bed_id);
    bool update_bed_status(const string& building, const string& floor, const string& dormNumber, const string& bedId, const string& status);
    bool clear_student_id(const string& dormitory_id);
private:
    DormitoryManager();
    ~DormitoryManager();

    MYSQL* con;
    const char* host = "127.0.0.1";
    const char* user = "root";
    const char* pw = "0199wbBB";
    const char* database_name = "student_data";
    const int port = 3306;
};

#endif
