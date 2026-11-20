#include "MainWindow.h"
#include "DocumentEditor.h"
#include "SyntaxHighlighter.h"
#include "UnsavedDocumentsDialog.h"
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QLabel>
#include <QListView>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QStringListModel>
#include <QTabWidget>
#include <QTextBlock>
#include <QTimer>
#include <QToolBar>
#include <QTreeView>

namespace {
QString normalizedPath(const QString& path)
{
    const QFileInfo info(path);
    QString result = info.canonicalFilePath();
    if (result.isEmpty()) result = info.absoluteFilePath();
    result = QDir::cleanPath(result);
#ifdef Q_OS_WIN
    result = result.toCaseFolded();
#endif
    return result;
}
}

MainWindow::MainWindow(QWidget* parent)
    : MainWindow(std::make_unique<XmlSyntaxSettingsReader>(),
                 QDir(QApplication::applicationDirPath()).filePath("settings/syntaxes.xml"), parent) {}

MainWindow::MainWindow(std::unique_ptr<SyntaxSettingsReader> reader,
                       const QString& path, QWidget* parent)
    : QMainWindow(parent), tabs_(new QTabWidget(this)),
      documents_(new QStringListModel(this)), cursorLabel_(new QLabel(this))
{
    setWindowTitle(tr("ЛР 3–4 — Текстовый редактор"));
    resize(1100, 740);
    tabs_->setObjectName("documentTabs");
    tabs_->setTabsClosable(true);
    setCentralWidget(tabs_);
    statusBar()->addPermanentWidget(cursorLabel_);
    QString error;
    if (!reader || !reader->read(path, syntaxes_, error)) {
        syntaxes_.clear();
        // Сообщение после показа окна, когда цикл событий уже запущен.
        QTimer::singleShot(0, this, [this, error] {
            showError(tr("Ошибка настроек подсветки"), error + tr("\nПодсветка отключена."));
        });
    }
    createActions();
    createDocks();
    connect(tabs_, &QTabWidget::tabCloseRequested, this, &MainWindow::closeDocument);
    connect(tabs_, &QTabWidget::currentChanged, this, [this] {
        updateDocuments(); updateCursor(); updateActions();
    });
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::updateActions);
    newDocument();
}

void MainWindow::createActions()
{
    auto* file = menuBar()->addMenu(tr("&Файл"));
    auto* toolbar = addToolBar(tr("Файл"));
    toolbar->setObjectName("fileToolBar");
    auto add = [this](QMenu* menu, const QString& text, const QString& name,
                      const QKeySequence& key, auto callback) {
        auto* action = menu->addAction(text);
        action->setObjectName(name);
        action->setShortcut(key);
        action->setStatusTip(text);
        connect(action, &QAction::triggered, this, callback);
        return action;
    };
    auto* create = add(file, tr("&Новый"), "newAction", QKeySequence::New,
                       [this] { newDocument(); });
    auto* open = add(file, tr("&Открыть…"), "openAction", QKeySequence::Open, [this] {
        const QString path = QFileDialog::getOpenFileName(this, tr("Открыть документ"),
                                                          QDir::homePath(), tr("Все файлы (*)"));
        if (!path.isEmpty()) openFile(path);
    });
    auto* save = add(file, tr("&Сохранить"), "saveAction", QKeySequence::Save,
                     [this] { saveDocument(currentEditor()); });
    auto* saveAs = add(file, tr("Сохранить &как…"), "saveAsAction", QKeySequence::SaveAs,
                       [this] { saveDocument(currentEditor(), true); });
    auto* saveAllAction = add(file, tr("Сохранить все"), "saveAllAction", QKeySequence("Ctrl+Alt+S"),
                              [this] { saveAll(); });
    file->addSeparator();
    auto* close = add(file, tr("Закрыть"), "closeAction", QKeySequence::Close,
                      [this] { closeDocument(tabs_->currentIndex()); });
    auto* closeAllAction = add(file, tr("Закрыть все"), "closeAllAction", QKeySequence("Ctrl+Shift+W"),
                               [this] { closeAll(); });
    file->addSeparator();
    add(file, tr("Выход"), "exitAction", QKeySequence("Ctrl+Q"), [this] { this->close(); });
    documentActions_ = {save, saveAs, saveAllAction, close, closeAllAction};
    for (auto* action : {create, open, save, saveAllAction}) {
        action->setIcon(QIcon(":/icons/" + action->objectName() + ".png"));
        toolbar->addAction(action);
    }
    auto* edit = menuBar()->addMenu(tr("&Правка"));
    cutAction_ = add(edit, tr("Вырезать"), "cutAction", QKeySequence::Cut,
                     [this] { if (auto* e = currentEditor()) e->cut(); });
    copyAction_ = add(edit, tr("Копировать"), "copyAction", QKeySequence::Copy,
                      [this] { if (auto* e = currentEditor()) e->copy(); });
    auto* paste = add(edit, tr("Вставить"), "pasteAction", QKeySequence::Paste,
                      [this] { if (auto* e = currentEditor()) e->paste(); });
    auto* select = add(edit, tr("Выделить всё"), "selectAllAction", QKeySequence::SelectAll,
                       [this] { if (auto* e = currentEditor()) e->selectAll(); });
    documentActions_.append({paste, select});
}

void MainWindow::createDocks()
{
    auto* view = menuBar()->addMenu(tr("&Вид"));
    auto* explorer = new QDockWidget(tr("Проводник"), this);
    explorer->setObjectName("explorerDock");
    auto* tree = new QTreeView(explorer);
    auto* model = new QFileSystemModel(tree);
    model->setReadOnly(true);
    model->setRootPath(QDir::rootPath());
    tree->setModel(model);
    tree->setCurrentIndex(model->index(QDir::homePath()));
    tree->expand(model->index(QDir::homePath()));
    tree->scrollTo(model->index(QDir::homePath()));
    for (int column = 1; column < model->columnCount(); ++column) tree->hideColumn(column);
    tree->setHeaderHidden(true);
    explorer->setWidget(tree);
    addDockWidget(Qt::LeftDockWidgetArea, explorer);
    connect(tree, &QTreeView::clicked, this, [this, model](const QModelIndex& index) {
        if (!model->isDir(index)) openFile(model->filePath(index));
    });
    auto* documentsDock = new QDockWidget(tr("Открытые документы"), this);
    documentsDock->setObjectName("documentsDock");
    documentList_ = new QListView(documentsDock);
    documentList_->setObjectName("documentList");
    documentList_->setModel(documents_);
    documentList_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    documentsDock->setWidget(documentList_);
    addDockWidget(Qt::LeftDockWidgetArea, documentsDock);
    connect(documentList_, &QListView::clicked, this, [this](const QModelIndex& index) {
        tabs_->setCurrentIndex(index.row());
    });
    auto* explorerAction = explorer->toggleViewAction();
    explorerAction->setText(tr("Показать проводник"));
    explorerAction->setShortcut(QKeySequence("Ctrl+Shift+E"));
    auto* documentsAction = documentsDock->toggleViewAction();
    documentsAction->setText(tr("Показать обозреватель открытых документов"));
    documentsAction->setShortcut(QKeySequence("Ctrl+Shift+D"));
    view->addAction(explorerAction);
    view->addAction(documentsAction);
}

DocumentEditor* MainWindow::currentEditor() const
{
    return qobject_cast<DocumentEditor*>(tabs_->currentWidget());
}

DocumentEditor* MainWindow::newDocument()
{
    auto* editor = new DocumentEditor(tr("Без имени %1").arg(++untitledCounter_));
    attachEditor(editor);
    statusBar()->showMessage(tr("Создан новый документ"), 5000);
    return editor;
}

void MainWindow::attachEditor(DocumentEditor* editor)
{
    new SyntaxHighlighter(editor->document());
    connect(editor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::updateDocuments);
    connect(editor, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursor);
    connect(editor, &QTextEdit::copyAvailable, this, &MainWindow::updateActions);
    tabs_->addTab(editor, editor->displayName()); // Qt получает владение страницей.
    tabs_->setCurrentWidget(editor);
    applySyntax(editor);
    updateDocuments();
    editor->setFocus();
}

bool MainWindow::openFile(const QString& path)
{
    for (int i = 0; i < tabs_->count(); ++i) {
        auto* editor = qobject_cast<DocumentEditor*>(tabs_->widget(i));
        if (!editor->filePath().isEmpty() && normalizedPath(editor->filePath()) == normalizedPath(path)) {
            tabs_->setCurrentIndex(i);
            return true;
        }
    }
    auto* editor = new DocumentEditor(QString());
    if (!editor->loadFile(path)) {
        showError(tr("Ошибка открытия"), path + "\n" + editor->lastError());
        delete editor;
        return false;
    }
    attachEditor(editor);
    statusBar()->showMessage(tr("Открыт документ %1").arg(editor->filePath()), 5000);
    return true;
}

void MainWindow::applySyntax(DocumentEditor* editor)
{
    QList<HighlightRule> rules;
    const QString extension = QFileInfo(editor->filePath()).suffix().toLower();
    for (const auto& syntax : syntaxes_) {
        if (syntax.extensions.contains(extension)) { rules = syntax.rules; break; }
    }
    editor->document()->findChild<SyntaxHighlighter*>()->setRules(rules);
}

bool MainWindow::saveDocument(DocumentEditor* editor, bool saveAs)
{
    if (!editor) return false;
    QString path = editor->filePath();
    if (saveAs || path.isEmpty()) {
        path = QFileDialog::getSaveFileName(this, tr("Сохранить документ"),
            path.isEmpty() ? QDir::homePath() + "/" + editor->displayName() + ".txt" : path,
            tr("Все файлы (*)"));
        if (path.isEmpty()) return false;
    }
    for (int i = 0; i < tabs_->count(); ++i) {
        auto* other = qobject_cast<DocumentEditor*>(tabs_->widget(i));
        if (other != editor && !other->filePath().isEmpty()
            && normalizedPath(other->filePath()) == normalizedPath(path)) {
            showError(tr("Ошибка сохранения"), tr("Файл уже открыт в другой вкладке."));
            return false;
        }
    }
    if (!editor->saveFile(path)) {
        showError(tr("Ошибка сохранения"), path + "\n" + editor->lastError());
        return false;
    }
    applySyntax(editor);
    updateDocuments();
    statusBar()->showMessage(tr("Документ %1 сохранён в каталоге %2")
        .arg(editor->displayName(), QFileInfo(editor->filePath()).absolutePath()), 5000);
    return true;
}

bool MainWindow::saveAll()
{
    for (int i = 0; i < tabs_->count(); ++i) {
        auto* editor = qobject_cast<DocumentEditor*>(tabs_->widget(i));
        if ((editor->document()->isModified() || editor->filePath().isEmpty())
            && !saveDocument(editor)) return false;
    }
    return true;
}

bool MainWindow::closeDocument(int index)
{
    auto* editor = qobject_cast<DocumentEditor*>(tabs_->widget(index));
    if (!editor) return false;
    if (editor->document()->isModified()) {
        tabs_->setCurrentIndex(index);
        const auto answer = QMessageBox::question(this, tr("Сохранить документ?"),
            tr("Сохранить изменения в «%1»?").arg(editor->displayName()),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
        if (answer != QMessageBox::Save && answer != QMessageBox::Discard) return false;
        if (answer == QMessageBox::Save && !saveDocument(editor)) return false;
    }
    tabs_->removeTab(index);
    delete editor; // removeTab сам не удаляет страницу.
    updateDocuments(); updateCursor(); updateActions();
    statusBar()->showMessage(tr("Документ закрыт"), 5000);
    return true;
}

bool MainWindow::closeAll()
{
    while (tabs_->count() > 0) if (!closeDocument(0)) return false;
    return true;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QList<QPair<QString, QString>> changed;
    for (int i = 0; i < tabs_->count(); ++i) {
        auto* editor = qobject_cast<DocumentEditor*>(tabs_->widget(i));
        if (editor->document()->isModified()) changed.append({editor->displayName(),
            editor->filePath().isEmpty() ? tr("Не сохранён") : QFileInfo(editor->filePath()).absolutePath()});
    }
    if (!changed.isEmpty()) {
        UnsavedDocumentsDialog dialog(changed, this);
        const int answer = dialog.exec();
        if (answer == UnsavedDocumentsDialog::Cancel) { event->ignore(); return; }
        if (answer == UnsavedDocumentsDialog::SaveAll) {
            // Сохраняем только изменённые: пустой нетронутый документ не мешает выходу.
            for (int i = 0; i < tabs_->count(); ++i) {
                auto* editor = qobject_cast<DocumentEditor*>(tabs_->widget(i));
                if (editor->document()->isModified() && !saveDocument(editor)) {
                    event->ignore(); return;
                }
            }
        }
    }
    event->accept();
}

void MainWindow::updateDocuments()
{
    QStringList names;
    for (int i = 0; i < tabs_->count(); ++i) {
        auto* editor = qobject_cast<DocumentEditor*>(tabs_->widget(i));
        const QString title = editor->displayName() + (editor->document()->isModified() ? "*" : "");
        tabs_->setTabText(i, title);
        tabs_->setTabToolTip(i, editor->filePath());
        names.append(title);
    }
    documents_->setStringList(names);
    if (documentList_) documentList_->setCurrentIndex(documents_->index(tabs_->currentIndex()));
}

void MainWindow::updateCursor()
{
    auto* editor = currentEditor();
    if (!editor) { cursorLabel_->setText(tr("Нет документа")); return; }
    const auto cursor = editor->textCursor();
    cursorLabel_->setText(tr("Строка %1, столбец %2")
        .arg(cursor.blockNumber() + 1).arg(cursor.positionInBlock() + 1));
}

void MainWindow::updateActions()
{
    auto* editor = currentEditor();
    for (auto* action : documentActions_) action->setEnabled(editor != nullptr);
    cutAction_->setEnabled(editor && editor->textCursor().hasSelection());
    copyAction_->setEnabled(editor && editor->textCursor().hasSelection());
}

void MainWindow::showError(const QString& title, const QString& detail)
{
    QMessageBox::critical(this, title, detail);
}
