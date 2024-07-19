#ifndef STUDENTMANAGER_H
#define STUDENTMANAGER_H

#include <mysql.h>
#include <iostream>
#include <string>
#include <vector>
#include <QString>

using namespace std;

struct student {
    string id;
    string name;
    string gender;
    string birthdate;
    string grade;
    string major;
    string classin;
    string phone;
    string passwd;
    string dormitory;
    string bed_id;
};

class StudentManager {
public:
    static StudentManager* getinstance() {
        static StudentManager instance;
        return &instance;
    }

    bool insert_student(student& t);
    bool update_student(student& t);
    bool delete_student(const string& student_id);
    vector<student> get_student(const string& condition = "1=1");
    QString getStudentClass(const student& stu);
    bool validate_student_login(const string& student_id, const string& password);
    bool validate_admin_login(const string& admin_id, const string& password);
    bool assign_dormitory_to_student(const student& stu);
    MYSQL* getMySQLConnection() const;

private:
    StudentManager();
    ~StudentManager();
    MYSQL* con;
    const char* host = "127.0.0.1";
    const char* user = "root";
    const char* pw = "0199wbBB";
    const char* database_name = "student_data";
    const int port = 3306;
};

#endif // STUDENTMANAGER_H
