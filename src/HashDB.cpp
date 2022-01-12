#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable:4996)
#include <mysql.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include<Windows.h>
#include <fstream>
#include <io.h>
#include <time.h>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include "SM3.h"
#include "Types.h"
#define LINK "localhost"
#define USRNAME "root"
#define PASSWD "root"
#define DBNAME "hashdb"
#define TABLE "table1"
#define PORT 3306

class HashDB {
public:
    MYSQL mysql;
    HashDB() {//构造函数中登录MySQL：mysql_init()，mysql_real_connect()
        try {
            if (!mysql_init(&mysql)) {
                throw std::string("MySQL init is error!");
            }
            if (!mysql_real_connect(&mysql, LINK, USRNAME, PASSWD, DBNAME, PORT, NULL, 0)) {
                throw std::string("MySQL connect is error!");
            }

        }
        catch (std::string& error_msg) {

            std::cout << error_msg << std::endl;
        }
        catch (...) {

            std::cout << "MySQL operation is error!" << std::endl;
        }

    }
    ~HashDB() {//析构函数中断开MySQL连接
        mysql_close(&mysql);
    }

    //接口 1 ： 传入:一个数据库条目的各个字段值，将此条目存入数据库，接口4调用接口1，
    //         返回:bool
    bool setFileHash(std::string fileID, std::string fileDir, std::string fileName,
        std::string hashValue, std::string joinTime, std::string lastTime, std::string fileSize);

    //接口 2 ： 传入文件路径(数据库中文件的绝对路径 唯一) 
    //         返回:哈希值string
    std::string getFileHashbyDir(std::string fileDir);

    //接口 3 ： 传入文件id(数据库中文件id 唯一) 
    //         返回:哈希值string
    std::string getFileHashbyID(std::string fileID);

    //接口 4 ： 传入:调用者分配的文件id、文件路径、文件哈希值，
    //         根据路径解析出文件属性信息，存入数据库条目,只有文件id 和 文件路径均唯一才能存入数据库
    //         返回:bool
    bool fileInfoToDB(std::string fileID, std::string fileDir, std::string hashValue);

    //接口 5 ：得到当前数据库中所有信息
    //         列出当前数据库中所有的条目存入一个二维的vector，每一个元素单元为string
    //         返回:二维的vector
    std::vector<std::vector<std::string>> getAll();

    //接口 6 ：由于MySQL中反斜杠自动转义，因此将路径字符串增加一个反斜杠转义
    std::string addSlash(std::string path) {
        std::string::size_type pos = 0;
        while ((pos = path.find('\\', pos)) != std::string::npos) {
            path.insert(pos, "\\");//插入一个反斜杠 
            pos = pos + 2;
        }
        return path;
    }

    //接口 7 ： 传入文件id(数据库中文件id 唯一) 
    //         返回:哈希值string
    void deleteFileHashbyID(std::string fileID);
};
//接口 7 ： 传入文件id(数据库中文件id 唯一) ,删除对应的数据库条目
//DELETE FROM table1 WHERE 文件id = '400';
void HashDB::deleteFileHashbyID(std::string fileID) {
    mysql_query(&mysql, "SET NAMES GBK"); //设置编码格式
    std::string cmd = "DELETE FROM ";
    cmd.append(TABLE);
    cmd.append(" where 文件id = '");
    cmd.append(fileID);
    cmd.append("';");
    if (mysql_query(&mysql, cmd.c_str())) {

        std::string err_string = mysql_error(&mysql);
        if (err_string.empty())
        {
            throw std::string("MySQL query is error!");
        }
        else
        {
            throw err_string;
        }
    }
    MYSQL_RES* result = mysql_store_result(&mysql);
    if (!result)
    {
        //throw std::string("MySQL not result!");
    }
    //MYSQL_ROW row = mysql_fetch_row(result); //获取整条数据内容
    //std::string temp = row[0];
    mysql_free_result(result);
    //return temp;
}

// eg：INSERT INTO `hashdb`.`table1` (`文件id`, `文件路径`, `文件名`, `校验的哈希值`, `加入系统时间`, `上次修改时间`, `文件大小`) 
//     VALUES ('002', 'c', '文件2', 'asd', '123', '567', '12');
//接口 1 ： 传入:一个数据库条目的各个字段值，将此条目存入数据库，接口4调用接口1，
//         返回:bool
bool HashDB::setFileHash(std::string fileID, std::string fileDir, std::string fileName,
    std::string hashValue, std::string joinTime, std::string lastTime, std::string fileSize) {

    mysql_query(&mysql, "SET NAMES GBK"); //设置编码格式
    std::ostringstream buffer;
    buffer << "INSERT INTO `" << DBNAME << "`.`" << TABLE << "` " <<
        "(`文件id`, `文件路径`, `文件名`, `校验的哈希值`, `加入系统时间`, `上次修改时间`, `文件大小`)"
        << " VALUES ('" << fileID << "', '" << fileDir << "', '"
        << fileName << "', '" << hashValue << "', '" << joinTime << "', '" << lastTime << "', '" << fileSize << "');";
    std::string cmd = buffer.str();
    if (mysql_query(&mysql, cmd.c_str())) {//失败返回 1 
        std::cout << "当前条目插入失败！请确保文件id和文件路径的唯一性" << std::endl;
        return false;
    }
    return true;
}

// eg：select 校验的哈希值 from table1 where 文件路径 = 'xxx'
//接口 2 ： 传入文件路径(数据库中文件的绝对路径 唯一) 
//         返回:哈希值string
std::string HashDB::getFileHashbyDir(std::string fileDir) {
    fileDir = addSlash(fileDir);
    mysql_query(&mysql, "SET NAMES GBK"); //设置编码格式
    std::string cmd = "select 校验的哈希值 from ";
    cmd.append(TABLE);
    cmd.append(" where 文件路径 = '");
    cmd.append(fileDir);
    cmd.append("';");
    if (mysql_query(&mysql, cmd.c_str())) {

        std::string err_string = mysql_error(&mysql);
        if (err_string.empty())
        {
            throw std::string("MySQL query is error!");
        }
        else
        {
            throw err_string;
        }
    }
    MYSQL_RES* result = mysql_store_result(&mysql);
    if (!result)
    {
        throw std::string("MySQL not result!");
    }
    MYSQL_ROW row = mysql_fetch_row(result); //获取整条数据内容
    //std::cout << row[0] << std::endl;
    std::string temp = row[0];
    mysql_free_result(result);
    return temp;

}

// eg：select 校验的哈希值 from table1 where 文件id = 'xxx'
//接口 3 ： 传入文件id(数据库中文件id 唯一) 
//         返回:哈希值string
std::string HashDB::getFileHashbyID(std::string fileID) {
    mysql_query(&mysql, "SET NAMES GBK"); //设置编码格式
    std::string cmd = "select 校验的哈希值 from ";
    cmd.append(TABLE);
    cmd.append(" where 文件id = '");
    cmd.append(fileID);
    cmd.append("';");
    if (mysql_query(&mysql, cmd.c_str())) {

        std::string err_string = mysql_error(&mysql);
        if (err_string.empty())
        {
            throw std::string("MySQL query is error!");
        }
        else
        {
            throw err_string;
        }
    }
    MYSQL_RES* result = mysql_store_result(&mysql);
    if (!result)
    {
        throw std::string("MySQL not result!");
    }
    MYSQL_ROW row = mysql_fetch_row(result); //获取整条数据内容
    std::string temp = row[0];
    mysql_free_result(result);
    return temp;
}
//接口 4 ： 传入:调用者分配的文件id、文件路径、文件哈希值，
//         根据路径解析出文件属性信息，存入数据库条目,只有文件id 和 文件路径均唯一才能存入数据库
//         返回:bool
bool HashDB::fileInfoToDB(std::string fileID, std::string fileDir, std::string hashValue) {
    std::string fileName;
    std::string joinTime;
    std::string lastTime;
    std::string fileSize;
    //根据路径获取 文件名
    std::string flag = "\\";
    std::string::size_type position = fileDir.rfind(flag);
    fileName = fileDir.substr(++position);
    //获取当前时间作为加入系统时间
    char* pszCurrTime = (char*)malloc(sizeof(char) * 20);
    memset(pszCurrTime, 0, sizeof(char) * 20);
    time_t now;
    time(&now);
    strftime(pszCurrTime, 20, "%Y/%m/%d %H:%M:%S", localtime(&now));
    joinTime = pszCurrTime;
    free(pszCurrTime);
    //根据路径获取 文件上一次修改时间
    TCHAR szWriteTime[30];
    HANDLE hFile = CreateFileA(fileDir.c_str(), 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    LPSTR lpszLastWriteTime = (LPSTR)szWriteTime;
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC1, stLocal1;
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
        std::cout << "error!" << std::endl;
    }
    FileTimeToSystemTime(&ftWrite, &stUTC1);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC1, &stLocal1);
    // printf("%02d/%02d/%02d %02d:%02d:%02d\n", 
    //     stLocal1.wYear, stLocal1.wMonth, stLocal1.wDay,
    //     stLocal1.wHour, stLocal1.wMinute, stLocal1.wSecond);
    wsprintfA(lpszLastWriteTime, "%02d/%02d/%02d %02d:%02d:%02d",
        stLocal1.wYear, stLocal1.wMonth, stLocal1.wDay,
        stLocal1.wHour, stLocal1.wMinute, stLocal1.wSecond);
    lastTime = ((char*)szWriteTime);
    //根据路径获取 文件大小 
    std::ifstream in(fileDir.c_str());
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    in.close();
    fileSize = std::to_string(size);
    fileDir = addSlash(fileDir);//增加一个反斜杠转义
    // std::cout << "正在将如下信息加入数据库:" << std::endl;
    // std::cout << fileID << std::endl;
    // std::cout << fileDir << std::endl;
    // std::cout << fileName << std::endl;
    // std::cout << hashValue << std::endl;
    // std::cout << joinTime << std::endl;
    // std::cout << lastTime << std::endl;
    // std::cout << fileSize << std::endl;
    if (setFileHash(fileID, fileDir, fileName, hashValue,
        joinTime, lastTime, fileSize)) {
        //std::cout << "加入成功!" << std::endl;
        return true;
    }
    return false;
}
//接口 5 ：得到当前数据库中所有信息
//         列出当前数据库中所有的条目存入一个二维的vector，每一个元素单元为string
//         返回:二维的vector
std::vector<std::vector<std::string>> HashDB::getAll() {
    mysql_query(&mysql, "SET NAMES GBK"); //设置编码格式
    std::vector<std::vector<std::string>> hashDatabase;
    try
    {
        //MySQL命令
        std::string sql = "SELECT * from ";
        sql.append(TABLE);
        if (mysql_query(&mysql, sql.c_str()))
        {
            std::string err_string = mysql_error(&mysql);
            if (err_string.empty())
            {
                throw std::string("MySQL query is error!");
            }
            else
            {
                throw err_string;
            }
        }
        //std::cout << "MySQL语句 : " << sql << std::endl;
        MYSQL_RES* result = mysql_store_result(&mysql);
        if (!result)
        {
            throw std::string("MySQL not result!");
        }
        //std::cout << std::setiosflags(std::ios::left) << std::setw(20);

        //获取字段数量
        int num_fields = mysql_num_fields(result);
        if (0 == num_fields)
        {
            throw std::string("MySQL fields number is 0!");
        }
        //获取字段名
        MYSQL_FIELD* fields = mysql_fetch_fields(result);
        if (!fields)
        {
            throw std::string("MySQL fields fetch is error!");
        }

        for (int i = 0; i < num_fields; i++)
        {
            //std::cout << fields[i].name << "\t";
            //std::cout << std::setiosflags(std::ios::left) << std::setw(20) << fields[i].name;
        }
        std::cout << std::endl;
        //数据库二维vector的行和列标记
        int rowDB = 0, colDB = 0;
        while (MYSQL_ROW row = mysql_fetch_row(result)) //获取整条数据内容
        {
            hashDatabase.push_back(std::vector<std::string>());//加入新的一行
            for (int i = 0; i < num_fields; i++)
            {

                if (NULL == row[i])
                {
                    //std::cout << std::setiosflags(std::ios::left) << std::setw(20) << "NULL";
                    hashDatabase[rowDB].push_back("NULL");//将本行的第i列push进vertor
                }
                else
                {
                    //std::cout << std::setiosflags(std::ios::left) << std::setw(20) << row[i];
                    hashDatabase[rowDB].push_back(row[i]);//将本行的第i列push进vertor
                }
            }
            //std::cout << std::endl;
            ++rowDB;
        }

        mysql_free_result(result);

        // std::cout << "MySQL is OK." << std::endl;
        // for (int i = 0; i < hashDatabase.size(); i++)
        // {
        //     for (int j = 0; j < hashDatabase[i].size(); j++)
        //     {
        //         std::cout << hashDatabase[i][j] << " ";//使用下标访问 
        //     }
        //     std::cout << std::endl;
        // }
        return hashDatabase;
    }
    catch (std::string& error_msg)
    {
        std::cout << error_msg << std::endl;
    }
    catch (...)
    {
        std::cout << "MySQL operation is error!" << std::endl;
    }

}

//main 函数仅用于测试

// int main() {
//     HashDB SM3DB;
//     SM3 sm3;
//     //E:\\VSCODE\\C++\\SM3Hash\\fileInfo.txt
//     SM3DB.fileInfoToDB("2019", "E:\\VSCODE\\TestHash.txt", sm3.Hash("E:\\VSCODE\\TestHash.txt"));
//     SM3DB.deleteFileHashbyID("300");
//     // SM3DB.setFileHash("1000", "e", "文件3", "xyz", "231", "wer", "eee");      //设置一个已经存在的条目
//     // std::string str1 = SM3DB.getFileHashbyDir("E:\\VSCODE\\C++\\SM3Hash\\HashDB\\fileInfo.txt");//得到 对应路径 的哈希值字符串
//     // std::cout << str1 << std::endl;
//     // std::string str2 = SM3DB.getFileHashbyID("300");//得到 对应文件id 的哈希值字符串
//     // std::cout << str2 << std::endl;
//     // std::vector<std::vector<std::string>> DataBaseInfo = SM3DB.getAll();                         //打印整张数据表的内容
//     // SM3DB.fileInfoToDB("1", "E:\\VSCODE\\C++\\SM3Hash\\HashDB\\fileInfo.txt", "333");
//     return 0;
// }
