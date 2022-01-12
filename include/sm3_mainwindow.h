#ifndef SM3_MAINWINDOW_H
#define SM3_MAINWINDOW_H

#include <QtWidgets/QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SM3_MainWindow; }
QT_END_NAMESPACE

class SM3_MainWindow : public QMainWindow {
    Q_OBJECT

public:
    SM3_MainWindow(QWidget *parent = nullptr);
    ~SM3_MainWindow();
    // 向系统信息表中插入新行
    void update_systable(std::string new_row[], int item_num);
    // 解析指定路径的文件，获取文件各个属性信息
    std::string* getfileinfo(std::string fileDir, std::string hashValue);

    // 槽函数
public slots:
    void slot_add_newfile(); // 选择文件按钮的槽函数
    void slot_double_clicked(int row, int col); // 表元素被双击时的槽函数

private slots:
    // 右击表元素打开菜单的槽函数
    void on_SM3_MainWindow_customContextMenuRequested(const QPoint &pos);

private:
    Ui::SM3_MainWindow *ui;
    // 系统中的记录数目
    int file_record_num = 0;

};
#endif // SM3_MAINWINDOW_H
