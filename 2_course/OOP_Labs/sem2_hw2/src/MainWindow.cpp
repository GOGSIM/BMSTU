#include "MainWindow.h"
#include "XmlTreeModel.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QTreeView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), tabs_(new QTabWidget(this))
{
    setWindowTitle(tr("ДЗ2 — XML и модель-представление Qt"));
    resize(1100, 720);
    tabs_->setObjectName("documentTabs");
    setCentralWidget(tabs_);
    tabs_->addTab(createView(), tr("Общее дерево"));

    QMenu* fileMenu = menuBar()->addMenu(tr("&Файл"));
    QAction* openAction = fileMenu->addAction(tr("&Открыть…"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setObjectName("openAction");
    QAction* closeAllAction = fileMenu->addAction(tr("&Закрыть все"));
    closeAllAction->setObjectName("closeAllAction");
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction(tr("&Выход"));
    exitAction->setShortcut(QKeySequence::Quit);

    // Сигнал сообщает о действии, слот выполняет нужную операцию.
    // Современная запись connect проверяет типы при компиляции.
    connect(openAction, &QAction::triggered, this, &MainWindow::slotOpen);
    connect(closeAllAction, &QAction::triggered, this, &MainWindow::closeAllFiles);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(tabs_, &QTabWidget::currentChanged, this, &MainWindow::updateStatus);
    updateStatus();
}

QTreeView* MainWindow::createView()
{
    QTreeView* view = new QTreeView;
    // QObject-родитель отвечает за время жизни модели. QTabWidget получает
    // владение view при addTab(), а view удаляет модель вместе с собой.
    view->setModel(new XmlTreeModel(view));
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setAlternatingRowColors(true);
    view->setUniformRowHeights(true);
    view->setColumnWidth(0, 360);
    view->header()->setStretchLastSection(true);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    // Координаты этого сигнала для QTreeView относятся к viewport — области,
    // где рисуются строки, без заголовка и полос прокрутки.
    connect(view, &QWidget::customContextMenuRequested,
            this, &MainWindow::slotContextMenu);
    return view;
}

XmlTreeModel* MainWindow::currentModel() const
{
    QTreeView* view = qobject_cast<QTreeView*>(tabs_->currentWidget());
    return view ? qobject_cast<XmlTreeModel*>(view->model()) : nullptr;
}

void MainWindow::slotOpen()
{
    const QString dataDirectory = QDir(QApplication::applicationDirPath()).filePath("data");
    const QString fileName = QFileDialog::getOpenFileName(
        this, tr("Открыть XML с данными общепита"), dataDirectory,
        tr("XML-файлы (*.xml);;Все файлы (*)"));
    if (!fileName.isEmpty()) {
        openFile(fileName);
    }
}

bool MainWindow::openFile(const QString& fileName)
{
    // Два документа с одинаковым именем из разных каталогов допустимы.
    // Только повторное открытие того же полного пути активирует прежний узел.
    for (int tab = 0; tab < tabs_->count(); ++tab) {
        QTreeView* view = qobject_cast<QTreeView*>(tabs_->widget(tab));
        XmlTreeModel* model = qobject_cast<XmlTreeModel*>(view->model());
        const int row = model->findFile(fileName);
        if (row >= 0) {
            tabs_->setCurrentIndex(tab);
            const QModelIndex index = model->index(row, 0);
            view->setCurrentIndex(index);
            view->scrollTo(index);
            return true;
        }
    }

    XmlTreeModel* model = currentModel();
    if (!model->appendFile(fileName)) {
        QMessageBox::critical(this, tr("Ошибка загрузки XML"), model->lastError());
        return false;
    }
    QTreeView* view = qobject_cast<QTreeView*>(tabs_->currentWidget());
    const QModelIndex index = model->index(model->rowCount() - 1, 0);
    view->expand(index); // Раскрываем файл, но не тысячи полей одновременно.
    view->setCurrentIndex(index);
    view->scrollTo(index);
    updateTabTitle();
    updateStatus();
    return true;
}

void MainWindow::slotContextMenu(const QPoint& position)
{
    // sender() — объект, отправивший сигнал. Все деревья используют один слот.
    QTreeView* view = qobject_cast<QTreeView*>(sender());
    if (!view) {
        return;
    }
    const QModelIndex clicked = view->indexAt(position);
    // Пустой parent у валидного индекса означает корень открытого файла.
    if (!clicked.isValid() || clicked.parent().isValid()) {
        return;
    }
    tabs_->setCurrentWidget(view);
    const int row = clicked.row();
    view->setCurrentIndex(clicked.siblingAtColumn(0));
    QMenu menu(this);
    QAction* closeAction = menu.addAction(tr("Закрыть"));
    QAction* moveAction = menu.addAction(tr("Переместить в новую вкладку"));
    moveAction->setEnabled(currentModel()->rowCount() > 1);
    // exec() ждёт выбора команды. nullptr означает отмену меню.
    QAction* selected = menu.exec(view->viewport()->mapToGlobal(position));
    if (selected == closeAction) {
        closeFile(row);
    } else if (selected == moveAction) {
        moveFileToNewTab(row);
    }
}

bool MainWindow::closeFile(int row)
{
    XmlTreeModel* model = currentModel();
    if (row < 0 || row >= model->rowCount()) {
        return false;
    }
    // removeRow удаляет и строку файла, и все вложенные элементы.
    model->removeRow(row);
    if (model->rowCount() == 0 && tabs_->currentIndex() != 0) {
        QWidget* page = tabs_->currentWidget();
        tabs_->removeTab(tabs_->currentIndex());
        // removeTab сам не уничтожает страницу: её нужно удалить явно.
        delete page;
    }
    updateTabTitle();
    updateStatus();
    return true;
}

bool MainWindow::moveFileToNewTab(int row)
{
    XmlTreeModel* sourceModel = currentModel();
    // Проверка нужна и здесь: недоступность пункта меню не заменяет условие
    // самой операции. Последний файл в исходной вкладке переносить нельзя.
    if (sourceModel->rowCount() <= 1 || row < 0 || row >= sourceModel->rowCount()) {
        return false;
    }
    QTreeView* targetView = createView();
    XmlTreeModel* targetModel = qobject_cast<XmlTreeModel*>(targetView->model());
    // takeRow, в отличие от removeRow, НЕ удаляет элементы. appendRow
    // передаёт их новой модели вместе со всем поддеревом и служебными ролями.
    targetModel->appendRow(sourceModel->takeRow(row));
    updateTabTitle();
    const int tab = tabs_->addTab(targetView, targetModel->item(0)->text());
    tabs_->setCurrentIndex(tab);
    targetView->expand(targetModel->index(0, 0));
    targetView->setCurrentIndex(targetModel->index(0, 0));
    updateTabTitle();
    updateStatus();
    return true;
}

void MainWindow::closeAllFiles()
{
    while (tabs_->count() > 1) {
        QWidget* page = tabs_->widget(tabs_->count() - 1);
        tabs_->removeTab(tabs_->count() - 1);
        delete page;
    }
    tabs_->setCurrentIndex(0);
    XmlTreeModel* model = currentModel();
    // Удаляем строки, сохраняя заголовки столбцов (clear() удалил бы и их).
    model->removeRows(0, model->rowCount());
    updateStatus();
}

void MainWindow::updateTabTitle()
{
    const int current = tabs_->currentIndex();
    if (current > 0) {
        XmlTreeModel* model = currentModel();
        tabs_->setTabText(current, model->rowCount() == 1
            ? model->item(0)->text() : tr("Документы (%1)").arg(model->rowCount()));
    }
}

void MainWindow::updateStatus()
{
    int total = 0;
    for (int tab = 0; tab < tabs_->count(); ++tab) {
        QTreeView* view = qobject_cast<QTreeView*>(tabs_->widget(tab));
        total += view->model()->rowCount();
    }
    statusBar()->showMessage(tr("Открыто файлов: %1. Вкладок: %2. "
                               "Команды документа — правой кнопкой по имени файла.")
                                .arg(total).arg(tabs_->count()));
}

