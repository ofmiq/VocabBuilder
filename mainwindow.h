#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>
#include "FileProcessingService.h"
#include "FileParser.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_selectFileButton_clicked();
    void on_downloadProcessedButton_clicked();

private:
    void updateSelectedFile(const QString &filePath);

    Ui::MainWindow *ui;
    QString selectedFilePath;
    QString tempProcessedFilePath;
    FileProcessingService *fileProcessingService;
};

#endif // MAINWINDOW_H
