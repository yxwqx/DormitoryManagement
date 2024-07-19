#include "StudentManager.h"
#include <QString>

StudentManager::StudentManager() {
    con = mysql_init(NULL);
    if (con == NULL) {
        cerr << "mysql_init() failed" << endl;
        exit(1);
    }
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "gbk");
    if (!mysql_real_connect(con, host, user, pw, database_name, port, NULL, 0)) {
        cerr << "连接数据库失败: " << mysql_error(con) << endl;
        mysql_close(con);
        exit(1);
    }
    if (mysql_set_character_set(con, "gbk")) {
        cerr << "设置字符集失败: " << mysql_error(con) << endl;
    } else {
        cerr << "StudentManager字符集设置成功: gbk" << endl;
    }
}

StudentManager::~StudentManager() {
    if (con) {
        mysql_close(con);
    }
}

MYSQL* StudentManager::getMySQLConnection() const {
    return con;
}

bool StudentManager::insert_student(student& stu) {
    QString qName = QString::fromLocal8Bit(stu.name.c_str());
    QString qGender = QString::fromLocal8Bit(stu.gender.c_str());
    QString qBirthdate = QString::fromLocal8Bit(stu.birthdate.c_str());
    QString qGrade = QString::fromLocal8Bit(stu.grade.c_str());
    QString qMajor = QString::fromLocal8Bit(stu.major.c_str());
    QString qClassin = QString::fromLocal8Bit(stu.classin.c_str());
    QString qPhone = QString::fromLocal8Bit(stu.phone.c_str());
    QString qPasswd = QString::fromLocal8Bit(stu.passwd.c_str());

    char sql[1024];
    snprintf(sql, sizeof(sql),
             "INSERT INTO student (id, name, gender, birthdate, grade, major, classin, phone, passwd) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
             stu.id.c_str(), qName.toLocal8Bit().constData(), qGender.toLocal8Bit().constData(), qBirthdate.toLocal8Bit().constData(), qGrade.toLocal8Bit().constData(), qMajor.toLocal8Bit().constData(), qClassin.toLocal8Bit().constData(), qPhone.toLocal8Bit().constData(), qPasswd.toLocal8Bit().constData());

    if (mysql_query(con, sql)) {
        cerr << "插入失败: " << mysql_error(con) << endl;
        return false;
    }
    return true;
}

bool StudentManager::validate_admin_login(const string& admin_id, const string& password) {
    if (admin_id == "123" && password == "123") {
        return true;
    }
    return false;
}

QString StudentManager::getStudentClass(const student& stu) {
    return QString::fromLocal8Bit(stu.grade.c_str()) + QString::fromLocal8Bit(stu.major.c_str()) + QString::fromLocal8Bit(stu.classin.c_str());
}

bool StudentManager::validate_student_login(const string& student_id, const string& password) {
    string query = "SELECT * FROM student WHERE id = '" + student_id + "' AND passwd = '" + password + "'";
    if (mysql_query(con, query.c_str())) {
        cerr << "Query failed: " << mysql_error(con) << endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(con);
    if (result == NULL) {
        cerr << "Store result failed: " << mysql_error(con) << endl;
        return false;
    }

    bool isValid = mysql_num_rows(result) > 0;
    mysql_free_result(result);
    return isValid;
}

vector<student> StudentManager::get_student(const string& condition) {
    vector<student> students;
    string query = "SELECT student.id, student.name, student.gender, student.birthdate, student.grade, student.major, student.classin, student.phone, student.passwd, "
                   "IFNULL(CONCAT(dormitory.building, '号楼', dormitory.dorm_number, CASE WHEN dormitory.bed_id = '1' THEN '一床' WHEN dormitory.bed_id = '2' THEN '二床' "
                   "WHEN dormitory.bed_id = '3' THEN '三床' WHEN dormitory.bed_id = '4' THEN '四床' ELSE '' END), '未入住') AS dormitory "
                   "FROM student LEFT JOIN dormitory ON student.id = dormitory.student_id";
    if (!condition.empty()) {
        query += " WHERE " + condition;
    }
    if (mysql_query(con, query.c_str())) {
        cerr << "查询学生信息失败: " << mysql_error(con) << endl;
        return students;
    }
    MYSQL_RES *result = mysql_store_result(con);
    if (result == NULL) {
        cerr << "存储结果失败: " << mysql_error(con) << endl;
        return students;
    }
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        student stu;
        stu.id = row[0];
        stu.name = row[1];
        stu.gender = row[2];
        stu.birthdate = row[3];
        stu.grade = row[4];
        stu.major = row[5];
        stu.classin = row[6];
        stu.phone = row[7];
        stu.passwd = row[8];
        stu.dormitory = row[9];
        students.push_back(stu);
    }
    mysql_free_result(result);
    return students;
}

bool StudentManager::update_student(student& stu) {
    QString qName = QString::fromLocal8Bit(stu.name.c_str());
    QString qGender = QString::fromLocal8Bit(stu.gender.c_str());
    QString qBirthdate = QString::fromLocal8Bit(stu.birthdate.c_str());
    QString qGrade = QString::fromLocal8Bit(stu.grade.c_str());
    QString qMajor = QString::fromLocal8Bit(stu.major.c_str());
    QString qClassin = QString::fromLocal8Bit(stu.classin.c_str());
    QString qPhone = QString::fromLocal8Bit(stu.phone.c_str());
    QString qPasswd = QString::fromLocal8Bit(stu.passwd.c_str());

    char sql[1024];
    snprintf(sql, sizeof(sql),
             "UPDATE student SET name='%s', gender='%s', birthdate='%s', grade='%s', major='%s', classin='%s', phone='%s', passwd='%s' WHERE id='%s'",
             qName.toLocal8Bit().constData(), qGender.toLocal8Bit().constData(), qBirthdate.toLocal8Bit().constData(), qGrade.toLocal8Bit().constData(), qMajor.toLocal8Bit().constData(), qClassin.toLocal8Bit().constData(), qPhone.toLocal8Bit().constData(), qPasswd.toLocal8Bit().constData(), stu.id.c_str());

    if (mysql_query(con, sql)) {
        cerr << "更新学生信息失败: " << mysql_error(con) << endl;
        return false;
    }
    return true;
}

bool StudentManager::delete_student(const string& student_id) {
    string query = "DELETE FROM student WHERE id='" + student_id + "'";
    if (mysql_query(con, query.c_str())) {
        cerr << "删除学生信息失败: " << mysql_error(con) << endl;
        return false;
    }
    return true;
}
