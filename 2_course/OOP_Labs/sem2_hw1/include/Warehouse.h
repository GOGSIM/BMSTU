// Защита от повторного подключения одного заголовочного файла.
#ifndef WAREHOUSE_H
#define WAREHOUSE_H

// Стандартные типы и контейнеры, используемые складской частью.
#include <cstddef>
#include <map>
#include <string>
#include <vector>

// Пространство имён объединяет всё, что относится к складу.
namespace warehouse
{

// const запрещает менять эти общие имена файлов во время работы программы.
const std::string WAREHOUSE_FILE_NAME = "warehouse.dat";
const std::string TEMPORARY_FILE_NAME = "warehouse.tmp";
const std::string SEARCH_RESULTS_FILE_NAME = "search_results.dat";
const std::string REPORT_FILE_NAME = "warehouse_report.txt";
const std::size_t PRODUCT_NAME_SIZE = 64;

// Простая структура календарной даты.
struct Date
{
    int day;   // День месяца.
    int month; // Номер месяца.
    int year;  // Год.
};

// Одна складская запись. Динамических полей здесь нет, поэтому объект можно
// целиком записать в бинарный файл одним вызовом write.
struct ProductRecord
{
    std::size_t index;                         // Уникальный индекс записи.
    int warehouseNumber;                      // Номер склада.
    int productCode;                          // Код товара.
    char productName[PRODUCT_NAME_SIZE];       // Название фиксированной длины.
    Date arrivalDate;                         // Дата поступления.
    int shelfLifeDays;                        // Срок хранения в днях.
    int quantity;                             // Количество единиц.
    double unitPrice;                         // Цена одной единицы.
};

// typedef создаёт короткое имя для типа карты складских записей.
typedef std::map<std::size_t, ProductRecord> RecordMap;

// Функции для проверки дат и определения просрочки.
bool isLeapYear(int year);
int daysInMonth(int month, int year);
bool isValidDate(const Date& date);
long long dateToDays(const Date& date);
int compareDates(const Date& left, const Date& right);
bool isExpired(const ProductRecord& record, const Date& controlDate);

// Класс загружает карту, изменяет её и синхронизирует с бинарным файлом.
class WarehouseStorage
{
public:
    // explicit запрещает неявно превращать строку в объект WarehouseStorage.
    explicit WarehouseStorage(const std::string& fileName = WAREHOUSE_FILE_NAME);

    // Загружает целые ProductRecord из бинарного файла в std::map.
    void load();
    // const после скобок обещает не изменять объект, а & возвращает карту без копии.
    const RecordMap& records() const;
    // Добавляет, удаляет или заменяет запись.
    std::size_t add(ProductRecord record);
    bool remove(std::size_t index);
    bool replace(std::size_t index, ProductRecord record);
    // Возвращают все записи, которые подходят под условие поиска.
    std::vector<ProductRecord> findByWarehouseNumber(int warehouseNumber) const;
    std::vector<ProductRecord> findByProductCode(int productCode) const;
    std::vector<ProductRecord> findByArrivalDate(const Date& date) const;
    std::vector<ProductRecord> findExpired(const Date& controlDate) const;
    std::vector<ProductRecord> findNotExpired(const Date& controlDate) const;
    // Сохраняет результаты поиска целыми бинарными объектами.
    void saveSearchResults(const std::vector<ProductRecord>& results) const;
    // Создаёт текстовую ведомость и считает общую стоимость.
    void createReport() const;

private:
    // private-методы используются только внутри WarehouseStorage.
    std::size_t nextIndex() const;
    void appendRecord(const ProductRecord& record) const;
    void rewriteFile(const RecordMap& updatedRecords) const;

    std::string fileName_; // Имя основного бинарного файла.
    RecordMap records_;    // Индексный массив в оперативной памяти.
};

// Запускает подменю второй части задания.
void runMenu();

}

#endif
