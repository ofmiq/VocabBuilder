#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    tempProcessedFilePath = "";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Select File"), "", tr("Text Files (*.txt);;All Files (*)"));

    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("No File Selected"), tr("Please select a valid file."));
        return;
    }
    updateSelectedFile(filePath);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1) {
        const QUrl fileUrl = event->mimeData()->urls().first();
        if (fileUrl.isLocalFile()) {
            event->acceptProposedAction();
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QUrl fileUrl = event->mimeData()->urls().first();
    QString filePath = fileUrl.toLocalFile();

    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Invalid File"), tr("The file could not be read."));
        return;
    }
    updateSelectedFile(filePath);
}

void MainWindow::updateSelectedFile(const QString &filePath)
{
    selectedFilePath = filePath;

    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();
    ui->selectFileButton->setText(fileName);
    ui->statusbar->showMessage(tr("File selected: %1").arg(fileName), 5000);

    FileParser parser(filePath);
    if (!parser.isValid()) {
        QMessageBox::critical(this, tr("Error"), tr("The selected file is not valid."));
        return;
    }

    QStringList words = parser.parseWords();
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString tempFileName = "parsed_words.txt";
    tempProcessedFilePath = tempDir + "/" + tempFileName;

    QFile outputFile(tempProcessedFilePath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save parsed words to file."));
        return;
    }

    QTextStream out(&outputFile);
    for (const QString &word : words) {
        out << word << "\n";
    }
    outputFile.close();

    ui->statusbar->showMessage(
        tr("Parsed words saved to temporary location: %1").arg(tempProcessedFilePath), 5000);
}

void MainWindow::on_downloadProcessedButton_clicked()
{
    if (tempProcessedFilePath.isEmpty()) {
        QMessageBox::warning(this, tr("No File Available"), tr("Please process a file first."));
        return;
    }

    QString saveFilePath = QFileDialog::getSaveFileName(
        this, tr("Save Processed File"), "", tr("Text Files (*.txt);;All Files (*)"));

    if (saveFilePath.isEmpty()) {
        return;
    }

    QFile::copy(tempProcessedFilePath, saveFilePath);
    QMessageBox::information(this, tr("File Saved"), tr("The processed file has been saved to: %1").arg(saveFilePath));
}
