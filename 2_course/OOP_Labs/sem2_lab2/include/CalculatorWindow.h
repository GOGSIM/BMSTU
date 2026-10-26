// pragma once не даёт подключить этот заголовочный файл дважды.
#pragma once

#include <QString>
#include <QWidget>

// Предварительные объявления позволяют использовать указатели на классы Qt,
// не подключая полные заголовочные файлы в этом файле.
class QLineEdit;
class QHBoxLayout;
class QKeyEvent;
class QPushButton;
class QRadioButton;

// После двоеточия указан базовый класс: окно калькулятора является QWidget.
class CalculatorWindow : public QWidget
{
    // Q_OBJECT добавляет поддержку сигналов, слотов и других возможностей Qt.
    Q_OBJECT

public:
    // public: эта часть доступна коду, который создаёт окно.
    // explicit запрещает неявно превращать QWidget* в CalculatorWindow.
    // nullptr означает, что по умолчанию у окна нет родительского виджета.
    explicit CalculatorWindow(QWidget* parent = nullptr);

protected:
    // protected: метод доступен самому классу и его возможным наследникам.
    // override показывает, что метод переопределяет обработчик QWidget.
    // Qt вызывает его при нажатии клавиши, пока активно окно калькулятора.
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    // private: напрямую эти обработчики используются только внутри класса.
    // Слоты — методы, которые Qt вызывает в ответ на сигналы элементов окна.
    // Переключает обычный и инженерный режимы без сброса вычисления.
    void slotModeChanged();

    // Добавляет на дисплей цифру нажатой кнопки.
    void slotDigitClicked();

    // Добавляет десятичную точку, если её ещё нет в числе.
    void slotPointClicked();

    // Полностью очищает дисплей и сохранённое состояние вычисления.
    void slotClear();

    // Запоминает первую часть бинарной операции: +, -, *, /, xʸ или ʸ√x.
    void slotBinaryOperationClicked();

    // Завершает ожидающую бинарную операцию и показывает результат.
    void slotEquals();

    // Выполняет sqrt или 1/x над текущим числом.
    void slotOrdinaryUnaryOperationClicked();

    // Выполняет sin, cos или tan; аргумент задаётся в радианах.
    void slotTrigonometricOperationClicked();

    // Выполняет sinh, cosh или tanh.
    void slotHyperbolicOperationClicked();

    // Выполняет exp, натуральный ln или десятичный log.
    void slotLogarithmicOperationClicked();

    // Возводит число в квадрат/куб или извлекает кубический корень.
    void slotPowerOperationClicked();

    // Обрабатывает факториал и ввод числа π.
    void slotSpecialOperationClicked();

private:
    // В private также находятся внутренние функции и данные калькулятора.
    // Выводит корректное число на дисплей с ограниченной точностью.
    void showValue(double value);

    // Показывает ошибку и сбрасывает незавершённую операцию.
    void showError();

    // Вычисляет ожидающую бинарную операцию.
    // Символ & перед result означает передачу по ссылке: метод записывает
    // ответ прямо в переменную вызывающего кода. const запрещает менять поля.
    bool calculate(double rightOperand, double& result) const;

    // Находит видимую кнопку по тексту и программно нажимает её.
    // const QString& передаёт строку без копирования и запрещает её менять.
    bool clickButton(const QString& text);

    // Указатели хранят адреса элементов интерфейса, которыми владеет Qt.
    QLineEdit* display_;
    QRadioButton* ordinaryModeButton_;
    QRadioButton* engineeringModeButton_;
    QPushButton* clearButton_;
    QHBoxLayout* buttonPanelsLayout_;
    QWidget* engineeringPanel_;

    // Состояние текущего вычисления.
    // bool хранит только true/false, double — вещественное число.
    bool waitingForNewValue_;
    bool errorState_;
    double storedValue_;
    // QString хранит текстовое обозначение ожидающей операции.
    QString pendingOperation_;
};
