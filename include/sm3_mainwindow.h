#ifndef SM3_MAINWINDOW_H
#define SM3_MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SM3_MainWindow; }
QT_END_NAMESPACE

class SM3_MainWindow : public QMainWindow {
    Q_OBJECT

public:
    SM3_MainWindow(QWidget *parent = nullptr);
    ~SM3_MainWindow();
    void update_systable(std::string new_row[], int item_num);
public slots:
    void slot_add_newfile();
    void slot_double_clicked(int row, int col);

private slots:
    void on_SM3_MainWindow_customContextMenuRequested(const QPoint &pos);

private:
    Ui::SM3_MainWindow *ui;

};
#endif // SM3_MAINWINDOW_H
