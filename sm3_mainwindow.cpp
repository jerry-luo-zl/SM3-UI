#include "Types.h"
#include "SM3.h"
#include <iostream>
#include <list>

#include <QFileDialog>
#include <QDir>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QProgressDialog>
#include <QMessageBox>

#include "sm3_mainwindow.h"
#include "ui_sm3.h"



SM3_MainWindow::SM3_MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SM3_MainWindow) {
    ui->setupUi(this);
    // connect select_added_file to select_file_but
    connect(ui->select_file_but, SIGNAL(clicked()), this, SLOT(slot_add_newfile()));

    // demo
    std::string test1[] = {"1", "E:\\test\\", "hello.txt", "abcdefghijklmnopqrstuvwxyzqwertyuio", \
                         "2022.1.6", "2022.1.5", "12KB"};
    update_systable(test1, 7);
    std::string test2[] = {"2", "E:\\test\\", "hello.txt", "abcdefghijklmnopqrstuvwxyzqwertyuio", \
                         "2022.1.6", "2022.1.5", "12KB"};
    update_systable(test2, 7);
    std::string test3[] = {"3", "E:\\test\\", "hello.txt", "abcdefghijklmnopqrstuvwxyzqwertyuio", \
                         "2022.1.6", "2022.1.5", "12KB"};
    update_systable(test3, 7);

    std::string test4[] = {"4", "E:\\test\\", "hello.txt", "abcdefghijklmnopqrstuvwxyzqwertyuio", \
                         "2022.1.6", "2022.1.5", "12KB"};
    update_systable(test4, 7);
    // demo end

    connect(ui->system_table, SIGNAL(cellDoubleClicked(int , int )), \
            this, SLOT(slot_double_clicked(int , int )));

    connect(ui->system_table, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(slot_item_click()));

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

    // update the table


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

    // get item_entire_path
    /* 此处需要宏定义 */
    QString filename = (ui->system_table->item(row, 1))->text();
    QString filepath = (ui->system_table->item(row, 2))->text();
    QString item_integrated_path =filename + filepath;
    QDesktopServices::openUrl(QUrl::fromLocalFile(item_integrated_path));

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

    });

    connect(p_action_calc_hash, &QAction::triggered, this, [=](){
        // recalculate SM3 Hash value
        SM3 SM3Handler;
        std::string hash_value = SM3Handler.Hash(file_full_path.toStdString());
        QProgressDialog process(this);
        process.resize(QSize(4000,2800));
        process.setLabelText(tr("calculating hashvalue..."));
        process.setRange(0,50000);
        process.setModal(true);
        process.setWindowTitle("Calculating");
        //process.setCancelButtonText(tr("cancel"));
        for(int i=0;i<50000;i++) {
            for(int j=0;j<10000;j++) {
                process.setValue(i);
            }
            if(process.wasCanceled())
              break;
        }
        QMessageBox::about(NULL, "Calculate Complete",  ("Hashvalue:  " + hash_value).c_str());

        // store value to the database


    });
    cmenu->exec(QCursor::pos());

}
