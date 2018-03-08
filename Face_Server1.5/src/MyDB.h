#ifndef MYDB_H_INCLUDED
#define MYDB_H_INCLUDED

#include <mysql/mysql.h>
#include <string>
#include <vector>

class MyDB
{
public:
    MyDB();
    int sql_find(const std::string& sql);
    int sql_insert(const std::string& sql);
    int sql_delete(const std::string& sql);
    std::string sql_get(const std::string& sql);
    bool sql_isExisted(const std::string& sql);
    //template<typename T>
    std::vector<std::string> sql_get_all(const std::string& sql);
    ~MyDB();

private:
    MYSQL mysql;
    MYSQL_RES *result;

};

#endif // MYDB_H_INCLUDED
