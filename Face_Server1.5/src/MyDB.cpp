#include "MyDB.h"
#include <iostream>
#include <assert.h>

using namespace std;

MyDB::MyDB()
{
    mysql_init( &mysql );
    mysql_real_connect(
      &mysql,"localhost","root","llw123","face",3306,NULL,0);
    result = NULL;

}

MyDB::~MyDB()
{
    mysql_free_result(result);
    mysql_close( &mysql );
}

int MyDB::sql_find(const string& sql)
{
    mysql_query( &mysql, sql.c_str() );
    result = mysql_store_result( &mysql );
    assert(NULL!=result);

    int row_count = mysql_num_rows( result );
    cout << "all data number: " << row_count << endl;

    int field_count = mysql_num_fields( result );
    cout << "field count : " << field_count << endl;


    MYSQL_FIELD* field = NULL;
    for( int i = 0; i < field_count; ++i)
    {
        field = mysql_fetch_field_direct( result, i );
        cout << field->name << "\t";
    }
    cout << endl;


    MYSQL_ROW row = NULL;
    row = mysql_fetch_row( result );
    while ( NULL != row )
    {
        for( int i = 0; i < field_count; ++i)
        {
            cout << row[i] << "\t";
        }
        cout << endl;
        row = mysql_fetch_row( result );
    }
    return 0;
}

int MyDB::sql_insert(const string& sql)
{
    mysql_query( &mysql, sql.c_str() );
    return 0;
}

int MyDB::sql_delete(const string& sql)
{
    mysql_query( &mysql, sql.c_str() );
    return 0;
}

string MyDB::sql_get(const string& sql)
{
    mysql_query( &mysql, sql.c_str() );
    result = mysql_store_result( &mysql );
    assert(NULL!=result);

    int field_count = mysql_num_fields( result );
    cout << "field count : " << field_count << endl;

    MYSQL_ROW row = NULL;
    row = mysql_fetch_row( result );

    assert(1==field_count);
    string res=row[0];

    return res;
}

bool MyDB::sql_isExisted(const string& sql)
{
    mysql_query(&mysql, sql.c_str());
    result = mysql_store_result( &mysql );
    if(NULL==result)
        return false;
    else
        return true;
}

//template<typename T>
vector<string> MyDB::sql_get_all(const string& sql)
{
    mysql_query( &mysql, sql.c_str() );
    result = mysql_store_result( &mysql );
    assert(NULL!=result);

    vector<string> res;

    int row_count = mysql_num_rows( result );
    cout << "all data number: " << row_count << endl;

    int field_count = mysql_num_fields( result );
    cout << "field count : " << field_count << endl;

    MYSQL_ROW row = NULL;
    row = mysql_fetch_row( result );
    while ( NULL != row )
    {
        res.push_back(row[0]);
        row = mysql_fetch_row( result );
    }
    return res;
}
