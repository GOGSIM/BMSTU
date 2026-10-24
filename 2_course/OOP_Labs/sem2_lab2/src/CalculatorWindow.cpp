#include "CalculatorWindow.h"

#include <cmath>

#include <QFontDatabase>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

CalculatorWindow::CalculatorWindow(QWidget* parent)
    : QWidget(parent),
      display_(new QLineEdit(this)),
      ordinaryModeButton_(new QRadioButton("Обычный", this)),
      engineeringModeButton_(new QRadioButton("Инженерный", this)),
      clearButton_(new QPushButton("C", this)),
      buttonPanelsLayout_(new QHBoxLayout),
      engineeringPanel_(new QWidget(this)),
      waitingForNewValue_(true),
      errorState_(false),
      storedValue_(0.0)
{
    setWindowTitle("Калькулятор (обычный)");

    QFont displayFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    displayFont.setPointSize(22);

    display_->setText("0");
    display_->setReadOnly(true);
    display_->setAlignment(Qt::AlignRight);
    display_->setFont(displayFont);
    display_->setMinimumHeight(50);

    ordinaryModeButton_->setChecked(true);

    QVBoxLayout* modeButtonsLayout = new QVBoxLayout;
    modeButtonsLayout->addWidget(ordinaryModeButton_);
    modeButtonsLayout->addWidget(engineeringModeButton_);

    QHBoxLayout* modeRowLayout = new QHBoxLayout;
    modeRowLayout->addLayout(modeButtonsLayout);
    modeRowLayout->addStretch();
    modeRowLayout->addWidget(clearButton_);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(display_);
    mainLayout->addLayout(modeRowLayout);
    mainLayout->addLayout(buttonPanelsLayout_);

    QWidget* ordinaryPanel = new QWidget(this);
    QGridLayout* ordinaryLayout = new QGridLayout(ordinaryPanel);

    QPushButton* button7 = new QPushButton("7", ordinaryPanel);
    QPushButton* button8 = new QPushButton("8", ordinaryPanel);
    QPushButton* button9 = new QPushButton("9", ordinaryPanel);
    QPushButton* divideButton = new QPushButton("/", ordinaryPanel);
    QPushButton* squareRootButton = new QPushButton("sqrt", ordinaryPanel);

    QPushButton* button4 = new QPushButton("4", ordinaryPanel);
    QPushButton* button5 = new QPushButton("5", ordinaryPanel);
    QPushButton* button6 = new QPushButton("6", ordinaryPanel);
    QPushButton* multiplyButton = new QPushButton("*", ordinaryPanel);
    QPushButton* reciprocalButton = new QPushButton("1/x", ordinaryPanel);

    QPushButton* button1 = new QPushButton("1", ordinaryPanel);
    QPushButton* button2 = new QPushButton("2", ordinaryPanel);
    QPushButton* button3 = new QPushButton("3", ordinaryPanel);
    QPushButton* subtractButton = new QPushButton("-", ordinaryPanel);
    QPushButton* equalsButton = new QPushButton("=", ordinaryPanel);

    QPushButton* button0 = new QPushButton("0", ordinaryPanel);
    QPushButton* pointButton = new QPushButton(".", ordinaryPanel);
    QPushButton* addButton = new QPushButton("+", ordinaryPanel);

    ordinaryLayout->addWidget(button7, 0, 0);
    ordinaryLayout->addWidget(button8, 0, 1);
    ordinaryLayout->addWidget(button9, 0, 2);
    ordinaryLayout->addWidget(divideButton, 0, 3);
    ordinaryLayout->addWidget(squareRootButton, 0, 4);

    ordinaryLayout->addWidget(button4, 1, 0);
    ordinaryLayout->addWidget(button5, 1, 1);
    ordinaryLayout->addWidget(button6, 1, 2);
    ordinaryLayout->addWidget(multiplyButton, 1, 3);
    ordinaryLayout->addWidget(reciprocalButton, 1, 4);

    ordinaryLayout->addWidget(button1, 2, 0);
    ordinaryLayout->addWidget(button2, 2, 1);
    ordinaryLayout->addWidget(button3, 2, 2);
    ordinaryLayout->addWidget(subtractButton, 2, 3);
    ordinaryLayout->addWidget(equalsButton, 2, 4, 2, 1);

    ordinaryLayout->addWidget(button0, 3, 0, 1, 2);
    ordinaryLayout->addWidget(pointButton, 3, 2);
    ordinaryLayout->addWidget(addButton, 3, 3);

    const QList<QPushButton*> digitButtons = {
        button0,
        button1,
        button2,
        button3,
        button4,
        button5,
        button6,
        button7,
        button8,
        button9
    };

    for (QPushButton* button : digitButtons) {
        connect(button, SIGNAL(clicked()), this, SLOT(slotDigitClicked()));
    }

    connect(pointButton, SIGNAL(clicked()), this, SLOT(slotPointClicked()));

    const QList<QPushButton*> binaryOperationButtons = {
        addButton,
        subtractButton,
        multiplyButton,
        divideButton
    };

    for (QPushButton* button : binaryOperationButtons) {
        connect(
            button,
            SIGNAL(clicked()),
            this,
            SLOT(slotBinaryOperationClicked())
        );
    }

    connect(equalsButton, SIGNAL(clicked()), this, SLOT(slotEquals()));
    connect(
        squareRootButton,
        SIGNAL(clicked()),
        this,
        SLOT(slotOrdinaryUnaryOperationClicked())
    );
    connect(
        reciprocalButton,
        SIGNAL(clicked()),
        this,
        SLOT(slotOrdinaryUnaryOperationClicked())
    );

    QGridLayout* engineeringLayout = new QGridLayout(engineeringPanel_);

    engineeringLayout->addWidget(new QPushButton("sinh", engineeringPanel_), 0, 0);
    engineeringLayout->addWidget(new QPushButton("sin", engineeringPanel_), 0, 1);
    engineeringLayout->addWidget(new QPushButton("exp", engineeringPanel_), 0, 2);
    engineeringLayout->addWidget(new QPushButton("x²", engineeringPanel_), 0, 3);

    engineeringLayout->addWidget(new QPushButton("cosh", engineeringPanel_), 1, 0);
    engineeringLayout->addWidget(new QPushButton("cos", engineeringPanel_), 1, 1);
    engineeringLayout->addWidget(new QPushButton("ln", engineeringPanel_), 1, 2);
    engineeringLayout->addWidget(new QPushButton("x³", engineeringPanel_), 1, 3);

    engineeringLayout->addWidget(new QPushButton("tanh", engineeringPanel_), 2, 0);
    engineeringLayout->addWidget(new QPushButton("tan", engineeringPanel_), 2, 1);
    engineeringLayout->addWidget(new QPushButton("log", engineeringPanel_), 2, 2);
    engineeringLayout->addWidget(new QPushButton("xʸ", engineeringPanel_), 2, 3);

    engineeringLayout->addWidget(new QPushButton("n!", engineeringPanel_), 3, 0);
    engineeringLayout->addWidget(new QPushButton("π", engineeringPanel_), 3, 1);
    engineeringLayout->addWidget(new QPushButton("³√x", engineeringPanel_), 3, 2);
    engineeringLayout->addWidget(new QPushButton("ʸ√x", engineeringPanel_), 3, 3);

    const QList<QPushButton*> engineeringButtons =
        engineeringPanel_->findChildren<QPushButton*>();

    for (QPushButton* button : engineeringButtons) {
        if (
            button->text() == "sin"
            || button->text() == "cos"
            || button->text() == "tan"
        ) {
            connect(
                button,
                SIGNAL(clicked()),
                this,
                SLOT(slotTrigonometricOperationClicked())
            );
        }

        if (
            button->text() == "sinh"
            || button->text() == "cosh"
            || button->text() == "tanh"
        ) {
            connect(
                button,
                SIGNAL(clicked()),
                this,
                SLOT(slotHyperbolicOperationClicked())
            );
        }
    }

    engineeringPanel_->hide();

    buttonPanelsLayout_->addWidget(engineeringPanel_);
    buttonPanelsLayout_->addWidget(ordinaryPanel);

    connect(
        engineeringModeButton_,
        SIGNAL(toggled(bool)),
        this,
        SLOT(slotModeChanged())
    );
    connect(clearButton_, SIGNAL(clicked()), this, SLOT(slotClear()));

    setLayout(mainLayout);
    setFixedSize(sizeHint());
}

void CalculatorWindow::slotModeChanged()
{
    const bool engineeringMode = engineeringModeButton_->isChecked();

    engineeringPanel_->setVisible(engineeringMode);
    setWindowTitle(
        engineeringMode
            ? "Калькулятор (инженерный)"
            : "Калькулятор (обычный)"
    );

    layout()->activate();
    setFixedSize(sizeHint());
}

void CalculatorWindow::slotDigitClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if (button == nullptr) {
        return;
    }

    if (waitingForNewValue_ || errorState_ || display_->text() == "0") {
        display_->setText(button->text());
        waitingForNewValue_ = false;
        errorState_ = false;
        return;
    }

    display_->setText(display_->text() + button->text());
}

void CalculatorWindow::slotPointClicked()
{
    if (waitingForNewValue_ || errorState_) {
        display_->setText("0.");
        waitingForNewValue_ = false;
        errorState_ = false;
        return;
    }

    if (!display_->text().contains('.')) {
        display_->setText(display_->text() + '.');
    }
}

void CalculatorWindow::slotClear()
{
    display_->setText("0");
    waitingForNewValue_ = true;
    errorState_ = false;
    storedValue_ = 0.0;
    pendingOperation_.clear();
}

void CalculatorWindow::slotBinaryOperationClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if (button == nullptr || errorState_) {
        return;
    }

    bool conversionOk = false;
    const double currentValue = display_->text().toDouble(&conversionOk);

    if (!conversionOk) {
        showError();
        return;
    }

    if (!pendingOperation_.isEmpty() && !waitingForNewValue_) {
        double result = 0.0;

        if (!calculate(currentValue, result)) {
            showError();
            return;
        }

        storedValue_ = result;
        showValue(result);
    } else if (pendingOperation_.isEmpty()) {
        storedValue_ = currentValue;
    }

    pendingOperation_ = button->text();
    waitingForNewValue_ = true;
}

void CalculatorWindow::slotEquals()
{
    if (
        errorState_
        || pendingOperation_.isEmpty()
        || waitingForNewValue_
    ) {
        return;
    }

    bool conversionOk = false;
    const double rightOperand = display_->text().toDouble(&conversionOk);
    double result = 0.0;

    if (!conversionOk || !calculate(rightOperand, result)) {
        showError();
        return;
    }

    showValue(result);
    storedValue_ = result;
    pendingOperation_.clear();
    waitingForNewValue_ = true;
}

void CalculatorWindow::slotOrdinaryUnaryOperationClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if (button == nullptr || errorState_) {
        return;
    }

    bool conversionOk = false;
    const double value = display_->text().toDouble(&conversionOk);

    if (!conversionOk) {
        showError();
        return;
    }

    double result = 0.0;

    if (button->text() == "sqrt") {
        if (value < 0.0) {
            showError();
            return;
        }

        result = std::sqrt(value);
    } else if (button->text() == "1/x") {
        if (value == 0.0) {
            showError();
            return;
        }

        result = 1.0 / value;
    } else {
        return;
    }

    showValue(result);
    waitingForNewValue_ = true;
}

void CalculatorWindow::slotTrigonometricOperationClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if (button == nullptr || errorState_) {
        return;
    }

    bool conversionOk = false;
    const double value = display_->text().toDouble(&conversionOk);

    if (!conversionOk) {
        showError();
        return;
    }

    double result = 0.0;

    if (button->text() == "sin") {
        result = std::sin(value);
    } else if (button->text() == "cos") {
        result = std::cos(value);
    } else if (button->text() == "tan") {
        result = std::tan(value);
    } else {
        return;
    }

    showValue(result);
    waitingForNewValue_ = true;
}

void CalculatorWindow::slotHyperbolicOperationClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if (button == nullptr || errorState_) {
        return;
    }

    bool conversionOk = false;
    const double value = display_->text().toDouble(&conversionOk);

    if (!conversionOk) {
        showError();
        return;
    }

    double result = 0.0;

    if (button->text() == "sinh") {
        result = std::sinh(value);
    } else if (button->text() == "cosh") {
        result = std::cosh(value);
    } else if (button->text() == "tanh") {
        result = std::tanh(value);
    } else {
        return;
    }

    showValue(result);
    waitingForNewValue_ = true;
}

void CalculatorWindow::showValue(double value)
{
    if (!std::isfinite(value)) {
        showError();
        return;
    }

    if (std::abs(value) < 1e-15) {
        value = 0.0;
    }

    display_->setText(QString::number(value, 'g', 15));
    errorState_ = false;
}

void CalculatorWindow::showError()
{
    display_->setText("Ошибка");
    waitingForNewValue_ = true;
    errorState_ = true;
    storedValue_ = 0.0;
    pendingOperation_.clear();
}

bool CalculatorWindow::calculate(double rightOperand, double& result) const
{
    if (pendingOperation_ == "+") {
        result = storedValue_ + rightOperand;
    } else if (pendingOperation_ == "-") {
        result = storedValue_ - rightOperand;
    } else if (pendingOperation_ == "*") {
        result = storedValue_ * rightOperand;
    } else if (pendingOperation_ == "/") {
        if (rightOperand == 0.0) {
            return false;
        }

        result = storedValue_ / rightOperand;
    } else {
        return false;
    }

    return std::isfinite(result);
}

