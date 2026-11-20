#include "UnsavedDocumentsDialog.h"
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

UnsavedDocumentsDialog::UnsavedDocumentsDialog(const QList<QPair<QString, QString>>& files,
                                             QWidget* parent) : QDialog(parent)
{
    setObjectName("unsavedDocumentsDialog");
    setWindowTitle(tr("Сохранить изменённые документы?"));
    resize(660, 320);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("Следующие документы содержат несохранённые изменения:"), this));
    auto* table = new QTableWidget(files.size(), 2, this);
    table->setObjectName("unsavedTable");
    table->setHorizontalHeaderLabels({tr("Имя файла"), tr("Расположение")});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    for (int row = 0; row < files.size(); ++row) {
        table->setItem(row, 0, new QTableWidgetItem(files.at(row).first));
        table->setItem(row, 1, new QTableWidgetItem(files.at(row).second));
    }
    layout->addWidget(table);
    auto* buttons = new QDialogButtonBox(this);
    auto* save = buttons->addButton(tr("Сохранить все"), QDialogButtonBox::AcceptRole);
    save->setObjectName("saveAllButton");
    save->setDefault(true);
    auto* discard = buttons->addButton(tr("Не сохранять"), QDialogButtonBox::DestructiveRole);
    discard->setObjectName("discardAllButton");
    auto* cancel = buttons->addButton(QDialogButtonBox::Cancel);
    cancel->setText(tr("Отмена"));
    connect(save, &QPushButton::clicked, this, [this] { done(SaveAll); });
    connect(discard, &QPushButton::clicked, this, [this] { done(DiscardAll); });
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    layout->addWidget(buttons);
}
