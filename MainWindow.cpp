#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "FileParser.h"
#include "FileProcessingService.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
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
    // Open file dialog for file selection.
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

    // Create FileParser and FileProcessingService using unique pointers.
    auto parser = std::make_unique<FileParser>(filePath);
    fileProcessingService = std::make_unique<FileProcessingService>(std::move(parser));

    // Retrieve the list of words from the file.
    QStringList words = fileProcessingService->getWords();
    int totalWords = words.size();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(totalWords);
    ui->progressBar->setValue(0);

    // Use QFutureWatcher to fetch definitions concurrently and update the progress bar.
    QFutureWatcher<QString>* watcher = new QFutureWatcher<QString>(this);
    connect(watcher, &QFutureWatcher<QString>::progressValueChanged, this, [this](int progress) {
        ui->progressBar->setValue(progress);
    });
    connect(watcher, &QFutureWatcher<QString>::finished, this, [this, watcher, words]() {
        QStringList definitions = watcher->future().results();
        QList<QPair<QString, QString>> wordDefinitions;
        for (int i = 0; i < words.size(); ++i) {
            wordDefinitions.append(qMakePair(words[i], definitions[i]));
        }
        currentWordDefinitions = wordDefinitions;

        // Generate temporary file path for processed plain text results.
        QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        tempProcessedFilePath = tempDir + "/parsed_words_with_definitions.txt";

        try {
            fileProcessingService->saveProcessedWordsWithDefinitions(tempProcessedFilePath, wordDefinitions);
            ui->statusbar->showMessage(tr("Processed file saved to temporary location: %1")
                                           .arg(tempProcessedFilePath), 5000);
        } catch (const std::exception &ex) {
            QMessageBox::critical(this, tr("Error"), tr("An error occurred: %1").arg(ex.what()));
        }
        watcher->deleteLater();
    });

    // Launch concurrent mapping: for each word, fetch its definition synchronously (with retry mechanism).
    QFuture<QString> future = QtConcurrent::mapped(words, [this](const QString &word) {
        return fileProcessingService->fetchDefinitionSync(word);
    });
    watcher->setFuture(future);
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

    // Remove existing file if any.
    if (QFile::exists(saveFilePath)) {
        QFile::remove(saveFilePath);
    }

    // Copy temporary plain text file to user-specified location.
    if (!QFile::copy(tempProcessedFilePath, saveFilePath)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save the processed file."));
        return;
    }

    QMessageBox::information(this, tr("File Saved"), tr("The processed file has been saved to: %1").arg(saveFilePath));
}

void MainWindow::on_downloadCSVButton_clicked()
{
    if (currentWordDefinitions.isEmpty()) {
        QMessageBox::warning(this, tr("No Data Available"), tr("Please process a file first."));
        return;
    }

    QString saveFilePath = QFileDialog::getSaveFileName(
        this, tr("Save CSV File"), "", tr("CSV Files (*.csv);;All Files (*)"));

    if (saveFilePath.isEmpty()) {
        return;
    }

    if (QFile::exists(saveFilePath)) {
        QFile::remove(saveFilePath);
    }

    try {
        fileProcessingService->saveProcessedWordsToCsv(saveFilePath, currentWordDefinitions);
        QMessageBox::information(this, tr("File Saved"), tr("The CSV file has been saved to: %1").arg(saveFilePath));
    } catch (const std::exception &ex) {
        QMessageBox::critical(this, tr("Error"), tr("An error occurred: %1").arg(ex.what()));
    }
}
