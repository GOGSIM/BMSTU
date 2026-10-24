#pragma once

#include <QString>
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
    void slotClear();
    void slotBinaryOperationClicked();
    void slotEquals();
    void slotOrdinaryUnaryOperationClicked();
    void slotTrigonometricOperationClicked();
    void slotHyperbolicOperationClicked();
    void slotLogarithmicOperationClicked();
    void slotPowerOperationClicked();
    void slotSpecialOperationClicked();

private:
    void showValue(double value);
    void showError();
    bool calculate(double rightOperand, double& result) const;

    QLineEdit* display_;
    QRadioButton* ordinaryModeButton_;
    QRadioButton* engineeringModeButton_;
    QPushButton* clearButton_;
    QHBoxLayout* buttonPanelsLayout_;
    QWidget* engineeringPanel_;
    bool waitingForNewValue_;
    bool errorState_;
    double storedValue_;
    QString pendingOperation_;
};

