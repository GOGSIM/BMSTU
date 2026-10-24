#pragma once

#include <QString>
#include <QWidget>

class QLineEdit;
class QHBoxLayout;
class QKeyEvent;
class QPushButton;
class QRadioButton;

class CalculatorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CalculatorWindow(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

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
    bool clickButton(const QString& text);

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
