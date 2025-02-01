#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "FileParser.h"
#include "FileProcessingService.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    fileProcessingService(nullptr)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    tempProcessedFilePath = "";
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileProcessingService;
}

void MainWindow::on_selectFileButton_clicked()
{
    // Open file dialog for file selection
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

    // Create FileParser and FileProcessingService
    FileParser *parser = new FileParser(filePath);
    fileProcessingService = new FileProcessingService(parser);

    try {
        // Process file and get list of <word, definition> pairs
        QList<QPair<QString, QString>> wordDefinitions = fileProcessingService->processFileWithDefinitions();

        // Generate temporary file path for processed results
        QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        tempProcessedFilePath = tempDir + "/parsed_words_with_definitions.txt";

        // Save processed results to temporary file
        fileProcessingService->saveProcessedWordsWithDefinitions(tempProcessedFilePath, wordDefinitions);

        ui->statusbar->showMessage(tr("Parsed words with definitions saved to temporary location: %1")
                                       .arg(tempProcessedFilePath), 5000);
    } catch (const std::exception &ex) {
        QMessageBox::critical(this, tr("Error"), tr("An error occurred: %1").arg(ex.what()));
    }
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

    // Remove existing file if any
    if (QFile::exists(saveFilePath)) {
        QFile::remove(saveFilePath);
    }

    // Copy temporary file to user-specified location
    if (!QFile::copy(tempProcessedFilePath, saveFilePath)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save the processed file."));
        return;
    }

    QMessageBox::information(this, tr("File Saved"), tr("The processed file has been saved to: %1").arg(saveFilePath));
}
