#pragma once
#include "SyntaxSettings.h"
#include <QMainWindow>
#include <memory>

class DocumentEditor;
class QTabWidget;
class QStringListModel;
class QListView;
class QLabel;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    // Внедрение стратегии: окно не знает, как устроен XML.
    MainWindow(std::unique_ptr<SyntaxSettingsReader> reader, const QString& settingsPath,
               QWidget* parent = nullptr);
    DocumentEditor* currentEditor() const;
    DocumentEditor* newDocument();
    bool openFile(const QString& path);
    bool saveDocument(DocumentEditor* editor, bool saveAs = false);
    bool saveAll();
    bool closeDocument(int index);
    bool closeAll();
protected:
    void closeEvent(QCloseEvent* event) override;
private:
    void createActions();
    void createDocks();
    void attachEditor(DocumentEditor* editor);
    void updateDocuments();
    void updateCursor();
    void updateActions();
    void applySyntax(DocumentEditor* editor);
    void showError(const QString& title, const QString& detail);
    QTabWidget* tabs_;
    QStringListModel* documents_;
    QListView* documentList_ = nullptr;
    QLabel* cursorLabel_;
    QList<SyntaxDefinition> syntaxes_;
    QList<QAction*> documentActions_;
    QAction* cutAction_ = nullptr;
    QAction* copyAction_ = nullptr;
    int untitledCounter_ = 0;
};
