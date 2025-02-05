#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>
#include "FileProcessingService.h"
#include "FileParser.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief The MainWindow class
 * Main window for the VocabBuilder application.
 * Responsible for user interactions, file selection, progress display, and saving results.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent The parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~MainWindow();

protected:
    /**
     * @brief Handles drag enter events.
     * Accepts the event if a single local file is dragged.
     * @param event The drag enter event.
     */
    void dragEnterEvent(QDragEnterEvent *event) override;

    /**
     * @brief Handles drop events.
     * Extracts the file path and updates the UI.
     * @param event The drop event.
     */
    void dropEvent(QDropEvent *event) override;

private slots:
    /**
     * @brief Slot for the "Select File" button click.
     * Opens a file dialog for file selection.
     */
    void on_selectFileButton_clicked();

    /**
     * @brief Slot for the "Download Processed File" button click.
     * Allows the user to save the processed file in plain text format.
     */
    void on_downloadProcessedButton_clicked();

    /**
     * @brief Slot for the "Download CSV File" button click.
     * Allows the user to save the processed file in CSV format.
     */
    void on_downloadCSVButton_clicked();

private:
    /**
     * @brief Updates the selected file, processes it asynchronously, and updates the progress bar.
     * @param filePath The path of the selected file.
     */
    void updateSelectedFile(const QString &filePath);

    Ui::MainWindow *ui;  ///< Pointer to the UI.
    QString selectedFilePath;             ///< Path to the selected file.
    QString tempProcessedFilePath;        ///< Path to the temporary processed plain text file.
    QList<QPair<QString, QString>> currentWordDefinitions; ///< Cached word-definition pairs.
    std::unique_ptr<FileProcessingService> fileProcessingService; ///< Service for processing files.
};

#endif // MAINWINDOW_H
