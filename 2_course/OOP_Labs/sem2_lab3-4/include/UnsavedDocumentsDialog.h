#pragma once
#include <QDialog>
#include <QList>
#include <QPair>

class UnsavedDocumentsDialog : public QDialog
{
    Q_OBJECT
public:
    enum Choice { Cancel = QDialog::Rejected, SaveAll = QDialog::Accepted, DiscardAll = 2 };
    explicit UnsavedDocumentsDialog(const QList<QPair<QString, QString>>& files,
                                    QWidget* parent = nullptr);
};
