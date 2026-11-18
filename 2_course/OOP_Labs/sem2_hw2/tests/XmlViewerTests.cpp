#include "MainWindow.h"
#include "XmlTreeModel.h"

#include <QAction>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QPersistentModelIndex>
#include <QPointer>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTimer>
#include <QTreeView>
#include <QtTest>

namespace
{
// Маленький независимый пример проверяет экранирование XML, кириллицу,
// несколько телефонов, ноль мест и порядок географических координат.
const QByteArray fixture = R"(<?xml version="1.0" encoding="utf-8"?>
<catering><organization>
<Name>Кафе &amp; чай</Name><AdmArea>Центральный округ</AdmArea>
<District>Басманный район</District><Address>Бауманская улица</Address>
<SeatsCount>0</SeatsCount><Latitude_WGS84>55.77</Latitude_WGS84>
<Longitude_WGS84>37.67</Longitude_WGS84>
<PublicPhone><phone>111</phone><phone/><phone>222</phone></PublicPhone>
</organization></catering>)";

QString writeFile(const QTemporaryDir& directory, const QString& name, const QByteArray& content)
{
    const QString path = directory.filePath(name);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(content) != content.size()) {
        return QString();
    }
    return path;
}

QStandardItem* child(QStandardItem* parent, const QString& label)
{
    for (int row = 0; row < parent->rowCount(); ++row) {
        if (parent->child(row, 0)->text() == label) {
            return parent->child(row, 0);
        }
    }
    return nullptr;
}

QString value(QStandardItem* parent, const QString& label)
{
    QStandardItem* item = child(parent, label);
    return item ? parent->child(item->row(), 1)->text() : QString();
}
}

class XmlViewerTests : public QObject
{
    Q_OBJECT

private slots:
    void constructorParsesReadableHierarchy()
    {
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        const QString path = writeFile(directory, "пример.xml", fixture);
        QVERIFY(!path.isEmpty());
        XmlTreeModel model(path);
        QVERIFY2(model.lastError().isEmpty(), qPrintable(model.lastError()));
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.columnCount(), 2);
        QCOMPARE(model.item(0)->text(), QStringLiteral("пример.xml"));
        QCOMPARE(model.item(0)->data(XmlTreeModel::FilePathRole).toString(),
                 XmlTreeModel::normalizedPath(path));
        QStandardItem* organization = model.item(0)->child(0, 0);
        QCOMPARE(model.item(0)->child(0, 1)->text(), QStringLiteral("Кафе & чай"));
        QCOMPARE(value(organization, "Телефон"), QStringLiteral("111; 222"));
        QCOMPARE(value(organization, "Кол-во посадочных мест"), QStringLiteral("0"));
        QCOMPARE(value(organization, "Район"), QStringLiteral("Басманный район"));
        QStandardItem* geo = child(organization, "Геоданные");
        QVERIFY(geo);
        QCOMPARE(value(geo, "Широта"), QStringLiteral("55.77"));
        QCOMPARE(value(geo, "Долгота"), QStringLiteral("37.67"));
        QVERIFY(!organization->isEditable());
        QVERIFY(!geo->child(0, 1)->isEditable());
    }

    void missingValuesAndEmptyDataset()
    {
        QTemporaryDir directory;
        XmlTreeModel model(writeFile(directory, "missing.xml",
                                    "<catering><organization/></catering>"));
        QVERIFY(model.lastError().isEmpty());
        QStandardItem* organization = model.item(0)->child(0);
        QCOMPARE(model.item(0)->child(0, 1)->text(), QStringLiteral("Нет данных"));
        QCOMPARE(value(organization, "Телефон"), QStringLiteral("Нет данных"));
        QCOMPARE(value(organization, "Кол-во посадочных мест"), QStringLiteral("Нет данных"));
        QCOMPARE(value(child(organization, "Геоданные"), "Широта"), QStringLiteral("Нет данных"));
        QVERIFY(model.appendFile(writeFile(directory, "empty.xml", "<catering/>")));
        QCOMPARE(model.item(1)->rowCount(), 0);
    }

    void rejectsInvalidXml_data()
    {
        QTest::addColumn<QByteArray>("content");
        QTest::addColumn<QString>("messagePart");
        QTest::newRow("empty-file") << QByteArray() << QStringLiteral("строка");
        QTest::newRow("broken-xml") << QByteArray("<catering><organization></catering>")
                                    << QStringLiteral("столбец");
        QTest::newRow("wrong-root") << QByteArray("<other/>") << QStringLiteral("структура");
        QTest::newRow("unexpected-record") << QByteArray("<catering><wrong/></catering>")
                                           << QStringLiteral("элемент");
    }

    void rejectsInvalidXml()
    {
        QFETCH(QByteArray, content);
        QFETCH(QString, messagePart);
        QTemporaryDir directory;
        const QString goodPath = writeFile(directory, "good.xml", fixture);
        XmlTreeModel model(goodPath);
        QStandardItem* original = model.item(0);
        const QString badPath = writeFile(directory, "bad.xml", content);
        QVERIFY(!model.appendFile(badPath));
        QVERIFY(model.lastError().contains(messagePart));
        QVERIFY(model.lastError().contains("bad.xml"));
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.item(0), original);
        XmlTreeModel failedConstructor(badPath);
        QCOMPARE(failedConstructor.rowCount(), 0);
        QVERIFY(!failedConstructor.lastError().isEmpty());
    }

    void missingFileAndDuplicatePath()
    {
        QTemporaryDir directory;
        const QString path = writeFile(directory, "one.xml", fixture);
        XmlTreeModel model(path);
        QVERIFY(!model.appendFile(directory.filePath("absent.xml")));
        QVERIFY(model.lastError().contains("absent.xml"));
        QVERIFY(model.appendFile(directory.path() + "/./one.xml"));
        QCOMPARE(model.rowCount(), 1);
        QVERIFY(model.lastError().isEmpty());
    }

    void suppliedDatasets_data()
    {
        QTest::addColumn<QString>("name");
        QTest::addColumn<int>("count");
        QTest::newRow("cafes") << QStringLiteral("cafes.xml") << 448;
        QTest::newRow("coffeehouses") << QStringLiteral("coffeehouses.xml") << 45;
        QTest::newRow("bars") << QStringLiteral("bars.xml") << 131;
        QTest::newRow("fast-food") << QStringLiteral("fast_food.xml") << 15;
    }

    void suppliedDatasets()
    {
        QFETCH(QString, name);
        QFETCH(int, count);
        XmlTreeModel model(QStringLiteral(TEST_DATA_DIR) + '/' + name);
        QVERIFY2(model.lastError().isEmpty(), qPrintable(model.lastError()));
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.item(0)->rowCount(), count);
    }

    void movingKeepsSubtreeWithoutReadingFileAgain()
    {
        QTemporaryDir directory;
        const QString first = writeFile(directory, "first.xml", fixture);
        const QString second = writeFile(directory, "second.xml", fixture);
        MainWindow window;
        QTabWidget* tabs = window.findChild<QTabWidget*>("documentTabs");
        QVERIFY(window.openFile(first));
        QVERIFY(!window.moveFileToNewTab(0));
        QVERIFY(window.openFile(second));
        QStandardItem* root = window.currentModel()->item(0);
        QStandardItem* organization = root->child(0);
        QPersistentModelIndex oldIndex(root->index());
        QVERIFY(QFile::remove(first));
        QVERIFY(window.moveFileToNewTab(0));
        QCOMPARE(tabs->count(), 2);
        QCOMPARE(tabs->currentIndex(), 1);
        QCOMPARE(window.currentModel()->item(0), root);
        QCOMPARE(root->child(0), organization);
        QCOMPARE(value(organization, "Телефон"), QStringLiteral("111; 222"));
        QVERIFY(!oldIndex.isValid());
        QVERIFY(!window.moveFileToNewTab(0));
        tabs->setCurrentIndex(0);
        QCOMPARE(window.currentModel()->rowCount(), 1);
        QCOMPARE(window.currentModel()->item(0)->text(), QStringLiteral("second.xml"));
    }

    void duplicatesAndSameBasenameInDifferentDirectories()
    {
        QTemporaryDir left;
        QTemporaryDir right;
        const QString first = writeFile(left, "same.xml", fixture);
        const QString second = writeFile(right, "same.xml", fixture);
        MainWindow window;
        QVERIFY(window.openFile(first));
        QVERIFY(window.openFile(second));
        QCOMPARE(window.currentModel()->rowCount(), 2);
        QVERIFY(window.moveFileToNewTab(0));
        QTabWidget* tabs = window.findChild<QTabWidget*>("documentTabs");
        tabs->setCurrentIndex(0);
        QVERIFY(window.openFile(left.path() + "/./same.xml"));
        QCOMPARE(tabs->currentIndex(), 1);
        QCOMPARE(tabs->count(), 2);
        QCOMPARE(window.currentModel()->rowCount(), 1);
    }

    void closingDeletesPagesAndModels()
    {
        QTemporaryDir directory;
        MainWindow window;
        QVERIFY(window.openFile(writeFile(directory, "one.xml", fixture)));
        QVERIFY(window.openFile(writeFile(directory, "two.xml", fixture)));
        QVERIFY(window.moveFileToNewTab(0));
        QPointer<XmlTreeModel> removedModel(window.currentModel());
        QVERIFY(window.closeFile(0));
        QVERIFY(removedModel.isNull());
        QTabWidget* tabs = window.findChild<QTabWidget*>("documentTabs");
        QCOMPARE(tabs->count(), 1);
        QVERIFY(!window.closeFile(-1));
        QVERIFY(!window.closeFile(5));
        QVERIFY(window.openFile(writeFile(directory, "three.xml", fixture)));
        QVERIFY(window.moveFileToNewTab(0));
        removedModel = window.currentModel();
        window.closeAllFiles();
        QVERIFY(removedModel.isNull());
        QCOMPARE(tabs->count(), 1);
        QCOMPARE(window.currentModel()->rowCount(), 0);
        QCOMPARE(window.currentModel()->headerData(0, Qt::Horizontal).toString(), QStringLiteral("Поле"));
        window.closeAllFiles(); // Повторная очистка пустой модели допустима.
        QVERIFY(window.openFile(directory.filePath("three.xml")));
        QVERIFY(window.closeFile(0));
        QCOMPARE(tabs->count(), 1);
    }

    void contextMenuOnlyOnFileRows()
    {
        QTemporaryDir directory;
        MainWindow window;
        QVERIFY(window.openFile(writeFile(directory, "one.xml", fixture)));
        window.show();
        QCoreApplication::processEvents();
        QTreeView* view = qobject_cast<QTreeView*>(window.findChild<QTabWidget*>()->currentWidget());
        auto probe = [&](const QPoint& position, bool expectedMenu, bool expectedMove) {
            bool sawMenu = false;
            bool moveEnabled = false;
            QTimer timer;
            timer.setSingleShot(true);
            connect(&timer, &QTimer::timeout, &window, [&]() {
                QMenu* menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
                if (menu) {
                    sawMenu = true;
                    QCOMPARE(menu->actions().size(), 2);
                    moveEnabled = menu->actions().at(1)->isEnabled();
                    menu->close();
                }
            });
            timer.start(0);
            QMetaObject::invokeMethod(view, "customContextMenuRequested", Qt::DirectConnection,
                                      Q_ARG(QPoint, position));
            QCoreApplication::processEvents();
            QCOMPARE(sawMenu, expectedMenu);
            if (expectedMenu) {
                QCOMPARE(moveEnabled, expectedMove);
            }
        };
        const QModelIndex root = window.currentModel()->index(0, 0);
        probe(view->visualRect(root).center(), true, false);
        probe(view->visualRect(root.siblingAtColumn(1)).center(), true, false);
        probe(view->visualRect(window.currentModel()->index(0, 0, root)).center(), false, false);
        probe(QPoint(10, view->viewport()->height() - 10), false, false);
        QVERIFY(window.openFile(writeFile(directory, "two.xml", fixture)));
        probe(view->visualRect(root).center(), true, true);
    }

    void windowReportsParseErrorWithoutLosingDocuments()
    {
        QTemporaryDir directory;
        MainWindow window;
        QVERIFY(window.openFile(writeFile(directory, "good.xml", fixture)));
        bool sawCriticalMessage = false;
        QTimer::singleShot(0, &window, [&]() {
            QMessageBox* message = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
            if (message) {
                sawCriticalMessage = message->icon() == QMessageBox::Critical
                    && message->text().contains("bad.xml") && message->text().contains("строка");
                message->accept();
            }
        });
        QVERIFY(!window.openFile(writeFile(directory, "bad.xml", "<catering>")));
        QVERIFY(sawCriticalMessage);
        QCOMPARE(window.currentModel()->rowCount(), 1);
    }
};

QTEST_MAIN(XmlViewerTests)
#include "XmlViewerTests.moc"
