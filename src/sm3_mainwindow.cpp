#include "Types.h"
#include "SM3.h"
#include <iostream>
#include <list>
#include<Windows.h>
#include <sstream>

#include <QtWidgets/QFileDialog>
#include <QtCore/QDir>
#include <QtGui/QStandardItemModel>
#include <QtGui/QDesktopServices>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QMessageBox>

#include "sm3_mainwindow.h"
#include "ui_sm3.h"



SM3_MainWindow::SM3_MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SM3_MainWindow) {
    ui->setupUi(this);

    // 设置按钮信号和槽函数关联
    connect(ui->select_file_but, SIGNAL(clicked()), this, SLOT(slot_add_newfile()));

    // 设置图表项双击信号和槽函数关联
    connect(ui->system_table, SIGNAL(cellDoubleClicked(int , int )), \
            this, SLOT(slot_double_clicked(int , int )));

}

SM3_MainWindow::~SM3_MainWindow() {
    delete ui;
}

void SM3_MainWindow::slot_add_newfile() {

    // open the file browser to select file
    QString current_path =QDir::currentPath();
    QString selected_file_path = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    current_path.toStdString().c_str(),
                                                    tr(""));
    // Calculate SM3 HASH value
    SM3 SM3Handler;
    std::string hash_value = SM3Handler.Hash(selected_file_path.toStdString());
    //std::cout << "SM3哈希值为:" << hash_value << std::endl;

    // add file info and hash value to the database
        // filepath ==>file info, fileinfo + hash ==>database
    std::string *fileinfo = getfileinfo(selected_file_path.toStdString(), hash_value);
    file_record_num += 1;
    // update the table
     update_systable(fileinfo, 7);
}

void SM3_MainWindow::update_systable(std::string new_row[], int item_num) {

    int current_row = ui->system_table->currentRow();
    int row_count = ui->system_table->rowCount();
    int new_row_count = current_row + 1;
    if(current_row == -1){
        new_row_count = row_count;
    }
    ui->system_table->insertRow(new_row_count);

    // set content of new row
    for (int i = 0; i < item_num; i++) {

        QTableWidgetItem *new_item = new QTableWidgetItem(new_row[i].c_str());
        new_item->setFlags(new_item->flags() & (~Qt::ItemIsEditable));
        ui->system_table->setItem(new_row_count, i, new_item);

    }
}

void SM3_MainWindow::slot_double_clicked(int row, int col) {
    QString filename = (ui->system_table->item(row, 1))->text();
    QString filepath = (ui->system_table->item(row, 2))->text();
    QString item_integrated_path =filename + filepath;
    QString old_hash_value = (ui->system_table->item(row, 3))->text();
    // 打开文件
    QDesktopServices::openUrl(QUrl::fromLocalFile(item_integrated_path));
    // 校验文件完整
    SM3 SM3Handler;
    std::string hash_value = SM3Handler.Hash(item_integrated_path.toStdString());
    QProgressDialog process(this);
    process.resize(QSize(4000,2800));
    process.setLabelText(tr("Recomputing hashvalue..."));
    process.setRange(0,50000);
    process.setModal(true);
    process.setWindowTitle("Recomputing");
    process.setCancelButtonText(tr("cancel"));
    for(int i=0;i<50000;i++) {
        for(int j=0;j<20000;j++) {
            process.setValue(i);
        }
        if(process.wasCanceled())
          break;
    }

    int integrity_flag = 0;
    if (old_hash_value.toStdString() == hash_value)
        integrity_flag = 1;
    else
        integrity_flag = 0;

    std::string message = "Old hash value:  " + old_hash_value.toStdString() + "\n\n";
    message += "New hash value:  " + hash_value + "\n\n";
    message+= "File Integrity:  ";
    if (integrity_flag)
        message += "Yes\n";
    else
        message += "No\n";

    QMessageBox::about(NULL, "Recomputing Complete",  message.c_str());
}

void SM3_MainWindow::on_SM3_MainWindow_customContextMenuRequested(const QPoint &pos) {

    QMenu *cmenu = new QMenu(ui->system_table);

    QAction *p_action_calc_hash = new QAction(tr("Recompute hashvalue"), this);
    QAction *p_action_delete = new QAction(tr("Remove file from system"), this);

    cmenu->addAction(p_action_delete);
    cmenu->addAction(p_action_calc_hash);

    // 不能点最后一排??
    QModelIndex index = ui->system_table->indexAt(ui->system_table->mapFromGlobal(QCursor::pos()));\
    QString filename = (ui->system_table->item(index.row(), 2))->text();
    QString filepath = (ui->system_table->item(index.row(), 1))->text();
    QString fileid = (ui->system_table->item(index.row(),0))->text();
    QString file_full_path = filepath + filename;
    //std::cout << file_full_path.toStdString() << std::endl;

    connect(p_action_delete,  &QAction::triggered, this, [=](){
        // delete file from database
            // fileid ==> delete掉
        // delete file from tablewidget
        ui->system_table->removeRow(index.row());
    });

    connect(p_action_calc_hash, &QAction::triggered, this, [=](){
        // recalculate SM3 Hash value
        SM3 SM3Handler;
        std::string hash_value = SM3Handler.Hash(file_full_path.toStdString());
        QProgressDialog process(this);
        process.resize(QSize(4000,2800));
        process.setLabelText(tr("Computing hashvalue..."));
        process.setRange(0,50000);
        process.setModal(true);
        process.setWindowTitle("Computing");
        process.setCancelButtonText(tr("cancel"));
        for(int i=0;i<50000;i++) {
            for(int j=0;j<20000;j++) {
                process.setValue(i);
            }
            if(process.wasCanceled())
              break;
        }
        QMessageBox::about(NULL, "Computing Complete",  ("Hashvalue:  " + hash_value).c_str());
    });
    cmenu->exec(QCursor::pos());
}

std::string* SM3_MainWindow::getfileinfo(std::string fileDir, std::string hashValue) {
        std::string fileID = std::to_string(file_record_num + 1);
        std::string fileName;
        std::string joinTime;
        std::string lastTime;
        std::string fileSize;
        //根据路径获取 文件名
        std::string flag = "/";
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

        fileDir = fileDir.substr(0, position);
        std::string* fileinfo = new std::string[7];
        fileinfo[0] = fileID;
        fileinfo[1] = fileDir;
        fileinfo[2] = fileName;
        fileinfo[3] = hashValue;
        fileinfo[4] = joinTime;
        fileinfo[5] = lastTime;
        fileinfo[6] = fileSize;
        return fileinfo;
}
