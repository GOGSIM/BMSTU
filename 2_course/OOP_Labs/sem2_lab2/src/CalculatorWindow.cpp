#include "CalculatorWindow.h"

#include <QFontDatabase>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

CalculatorWindow::CalculatorWindow(QWidget* parent)
    : QWidget(parent),
      display_(new QLineEdit(this)),
      ordinaryModeButton_(new QRadioButton("Обычный", this)),
      engineeringModeButton_(new QRadioButton("Инженерный", this)),
      clearButton_(new QPushButton("C", this)),
      buttonPanelsLayout_(new QHBoxLayout)
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

    buttonPanelsLayout_->addWidget(ordinaryPanel);

    setLayout(mainLayout);
}

