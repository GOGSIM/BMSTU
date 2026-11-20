#include "DocumentEditor.h"
#include "MainWindow.h"
#include "SyntaxHighlighter.h"
#include "SyntaxSettings.h"
#include "UnsavedDocumentsDialog.h"
#include <QAction>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QDockWidget>
#include <QListView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStringListModel>
#include <QTabWidget>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTest>
#include <QTextBlock>
#include <QTextLayout>
#include <QTimer>

class MemoryReader : public SyntaxSettingsReader
{
public:
    bool read(const QString&, QList<SyntaxDefinition>& result, QString&) const override
    {
        HighlightRule rule;
        rule.pattern = QRegularExpression("MAGIC");
        rule.format.setForeground(Qt::red);
        result = {{"custom", "Custom", {"custom"}, {rule}}};
        return true;
    }
};

class EditorTests : public QObject
{
    Q_OBJECT
private:
    static QString settings() { return QString(SOURCE_DIR) + "/settings/syntaxes.xml"; }
    static void write(const QString& path, const QByteArray& data)
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        QCOMPARE(file.write(data), data.size());
    }
    static QColor colorAt(QTextBlock block, int position)
    {
        for (const auto& range : block.layout()->formats())
            if (position >= range.start && position < range.start + range.length)
                return range.format.foreground().color();
        return {};
    }
    static MainWindow* window()
    {
        return new MainWindow(std::make_unique<XmlSyntaxSettingsReader>(), settings());
    }
    static void selectSavePath(const QString& path)
    {
        QTimer::singleShot(0, [path] {
            auto* dialog = qobject_cast<QFileDialog*>(QApplication::activeModalWidget());
            QVERIFY(dialog);
            dialog->selectFile(path);
            QMetaObject::invokeMethod(dialog, "accept", Qt::DirectConnection);
        });
    }
private slots:
    void initTestCase()
    {
        // Детерминированные Qt-диалоги для автоматических проверок.
        // В приложении остаются стандартные системные диалоги.
        QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    }
    void documentRoundTrip()
    {
        QTemporaryDir directory;
        DocumentEditor editor("Новый");
        editor.insertPlainText(QString::fromUtf8("Привет\nмир"));
        QVERIFY(editor.document()->isModified());
        const QString path = directory.filePath("текст.txt");
        QVERIFY(editor.saveFile(path));
        QVERIFY(!editor.document()->isModified());
        DocumentEditor loaded("Другой");
        QVERIFY(loaded.loadFile(path));
        QCOMPARE(loaded.toPlainText(), editor.toPlainText());
        loaded.insertPlainText("changed");
        const QString oldPath = loaded.filePath();
        QVERIFY(!loaded.saveFile(directory.filePath("missing/file.txt")));
        QCOMPARE(loaded.filePath(), oldPath);
        QVERIFY(loaded.document()->isModified());
        const QString oldText = loaded.toPlainText();
        QVERIFY(!loaded.loadFile(directory.filePath("missing.txt")));
        QCOMPARE(loaded.toPlainText(), oldText);
    }
    void settingsValidation()
    {
        XmlSyntaxSettingsReader reader;
        QList<SyntaxDefinition> definitions;
        QString error;
        QVERIFY2(reader.read(settings(), definitions, error), qPrintable(error));
        QVERIFY(definitions.first().extensions.contains("cpp"));
        QCOMPARE(definitions.first().rules.first().format.fontWeight(), int(QFont::Bold));
        QTemporaryDir directory;
        const QString path = directory.filePath("settings.xml");
        for (const QByteArray& xml : {QByteArray("<bad>"), QByteArray("<settings/>"),
             QByteArray("<settings><syntaxes><syntax id='x' ext_list='x'><rule><pattern value='['/><format foreground='red'/></rule></syntax></syntaxes></settings>"),
             QByteArray("<settings><syntaxes><syntax id='x' ext_list='x'><rule><pattern value='x'/><format foreground='wrong'/></rule></syntax></syntaxes></settings>")}) {
            write(path, xml);
            QVERIFY(!reader.read(path, definitions, error));
            QVERIFY(!error.isEmpty());
            QCOMPARE(definitions.size(), 1); // Не публикуем частично прочитанный набор.
        }
    }
    void highlighting()
    {
        XmlSyntaxSettingsReader reader;
        QList<SyntaxDefinition> definitions;
        QString error;
        QVERIFY(reader.read(settings(), definitions, error));
        QTextDocument document;
        SyntaxHighlighter highlighter(&document);
        highlighter.setRules(definitions.first().rules);
        document.setPlainText("int x; /* comment\nclass remains comment\n*/ int y;\n\"https://site\"; // comment");
        highlighter.rehighlight();
        QCOMPARE(colorAt(document.firstBlock(), 0), QColor("#174EA6"));
        QCOMPARE(colorAt(document.findBlockByNumber(1), 0), QColor("#008000"));
        QCOMPARE(colorAt(document.findBlockByNumber(2), 3), QColor("#174EA6"));
        QCOMPARE(colorAt(document.findBlockByNumber(3), 9), QColor("#A31515"));
        const bool modified = document.isModified();
        highlighter.rehighlight();
        QCOMPARE(document.isModified(), modified); // Оформление не меняет документ.
    }
    void tabsAndSaveAs()
    {
        QTemporaryDir directory;
        const QString path = directory.filePath("same.cpp");
        write(path, "int value;");
        std::unique_ptr<MainWindow> w(window());
        auto* tabs = w->findChild<QTabWidget*>("documentTabs");
        auto* first = w->currentEditor();
        first->insertPlainText("first");
        QVERIFY(tabs->tabText(0).endsWith('*'));
        QVERIFY(w->openFile(path));
        QCOMPARE(tabs->count(), 2);
        QVERIFY(w->openFile(path));
        QCOMPARE(tabs->count(), 2);
        QCOMPARE(first->toPlainText(), QString("first"));
        auto* editor = w->currentEditor();
        editor->insertPlainText("changed ");
        QVERIFY(w->saveDocument(editor));
        QVERIFY(!tabs->tabText(1).endsWith('*'));
        auto* highlighter = editor->document()->findChild<SyntaxHighlighter*>();
        QVERIFY(highlighter);
        QCOMPARE(colorAt(editor->document()->firstBlock(), 8), QColor("#174EA6"));
        QTimer::singleShot(0, [] {
            auto* dialog = qobject_cast<QFileDialog*>(QApplication::activeModalWidget());
            QVERIFY(dialog); dialog->reject();
        });
        QVERIFY(!w->saveDocument(editor, true));
        QCOMPARE(editor->filePath(), path);
        first->document()->setModified(false);
        QVERIFY(w->closeAll());
        QCOMPARE(tabs->count(), 0);
        QVERIFY(!w->findChild<QAction*>("saveAction")->isEnabled());
    }
    void strategyReplacement()
    {
        QTemporaryDir directory;
        const QString path = directory.filePath("test.custom");
        write(path, "MAGIC");
        MainWindow w(std::make_unique<MemoryReader>(), "unused");
        QVERIFY(w.openFile(path));
        auto* highlighter = w.currentEditor()->document()->findChild<SyntaxHighlighter*>();
        highlighter->rehighlight();
        QCOMPARE(colorAt(w.currentEditor()->document()->firstBlock(), 0), QColor(Qt::red));
    }
    void externalXmlExtension()
    {
        QTemporaryDir directory;
        const QString config = directory.filePath("settings.xml");
        write(config, "<settings><syntaxes><syntax id='new' ext_list='NEW'><rule><pattern value='MAGIC'/><format foreground='#ff0000'/></rule></syntax></syntaxes></settings>");
        const QString path = directory.filePath("test.new");
        write(path, "MAGIC");
        MainWindow w(std::make_unique<XmlSyntaxSettingsReader>(), config);
        QVERIFY(w.openFile(path));
        auto* highlighter = w.currentEditor()->document()->findChild<SyntaxHighlighter*>();
        highlighter->rehighlight();
        QCOMPARE(colorAt(w.currentEditor()->document()->firstBlock(), 0), QColor(Qt::red));
    }
    void saveAsChangesSyntax()
    {
        QTemporaryDir directory;
        std::unique_ptr<MainWindow> w(window());
        w->currentEditor()->insertPlainText("int value;");
        selectSavePath(directory.filePath("test.cpp"));
        QVERIFY(w->saveDocument(w->currentEditor()));
        QCOMPARE(colorAt(w->currentEditor()->document()->firstBlock(), 0), QColor("#174EA6"));
        selectSavePath(directory.filePath("test.txt"));
        QVERIFY(w->saveDocument(w->currentEditor(), true));
        QVERIFY(w->currentEditor()->document()->firstBlock().layout()->formats().isEmpty());
        QCOMPARE(w->currentEditor()->displayName(), QString("test.txt"));
    }
    void saveAllAndExitSave()
    {
        QTemporaryDir directory;
        std::unique_ptr<MainWindow> w(window());
        auto* first = w->currentEditor();
        first->insertPlainText("one");
        selectSavePath(directory.filePath("one.txt"));
        QVERIFY(w->saveAll());
        auto* second = w->newDocument();
        second->insertPlainText("two");
        selectSavePath(directory.filePath("two.txt"));
        QVERIFY(w->saveAll());
        first->insertPlainText(" updated");
        second->insertPlainText(" updated");
        w->show();
        QTimer::singleShot(0, [] {
            auto* dialog = qobject_cast<UnsavedDocumentsDialog*>(QApplication::activeModalWidget());
            QVERIFY(dialog); dialog->findChild<QPushButton*>("saveAllButton")->click();
        });
        QVERIFY(w->close());
        QVERIFY(!first->document()->isModified());
        QVERIFY(!second->document()->isModified());
        DocumentEditor loaded("new");
        QVERIFY(loaded.loadFile(first->filePath()));
        QCOMPARE(loaded.toPlainText(), first->toPlainText());
    }
    void exitWriteErrorKeepsTabs()
    {
        QTemporaryDir directory;
        const QString path = directory.filePath("locked.txt");
        write(path, "original");
        std::unique_ptr<MainWindow> w(window());
        QVERIFY(w->openFile(path));
        w->currentEditor()->insertPlainText("changed");
        // Детерминированная ошибка: исходный путь превращается в каталог.
        QVERIFY(QFile::remove(path));
        QVERIFY(QDir().mkdir(path));
        w->show();
        QTimer::singleShot(0, [] {
            auto* dialog = qobject_cast<UnsavedDocumentsDialog*>(QApplication::activeModalWidget());
            QVERIFY(dialog); dialog->done(UnsavedDocumentsDialog::SaveAll);
            QTimer::singleShot(0, [] {
                auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
                QVERIFY(box); box->accept();
            });
        });
        QVERIFY(!w->close());
        QCOMPARE(w->findChild<QTabWidget*>()->count(), 2);
        QVERIFY(w->currentEditor()->document()->isModified());
    }
    void actionsAndPanels()
    {
        std::unique_ptr<MainWindow> w(window());
        w->show();
        auto* editor = w->currentEditor();
        editor->insertPlainText("alpha\nbeta");
        for (const QString& name : {"newAction", "openAction", "saveAction", "saveAllAction"})
            QVERIFY(!w->findChild<QAction*>(name)->icon().isNull());
        w->findChild<QAction*>("selectAllAction")->trigger();
        QVERIFY(w->findChild<QAction*>("copyAction")->isEnabled());
        w->findChild<QAction*>("copyAction")->trigger();
        w->findChild<QAction*>("newAction")->trigger();
        w->findChild<QAction*>("pasteAction")->trigger();
        QCOMPARE(w->currentEditor()->toPlainText(), QString("alpha\nbeta"));
        auto* dock = w->findChild<QDockWidget*>("explorerDock");
        dock->toggleViewAction()->trigger();
        QVERIFY(dock->isHidden());
        dock->toggleViewAction()->trigger();
        QVERIFY(!dock->isHidden());
        auto* list = w->findChild<QListView*>("documentList");
        const auto first = list->model()->index(0, 0);
        QVERIFY(QMetaObject::invokeMethod(list, "clicked", Qt::DirectConnection, Q_ARG(QModelIndex, first)));
        QCOMPARE(w->currentEditor(), editor);
        auto cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::End);
        editor->setTextCursor(cursor);
        bool coordinatesFound = false;
        for (auto* label : w->findChildren<QLabel*>())
            coordinatesFound |= label->text().contains("Строка 2, столбец 5");
        QVERIFY(coordinatesFound);
    }
    void exitCancellation()
    {
        std::unique_ptr<MainWindow> w(window());
        w->show();
        w->currentEditor()->insertPlainText("one");
        w->newDocument()->insertPlainText("two");
        auto* tabs = w->findChild<QTabWidget*>("documentTabs");
        QTimer::singleShot(0, [] {
            auto* dialog = qobject_cast<UnsavedDocumentsDialog*>(QApplication::activeModalWidget());
            QVERIFY(dialog);
            QCOMPARE(dialog->findChild<QTableWidget*>()->rowCount(), 2);
            dialog->reject();
        });
        QVERIFY(!w->close());
        QCOMPARE(tabs->count(), 2);
        QTimer::singleShot(0, [] {
            auto* dialog = qobject_cast<UnsavedDocumentsDialog*>(QApplication::activeModalWidget());
            QVERIFY(dialog); dialog->done(UnsavedDocumentsDialog::SaveAll);
            QTimer::singleShot(0, [] {
                auto* file = qobject_cast<QFileDialog*>(QApplication::activeModalWidget());
                QVERIFY(file); file->reject();
            });
        });
        QVERIFY(!w->close());
        QCOMPARE(tabs->count(), 2);
        QTimer::singleShot(0, [] {
            auto* dialog = qobject_cast<UnsavedDocumentsDialog*>(QApplication::activeModalWidget());
            QVERIFY(dialog); dialog->done(UnsavedDocumentsDialog::DiscardAll);
        });
        QVERIFY(w->close());
    }
    void tabCancellation()
    {
        std::unique_ptr<MainWindow> w(window());
        w->currentEditor()->insertPlainText("changed");
        QTimer::singleShot(0, [] {
            auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
            QVERIFY(box); box->button(QMessageBox::Cancel)->click();
        });
        QVERIFY(!w->closeDocument(0));
        QVERIFY(w->currentEditor()->document()->isModified());
    }
    void duplicateNamesAndSaveConflict()
    {
        QTemporaryDir directory;
        QVERIFY(QDir().mkdir(directory.filePath("a")));
        QVERIFY(QDir().mkdir(directory.filePath("b")));
        const QString a = directory.filePath("a/test.cpp");
        const QString b = directory.filePath("b/test.cpp");
        write(a, "int a;"); write(b, "int b;");
        std::unique_ptr<MainWindow> w(window());
        QVERIFY(w->openFile(a));
        QVERIFY(w->openFile(b));
        QCOMPARE(w->findChild<QTabWidget*>()->count(), 3);
        w->currentEditor()->insertPlainText("modified ");
        QTimer::singleShot(0, [a] {
            auto* dialog = qobject_cast<QFileDialog*>(QApplication::activeModalWidget());
            QVERIFY(dialog);
            dialog->setOption(QFileDialog::DontConfirmOverwrite);
            dialog->selectFile(a);
            QMetaObject::invokeMethod(dialog, "accept", Qt::DirectConnection);
            QTimer::singleShot(0, [] {
                auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
                QVERIFY(box); box->accept();
            });
        });
        QVERIFY(!w->saveDocument(w->currentEditor(), true));
        QCOMPARE(w->currentEditor()->filePath(), b);
        QVERIFY(w->currentEditor()->document()->isModified());
        DocumentEditor loaded("new");
        QVERIFY(loaded.loadFile(a));
        QCOMPARE(loaded.toPlainText(), QString("int a;"));
    }
    void invalidSettingsStillAllowEditing()
    {
        QTemporaryDir directory;
        MainWindow w(std::make_unique<XmlSyntaxSettingsReader>(), directory.filePath("absent.xml"));
        QTimer::singleShot(0, [] {
            auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
            QVERIFY(box); box->accept();
        });
        // Первый timer показывает ошибку, второй закрывает её внутри exec().
        QCoreApplication::processEvents();
        w.currentEditor()->insertPlainText("still usable");
        QCOMPARE(w.currentEditor()->toPlainText(), QString("still usable"));
    }
};
QTEST_MAIN(EditorTests)
#include "EditorTests.moc"
