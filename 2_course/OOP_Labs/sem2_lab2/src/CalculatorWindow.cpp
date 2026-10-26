#include "CalculatorWindow.h"

#include <cmath>

#include <QFontDatabase>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLayout>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

CalculatorWindow::CalculatorWindow(QWidget* parent)
    // После двоеточия идёт список инициализации: базовый класс и поля
    // создаются сразу с нужными значениями до выполнения тела конструктора.
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
    // objectName позволяет обратиться именно к главному окну в QSS через
    // селектор QWidget#calculatorWindow, не меняя фон всех дочерних виджетов.
    setObjectName("calculatorWindow");
    setWindowTitle("Калькулятор (обычный)");

    // Берём системный моноширинный шрифт, чтобы цифры не меняли ширину.
    QFont displayFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    displayFont.setPointSize(24);

    // Стрелка -> вызывает метод объекта, адрес которого хранится в указателе.
    // Пользователь видит дисплей, но не может редактировать его напрямую.
    // Имя display используется ниже, чтобы задать экрану отдельные цвета.
    display_->setObjectName("display");
    display_->setText("0");
    display_->setReadOnly(true);
    display_->setAlignment(Qt::AlignRight);
    display_->setFont(displayFont);
    display_->setMinimumHeight(58);
    display_->setMaxLength(24);

    ordinaryModeButton_->setChecked(true);

    // Компоновщики автоматически размещают элементы без ручных координат.
    // setSpacing задаёт одинаковое расстояние между соседними элементами.
    QVBoxLayout* modeButtonsLayout = new QVBoxLayout;
    modeButtonsLayout->setSpacing(4);
    modeButtonsLayout->addWidget(ordinaryModeButton_);
    modeButtonsLayout->addWidget(engineeringModeButton_);

    QHBoxLayout* modeRowLayout = new QHBoxLayout;
    modeRowLayout->setSpacing(10);
    modeRowLayout->addLayout(modeButtonsLayout);
    modeRowLayout->addStretch();
    modeRowLayout->addWidget(clearButton_);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(display_);
    mainLayout->addLayout(modeRowLayout);
    mainLayout->addLayout(buttonPanelsLayout_);

    // parent передаётся в конструктор виджета: Qt сам удалит дочерний объект.
    QWidget* ordinaryPanel = new QWidget(this);
    QGridLayout* ordinaryLayout = new QGridLayout(ordinaryPanel);
    ordinaryLayout->setSpacing(6);

    // Создаём кнопки обычного режима и задаём их текст.
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
    // По имени equalsButton QSS выделяет кнопку результата синим цветом.
    equalsButton->setObjectName("equalsButton");

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

    // QList — шаблонный контейнер Qt. Тип внутри <...> говорит, что список
    // хранит указатели именно на QPushButton.
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

    // Все цифровые кнопки посылают сигнал clicked одному общему слоту.
    for (QPushButton* button : digitButtons) {
        // В Qt 6 полная сигнатура сигнала — clicked(bool). Параметр сообщает,
        // отмечена ли кнопка, но обычному QPushButton и нашему слоту он не нужен.
        connect(button, SIGNAL(clicked(bool)), this, SLOT(slotDigitClicked()));
    }

    connect(pointButton, SIGNAL(clicked(bool)), this, SLOT(slotPointClicked()));

    const QList<QPushButton*> binaryOperationButtons = {
        addButton,
        subtractButton,
        multiplyButton,
        divideButton
    };

    // Один обработчик различает операции по тексту на нажатой кнопке.
    for (QPushButton* button : binaryOperationButtons) {
        connect(
            button,
            SIGNAL(clicked(bool)),
            this,
            SLOT(slotBinaryOperationClicked())
        );
    }

    connect(equalsButton, SIGNAL(clicked(bool)), this, SLOT(slotEquals()));
    connect(
        squareRootButton,
        SIGNAL(clicked(bool)),
        this,
        SLOT(slotOrdinaryUnaryOperationClicked())
    );
    connect(
        reciprocalButton,
        SIGNAL(clicked(bool)),
        this,
        SLOT(slotOrdinaryUnaryOperationClicked())
    );

    // Инженерные кнопки находятся на отдельной панели: так её можно целиком
    // показать или скрыть при переключении режима.
    QGridLayout* engineeringLayout = new QGridLayout(engineeringPanel_);
    engineeringLayout->setSpacing(6);

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

    // findChildren<T>() — шаблонный метод. В угловых скобках указан тип
    // дочерних объектов, которые нужно найти на инженерной панели.
    const QList<QPushButton*> engineeringButtons =
        engineeringPanel_->findChildren<QPushButton*>();

    // Подключаем каждую группу операций к своему общему слоту.
    for (QPushButton* button : engineeringButtons) {
        if (
            button->text() == "sin"
            || button->text() == "cos"
            || button->text() == "tan"
        ) {
            connect(
                button,
                SIGNAL(clicked(bool)),
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
                SIGNAL(clicked(bool)),
                this,
                SLOT(slotHyperbolicOperationClicked())
            );
        }

        if (
            button->text() == "exp"
            || button->text() == "ln"
            || button->text() == "log"
        ) {
            connect(
                button,
                SIGNAL(clicked(bool)),
                this,
                SLOT(slotLogarithmicOperationClicked())
            );
        }

        if (
            button->text() == "x²"
            || button->text() == "x³"
            || button->text() == "³√x"
        ) {
            connect(
                button,
                SIGNAL(clicked(bool)),
                this,
                SLOT(slotPowerOperationClicked())
            );
        }

        if (
            button->text() == "xʸ"
            || button->text() == "ʸ√x"
        ) {
            connect(
                button,
                SIGNAL(clicked(bool)),
                this,
                SLOT(slotBinaryOperationClicked())
            );
        }

        if (
            button->text() == "n!"
            || button->text() == "π"
        ) {
            connect(
                button,
                SIGNAL(clicked(bool)),
                this,
                SLOT(slotSpecialOperationClicked())
            );
        }
    }

    engineeringPanel_->hide();

    // Инженерная панель стоит слева, обычная клавиатура — справа.
    buttonPanelsLayout_->setSpacing(12);
    buttonPanelsLayout_->addWidget(engineeringPanel_);
    buttonPanelsLayout_->addWidget(ordinaryPanel);

    connect(
        engineeringModeButton_,
        SIGNAL(toggled(bool)),
        this,
        SLOT(slotModeChanged())
    );
    connect(clearButton_, SIGNAL(clicked(bool)), this, SLOT(slotClear()));

    // Шаблонный вызов ищет все кнопки среди дочерних объектов окна.
    const QList<QPushButton*> allButtons = findChildren<QPushButton*>();

    // Кнопки не забирают фокус, поэтому ввод с клавиатуры получает окно.
    for (QPushButton* button : allButtons) {
        button->setFocusPolicy(Qt::NoFocus);
    }

    ordinaryModeButton_->setFocusPolicy(Qt::NoFocus);
    engineeringModeButton_->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);

    // По имени clearButton QSS отдельно выделяет очистку красным цветом.
    clearButton_->setObjectName("clearButton");

    // QSS похож на CSS и задаёт общий внешний вид элементов Qt Widgets.
    // Цвет текста задаётся явно, поэтому интерфейс одинаково читается при
    // светлой и тёмной системной теме. Символ # в селекторе означает поиск
    // элемента по objectName, установленному выше.
    setStyleSheet(
        "QWidget#calculatorWindow {"
        "    background-color: #202733;"
        "}"
        "QWidget {"
        "    font-size: 14px;"
        "}"
        "QLineEdit#display {"
        "    color: #111827;"
        "    background-color: #f8fafc;"
        "    border: 2px solid #64748b;"
        "    border-radius: 6px;"
        "    padding: 8px;"
        "    selection-color: white;"
        "    selection-background-color: #2563eb;"
        "}"
        "QPushButton {"
        "    color: #f8fafc;"
        "    min-width: 54px;"
        "    min-height: 42px;"
        "    background-color: #374151;"
        "    border: 1px solid #64748b;"
        "    border-radius: 6px;"
        "    font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "    background-color: #4b5563;"
        "    border-color: #94a3b8;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1f2937;"
        "}"
        "QPushButton#clearButton {"
        "    background-color: #b91c1c;"
        "    border-color: #ef4444;"
        "}"
        "QPushButton#clearButton:hover {"
        "    background-color: #dc2626;"
        "}"
        "QPushButton#equalsButton {"
        "    background-color: #1d4ed8;"
        "    border-color: #60a5fa;"
        "}"
        "QPushButton#equalsButton:hover {"
        "    background-color: #2563eb;"
        "}"
        "QRadioButton {"
        "    color: #f8fafc;"
        "    spacing: 8px;"
        "}"
    );

    // Передаём окну главный компоновщик и фиксируем вычисленный им размер.
    setLayout(mainLayout);
    setFixedSize(sizeHint());
}

void CalculatorWindow::keyPressEvent(QKeyEvent* event)
{
    // Коды цифр идут подряд, поэтому вычитание Key_0 даёт число от 0 до 9.
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {
        const int digit = event->key() - Qt::Key_0;
        // Клавиатура запускает ту же кнопку, что и мышь: логика не дублируется.
        clickButton(QString::number(digit));
        event->accept();
        return;
    }

    // switch выбирает действие по коду специальной клавиши.
    switch (event->key()) {
    case Qt::Key_Period:
        clickButton(".");
        break;
    case Qt::Key_Plus:
        clickButton("+");
        break;
    case Qt::Key_Minus:
        clickButton("-");
        break;
    case Qt::Key_Asterisk:
        clickButton("*");
        break;
    case Qt::Key_Slash:
        clickButton("/");
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        // Два case подряд обрабатывают обе клавиши Enter одинаково.
        clickButton("=");
        break;
    case Qt::Key_Escape:
        clickButton("C");
        break;
    default:
        // Неизвестную клавишу передаём стандартному обработчику QWidget.
        QWidget::keyPressEvent(event);
        return;
    }

    event->accept();
}

void CalculatorWindow::slotModeChanged()
{
    const bool engineeringMode = engineeringModeButton_->isChecked();

    // Панель меняет видимость, а состояние текущего вычисления сохраняется.
    engineeringPanel_->setVisible(engineeringMode);
    // Условный оператор ?: выбирает один из двух заголовков окна.
    setWindowTitle(
        engineeringMode
            ? "Калькулятор (инженерный)"
            : "Калькулятор (обычный)"
    );

    // После изменения видимости заново вычисляем подходящий размер окна.
    layout()->activate();
    setFixedSize(sizeHint());
}

void CalculatorWindow::slotDigitClicked()
{
    // sender() возвращает объект, который послал сигнал. qobject_cast<T>() —
    // безопасное Qt-приведение к типу из угловых скобок.
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    // При неудачном приведении qobject_cast возвращает nullptr.
    if (button == nullptr) {
        return;
    }

    // Новая цифра заменяет ноль, предыдущий результат или сообщение об ошибке.
    if (waitingForNewValue_ || errorState_ || display_->text() == "0") {
        display_->setText(button->text());
        waitingForNewValue_ = false;
        errorState_ = false;
        return;
    }

    // Иначе дописываем цифру справа к уже введённому тексту.
    display_->setText(display_->text() + button->text());
}

void CalculatorWindow::slotPointClicked()
{
    // Новое дробное число начинаем с понятной записи "0.".
    if (waitingForNewValue_ || errorState_) {
        display_->setText("0.");
        waitingForNewValue_ = false;
        errorState_ = false;
        return;
    }

    // contains не позволяет добавить в одно число вторую точку.
    if (!display_->text().contains('.')) {
        display_->setText(display_->text() + '.');
    }
}

void CalculatorWindow::slotClear()
{
    // Возвращаем и интерфейс, и внутреннее состояние к начальному виду.
    display_->setText("0");
    waitingForNewValue_ = true;
    errorState_ = false;
    storedValue_ = 0.0;
    pendingOperation_.clear();
}

void CalculatorWindow::slotBinaryOperationClicked()
{
    // Получаем конкретную кнопку, хотя все бинарные операции используют слот.
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if (button == nullptr || errorState_) {
        return;
    }

    bool conversionOk = false;
    // toDouble записывает в conversionOk, получилось ли преобразовать текст.
    const double currentValue = display_->text().toDouble(&conversionOk);

    if (!conversionOk) {
        showError();
        return;
    }

    // Если операция уже ожидалась, сначала завершаем предыдущую. Благодаря
    // этому выражение 2 + 3 + сразу показывает промежуточный результат 5.
    if (!pendingOperation_.isEmpty() && !waitingForNewValue_) {
        double result = 0.0;

        if (!calculate(currentValue, result)) {
            showError();
            return;
        }

        storedValue_ = result;
        showValue(result);
    } else if (pendingOperation_.isEmpty()) {
        // Для первой операции запоминаем число слева от оператора.
        storedValue_ = currentValue;
    }

    // Текст кнопки служит простым обозначением ожидающей операции.
    pendingOperation_ = button->text();
    waitingForNewValue_ = true;
}

void CalculatorWindow::slotEquals()
{
    // Без двух операндов и операции вычислять нечего.
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

    // calculate возвращает false при недопустимой операции.
    if (!conversionOk || !calculate(rightOperand, result)) {
        showError();
        return;
    }

    // После "=" результат остаётся на дисплее и может стать новым операндом.
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

    // Унарная операция использует только текущее число с дисплея.
    if (button->text() == "sqrt") {
        // В вещественных числах квадратный корень отрицательного не определён.
        if (value < 0.0) {
            showError();
            return;
        }

        result = std::sqrt(value);
    } else if (button->text() == "1/x") {
        // Деление на ноль запрещено.
        if (value == 0.0) {
            showError();
            return;
        }

        result = 1.0 / value;
    } else {
        return;
    }

    showValue(result);
    // Если бинарная операция не ждёт второй операнд, следующий ввод заменит
    // результат; иначе результат остаётся вторым операндом этой операции.
    waitingForNewValue_ = pendingOperation_.isEmpty();
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

    // Функции std::sin, std::cos и std::tan принимают угол в радианах.
    // Префикс std:: указывает, что функции находятся в стандартном пространстве имён.
    if (button->text() == "sin") {
        result = std::sin(value);
    } else if (button->text() == "cos") {
        result = std::cos(value);
    } else if (button->text() == "tan") {
        // tan не существует, когда cos близок к нулю. Сравнение выполняется
        // с малой погрешностью, потому что double хранит числа приближённо.
        if (std::abs(std::cos(value)) < 1e-12) {
            showError();
            return;
        }

        result = std::tan(value);
    } else {
        return;
    }

    showValue(result);
    waitingForNewValue_ = pendingOperation_.isEmpty();
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

    // Выбираем гиперболическую функцию по надписи общей кнопки-отправителя.
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
    waitingForNewValue_ = pendingOperation_.isEmpty();
}

void CalculatorWindow::slotLogarithmicOperationClicked()
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

    // exp вычисляет e в степени текущего числа.
    if (button->text() == "exp") {
        result = std::exp(value);
    } else if (button->text() == "ln") {
        // Оба вещественных логарифма определены только для чисел больше нуля.
        if (value <= 0.0) {
            showError();
            return;
        }

        result = std::log(value);
    } else if (button->text() == "log") {
        if (value <= 0.0) {
            showError();
            return;
        }

        result = std::log10(value);
    } else {
        return;
    }

    showValue(result);
    waitingForNewValue_ = pendingOperation_.isEmpty();
}

void CalculatorWindow::slotPowerOperationClicked()
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

    // Для небольших целых степеней достаточно обычного умножения.
    if (button->text() == "x²") {
        result = value * value;
    } else if (button->text() == "x³") {
        result = value * value * value;
    } else if (button->text() == "³√x") {
        // cbrt корректно извлекает кубический корень и из отрицательного числа.
        result = std::cbrt(value);
    } else {
        return;
    }

    showValue(result);
    waitingForNewValue_ = pendingOperation_.isEmpty();
}

void CalculatorWindow::slotSpecialOperationClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if (button == nullptr) {
        return;
    }

    if (button->text() == "π") {
        // arccos(-1) равен π и не требует отдельной константы платформы.
        showValue(std::acos(-1.0));
        waitingForNewValue_ = pendingOperation_.isEmpty();
        return;
    }

    if (button->text() != "n!" || errorState_) {
        return;
    }

    bool conversionOk = false;
    const double value = display_->text().toDouble(&conversionOk);
    const double integerValue = std::round(value);

    // Факториал определён для целых неотрицательных чисел. После 170
    // результат уже не помещается в тип double.
    if (
        !conversionOk
        || value < 0.0
        || value > 170.0
        || std::abs(value - integerValue) >= 1e-12
    ) {
        showError();
        return;
    }

    double result = 1.0;

    // static_cast<int> — явное преобразование double в int после проверки,
    // что значение действительно целое и находится в допустимом диапазоне.
    for (int number = 2; number <= static_cast<int>(integerValue); ++number) {
        result *= number;
    }

    showValue(result);
    waitingForNewValue_ = pendingOperation_.isEmpty();
}

void CalculatorWindow::showValue(double value)
{
    // isfinite отсеивает бесконечность и NaN (неопределённый числовой результат).
    if (!std::isfinite(value)) {
        showError();
        return;
    }

    // Очень малое значение выводим как 0, а не как погрешность вида 1e-16.
    if (std::abs(value) < 1e-15) {
        value = 0.0;
    }

    // Формат 'g' выбирает обычную или научную запись, 15 — число значащих цифр.
    display_->setText(QString::number(value, 'g', 15));
    errorState_ = false;
}

void CalculatorWindow::showError()
{
    // После ошибки любая цифра начинает новое вычисление с чистого состояния.
    display_->setText("Ошибка");
    waitingForNewValue_ = true;
    errorState_ = true;
    storedValue_ = 0.0;
    pendingOperation_.clear();
}

bool CalculatorWindow::calculate(double rightOperand, double& result) const
{
    // storedValue_ — левый операнд, rightOperand — правый.
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
    } else if (pendingOperation_ == "xʸ") {
        // pow возводит сохранённое основание в введённую степень.
        result = std::pow(storedValue_, rightOperand);
    } else if (pendingOperation_ == "ʸ√x") {
        // Нулевая степень корня привела бы к делению 1 / 0.
        if (rightOperand == 0.0) {
            return false;
        }

        if (storedValue_ < 0.0) {
            // Из отрицательного числа вещественный корень существует только
            // при целой нечётной степени.
            const double integerDegree = std::round(rightOperand);
            const bool isIntegerDegree =
                std::abs(rightOperand - integerDegree) < 1e-12;
            const bool isOddDegree =
                std::fmod(std::abs(integerDegree), 2.0) == 1.0;

            if (!isIntegerDegree || !isOddDegree) {
                return false;
            }

            // Для отрицательного числа отдельно считаем модуль и возвращаем знак.
            result = -std::pow(-storedValue_, 1.0 / rightOperand);
        } else {
            result = std::pow(storedValue_, 1.0 / rightOperand);
        }
    } else {
        return false;
    }

    // Даже допустимая формула может выйти за диапазон double.
    return std::isfinite(result);
}

bool CalculatorWindow::clickButton(const QString& text)
{
    const QList<QPushButton*> buttons = findChildren<QPushButton*>();

    for (QPushButton* button : buttons) {
        // Скрытые инженерные кнопки недоступны в обычном режиме.
        if (button->text() == text && button->isVisible()) {
            // click программно посылает тот же сигнал clicked, что и мышь.
            button->click();
            return true;
        }
    }

    return false;
}
