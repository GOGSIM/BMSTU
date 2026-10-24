#pragma once

#include <QWidget>

class QLineEdit;
class QHBoxLayout;
class QPushButton;
class QRadioButton;

class CalculatorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CalculatorWindow(QWidget* parent = nullptr);

private slots:
    void slotModeChanged();
    void slotDigitClicked();
    void slotPointClicked();

private:
    QLineEdit* display_;
    QRadioButton* ordinaryModeButton_;
    QRadioButton* engineeringModeButton_;
    QPushButton* clearButton_;
    QHBoxLayout* buttonPanelsLayout_;
    QWidget* engineeringPanel_;
    bool waitingForNewValue_;
    bool errorState_;
};

