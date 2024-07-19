#include "DormitoryManager.h"
#include <QString>
#include <QStringList>

DormitoryManager::DormitoryManager() {
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
    }
}

DormitoryManager::~DormitoryManager() {
    if (con) {
        mysql_close(con);
    }
}

bool DormitoryManager::insert_dormitory(Dormitory& dorm) {
    QString qBuilding = QString::fromLocal8Bit(dorm.building.c_str());
    QString qFloor = QString::fromLocal8Bit(dorm.floor.c_str());
    QString qDormNumber = QString::fromLocal8Bit(dorm.dorm_number.c_str());
    QString qType = QString::fromLocal8Bit(dorm.type.c_str());
    QString qBedId = QString::fromLocal8Bit(dorm.bed_id.c_str());
    QString qStudentId = QString::fromLocal8Bit(dorm.student_id.c_str());

    char sql[1024];
    snprintf(sql, sizeof(sql),
             "INSERT INTO dormitory (building, floor, dorm_number, type, bed_id, student_id) "
             "VALUES ('%s', '%s', '%s', '%s', '%s', '%s')",
             qBuilding.toLocal8Bit().constData(),
             qFloor.toLocal8Bit().constData(),
             qDormNumber.toLocal8Bit().constData(),
             qType.toLocal8Bit().constData(),
             qBedId.toLocal8Bit().constData(),
             qStudentId.toLocal8Bit().constData());

    if (mysql_query(con, sql)) {
        cerr << "插入宿舍信息失败: " << mysql_error(con) << endl;
        return false;
    }
    return true;
}

bool DormitoryManager::update_dormitory(Dormitory& dorm) {
    QString qBuilding = QString::fromLocal8Bit(dorm.building.c_str());
    QString qFloor = QString::fromLocal8Bit(dorm.floor.c_str());
    QString qDormNumber = QString::fromLocal8Bit(dorm.dorm_number.c_str());
    QString qType = QString::fromLocal8Bit(dorm.type.c_str());
    QString qBedId = QString::fromLocal8Bit(dorm.bed_id.c_str());
    QString qStudentId = QString::fromLocal8Bit(dorm.student_id.c_str());

    char sql[1024];
    snprintf(sql, sizeof(sql),
             "UPDATE dormitory SET building='%s', floor='%s', dorm_number='%s', type='%s', bed_id='%s', student_id='%s' "
             "WHERE dormitory_id='%s'",
             qBuilding.toLocal8Bit().constData(),
             qFloor.toLocal8Bit().constData(),
             qDormNumber.toLocal8Bit().constData(),
             qType.toLocal8Bit().constData(),
             qBedId.toLocal8Bit().constData(),
             qStudentId.toLocal8Bit().constData(),
             dorm.dormitory_id.c_str());

    if (mysql_query(con, sql)) {
        cerr << "更新宿舍信息失败: " << mysql_error(con) << endl;
        return false;
    }
    return true;
}

bool DormitoryManager::delete_dormitory(const string& dormitory_id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "DELETE FROM dormitory WHERE dormitory_id='%s'", dormitory_id.c_str());

    if (mysql_query(con, sql)) {
        cerr << "删除宿舍信息失败: " << mysql_error(con) << endl;
        return false;
    }
    return true;
}

int DormitoryManager::getLastDormitoryId() {
    const char* sql = "SELECT MAX(dormitory_id) FROM dormitory";
    if (mysql_query(con, sql)) {
        cerr << "查询最后一个宿舍ID失败: " << mysql_error(con) << endl;
        return -1;
    }

    MYSQL_RES* result = mysql_store_result(con);
    if (result == NULL) {
        cerr << "存储结果集失败: " << mysql_error(con) << endl;
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int lastId = row[0] ? atoi(row[0]) : 0;
    mysql_free_result(result);
    return lastId;
}

vector<Dormitory> DormitoryManager::get_dormitory(string condition) {
    vector<Dormitory> dormitories;
    string sql = "SELECT d.*, s.name FROM dormitory d LEFT JOIN student s ON d.student_id = s.id";
    if (!condition.empty()) {
        sql += " WHERE " + condition;
    }

    if (mysql_query(con, sql.c_str())) {
        cerr << "查询宿舍信息失败: " << mysql_error(con) << endl;
        return dormitories;
    }

    MYSQL_RES* result = mysql_store_result(con);
    if (result == NULL) {
        cerr << "存储结果集失败: " << mysql_error(con) << endl;
        return dormitories;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        Dormitory dorm;
        dorm.dormitory_id = row[0] ? row[0] : "";
        dorm.building = row[1] ? row[1] : "";
        dorm.floor = row[2] ? row[2] : "";
        dorm.dorm_number = row[3] ? row[3] : "";
        dorm.type = row[4] ? row[4] : "";
        dorm.bed_id = row[5] ? row[5] : "";
        dorm.student_id = row[6] ? row[6] : "";
        dorm.student_name = row[7] ? row[7] : "";
        dormitories.push_back(dorm);
    }
    mysql_free_result(result);
    return dormitories;
}

QString DormitoryManager::getBedIdInChinese(const string& bed_id) {
    int bedNumber = atoi(bed_id.c_str());
    QStringList bedNames = {"一号床", "二号床", "三号床", "四号床"};
    if (bedNumber >= 1 && bedNumber <= bedNames.size()) {
        return bedNames[bedNumber - 1];
    }
    return QString::fromLocal8Bit(bed_id.c_str());
}

bool DormitoryManager::update_bed_status(const string& building, const string& floor, const string& dormNumber, const string& bedId, const string& status) {
    char sql[256];
    snprintf(sql, sizeof(sql),
             "UPDATE dormitory SET status='%s' WHERE building='%s' AND floor='%s' AND dorm_number='%s' AND bed_id='%s'",
             status.c_str(), building.c_str(), floor.c_str(), dormNumber.c_str(), bedId.c_str());

    if (mysql_query(con, sql)) {
        cerr << "更新床位状态失败: " << mysql_error(con) << endl;
        return false;
    }
    return true;
}

bool DormitoryManager::clear_student_id(const string& dormitory_id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "UPDATE dormitory SET student_id = NULL WHERE dormitory_id = '%s'", dormitory_id.c_str());

    if (mysql_query(con, sql)) {
        cerr << "更新宿舍 student_id 失败: " << mysql_error(con) << endl;
        return false;
    }
    return true;
}
