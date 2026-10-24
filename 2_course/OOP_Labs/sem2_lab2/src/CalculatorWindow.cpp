#include "CalculatorWindow.h"

#include <QFontDatabase>
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
      clearButton_(new QPushButton("C", this))
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

    setLayout(mainLayout);
}

