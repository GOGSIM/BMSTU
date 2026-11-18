#pragma once

#include <QMainWindow>

class QTabWidget;
class QTreeView;
class XmlTreeModel;

// Окно связывает действия пользователя с моделью. Сам XML здесь не разбирается.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    bool openFile(const QString& fileName);
    XmlTreeModel* currentModel() const;

public slots:
    // Эти операции используются и меню, и проверками приложения.
    bool closeFile(int row);
    bool moveFileToNewTab(int row);
    void closeAllFiles();

private slots:
    void slotOpen();
    void slotContextMenu(const QPoint& position);

private:
    QTreeView* createView();
    void updateTabTitle();
    void updateStatus();
    QTabWidget* tabs_;
};

