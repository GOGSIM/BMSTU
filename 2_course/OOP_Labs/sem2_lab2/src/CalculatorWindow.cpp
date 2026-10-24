#include "CalculatorWindow.h"

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
      errorState_(false)
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
}

