#include "Warehouse.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace warehouse
{

namespace
{

template <typename Predicate>
std::vector<ProductRecord> findRecords(const RecordMap& records, Predicate predicate)
{
    std::vector<ProductRecord> result;
    RecordMap::const_iterator current = records.begin();

    while (current != records.end())
    {
        current = std::find_if(
            current, records.end(),
            [&predicate](const RecordMap::value_type& pair)
            {
                return predicate(pair.second);
            });

        if (current != records.end())
        {
            result.push_back(current->second);
            ++current;
        }
    }

    return result;
}

template <typename Value>
Value readValue(const std::string& prompt)
{
    Value value{};

    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value)
        {
            return value;
        }

        if (std::cin.eof())
        {
            throw std::runtime_error("Input stream is closed.");
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Incorrect input. Try again.\n";
    }
}

template <typename Value>
Value readNonNegativeValue(const std::string& prompt)
{
    while (true)
    {
        Value value = readValue<Value>(prompt);
        if (value >= 0)
        {
            return value;
        }

        std::cout << "The value cannot be negative.\n";
    }
}

std::size_t readIndex(const std::string& prompt)
{
    long long value = readNonNegativeValue<long long>(prompt);
    return static_cast<std::size_t>(value);
}

Date readDate(const std::string& title)
{
    while (true)
    {
        std::cout << title << '\n';
        Date date{};
        date.day = readValue<int>("Day: ");
        date.month = readValue<int>("Month: ");
        date.year = readValue<int>("Year: ");

        if (isValidDate(date))
        {
            return date;
        }

        std::cout << "Incorrect date. Enter it again.\n";
    }
}

std::string readProductName()
{
    while (true)
    {
        std::cout << "Product name: ";
        std::string name;
        std::getline(std::cin >> std::ws, name);

        if (std::cin.eof() && name.empty())
        {
            throw std::runtime_error("Input stream is closed.");
        }

        if (name.empty())
        {
            std::cout << "The name cannot be empty.\n";
        }
        else if (name.size() >= PRODUCT_NAME_SIZE)
        {
            std::cout << "The name must contain at most "
                      << PRODUCT_NAME_SIZE - 1 << " characters.\n";
        }
        else
        {
            return name;
        }
    }
}

ProductRecord readProductRecord()
{
    ProductRecord record{};
    record.warehouseNumber = readNonNegativeValue<int>("Warehouse number: ");
    record.productCode = readNonNegativeValue<int>("Product code: ");

    const std::string name = readProductName();
    std::memcpy(record.productName, name.c_str(), name.size() + 1);

    record.arrivalDate = readDate("Arrival date");
    record.shelfLifeDays = readNonNegativeValue<int>("Shelf life in days: ");
    record.quantity = readNonNegativeValue<int>("Quantity: ");
    record.unitPrice = readNonNegativeValue<double>("Unit price: ");
    return record;
}

void printRecord(const ProductRecord& record)
{
    std::cout << "Index: " << record.index
              << ", warehouse: " << record.warehouseNumber
              << ", code: " << record.productCode
              << ", name: " << record.productName
              << ", arrival: "
              << std::setfill('0') << std::setw(2) << record.arrivalDate.day << '.'
              << std::setw(2) << record.arrivalDate.month << '.'
              << std::setw(4) << record.arrivalDate.year
              << std::setfill(' ')
              << ", shelf life: " << record.shelfLifeDays
              << ", quantity: " << record.quantity
              << ", unit price: " << std::fixed << std::setprecision(2)
              << record.unitPrice << '\n';
}

void printRecords(const RecordMap& records)
{
    if (records.empty())
    {
        std::cout << "There are no warehouse records.\n";
        return;
    }

    for (RecordMap::const_iterator iterator = records.begin();
         iterator != records.end(); ++iterator)
    {
        std::cout << "Map key " << iterator->first << ": ";
        printRecord(iterator->second);
    }
}

void printAndSaveSearchResults(
    const WarehouseStorage& storage,
    const std::vector<ProductRecord>& results)
{
    if (results.empty())
    {
        std::cout << "No matching records found.\n";
    }
    else
    {
        std::for_each(results.begin(), results.end(), printRecord);
    }

    storage.saveSearchResults(results);
    std::cout << "Found records: " << results.size()
              << ". Results are saved to search_results.dat.\n";
}

}

static_assert(std::is_trivially_copyable<ProductRecord>::value,
              "ProductRecord must be written as one binary object");

bool isLeapYear(int year)
{
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

int daysInMonth(int month, int year)
{
    const int days[] = {31, 28, 31, 30, 31, 30,
                        31, 31, 30, 31, 30, 31};

    if (month < 1 || month > 12)
    {
        return 0;
    }

    if (month == 2 && isLeapYear(year))
    {
        return 29;
    }

    return days[month - 1];
}

bool isValidDate(const Date& date)
{
    return date.year > 0 && date.month >= 1 && date.month <= 12 &&
           date.day >= 1 && date.day <= daysInMonth(date.month, date.year);
}

long long dateToDays(const Date& date)
{
    long long previousYear = static_cast<long long>(date.year) - 1;
    long long result = previousYear * 365 + previousYear / 4 -
                       previousYear / 100 + previousYear / 400;

    for (int month = 1; month < date.month; ++month)
    {
        result += daysInMonth(month, date.year);
    }

    return result + date.day;
}

int compareDates(const Date& left, const Date& right)
{
    long long leftDays = dateToDays(left);
    long long rightDays = dateToDays(right);

    if (leftDays < rightDays)
    {
        return -1;
    }
    if (leftDays > rightDays)
    {
        return 1;
    }
    return 0;
}

bool isExpired(const ProductRecord& record, const Date& controlDate)
{
    long long expirationDay = dateToDays(record.arrivalDate) + record.shelfLifeDays;
    return dateToDays(controlDate) > expirationDay;
}

WarehouseStorage::WarehouseStorage(const std::string& fileName)
    : fileName_(fileName)
{
}

void WarehouseStorage::load()
{
    records_.clear();

    std::ifstream input(fileName_, std::ios::binary);
    if (!input)
    {
        std::ofstream newFile(fileName_, std::ios::binary);
        if (!newFile)
        {
            throw std::runtime_error("Cannot create warehouse file.");
        }
        return;
    }

    ProductRecord record{};
    while (input.read(reinterpret_cast<char*>(&record), sizeof(ProductRecord)))
    {
        record.productName[PRODUCT_NAME_SIZE - 1] = '\0';
        if (!records_.insert(std::make_pair(record.index, record)).second)
        {
            throw std::runtime_error("Duplicate record index in warehouse file.");
        }
    }

    if (!input.eof())
    {
        throw std::runtime_error("Warehouse file contains an incomplete record.");
    }
}

const RecordMap& WarehouseStorage::records() const
{
    return records_;
}

std::size_t WarehouseStorage::nextIndex() const
{
    if (records_.empty())
    {
        return 1;
    }

    return records_.rbegin()->first + 1;
}

void WarehouseStorage::appendRecord(const ProductRecord& record) const
{
    std::ofstream output(fileName_, std::ios::binary | std::ios::app);
    if (!output)
    {
        throw std::runtime_error("Cannot open warehouse file for appending.");
    }

    output.write(reinterpret_cast<const char*>(&record), sizeof(ProductRecord));
    if (!output)
    {
        throw std::runtime_error("Cannot append a record to warehouse file.");
    }
}

std::size_t WarehouseStorage::add(ProductRecord record)
{
    record.index = nextIndex();
    appendRecord(record);
    records_.insert(std::make_pair(record.index, record));
    return record.index;
}

void WarehouseStorage::rewriteFile(const RecordMap& updatedRecords) const
{
    std::ofstream output(TEMPORARY_FILE_NAME, std::ios::binary | std::ios::trunc);
    if (!output)
    {
        throw std::runtime_error("Cannot create temporary warehouse file.");
    }

    for (RecordMap::const_iterator iterator = updatedRecords.begin();
         iterator != updatedRecords.end(); ++iterator)
    {
        output.write(reinterpret_cast<const char*>(&iterator->second),
                     sizeof(ProductRecord));
        if (!output)
        {
            throw std::runtime_error("Cannot write a record to temporary file.");
        }
    }

    output.close();
    if (!output)
    {
        throw std::runtime_error("Cannot close temporary warehouse file.");
    }

    if (std::remove(fileName_.c_str()) != 0)
    {
        throw std::runtime_error("Cannot remove the old warehouse file.");
    }

    if (std::rename(TEMPORARY_FILE_NAME.c_str(), fileName_.c_str()) != 0)
    {
        throw std::runtime_error(
            "Cannot replace warehouse file. Data remains in the temporary file.");
    }
}

bool WarehouseStorage::remove(std::size_t index)
{
    if (records_.find(index) == records_.end())
    {
        return false;
    }

    RecordMap updatedRecords = records_;
    updatedRecords.erase(index);
    rewriteFile(updatedRecords);
    records_ = updatedRecords;
    return true;
}

bool WarehouseStorage::replace(std::size_t index, ProductRecord record)
{
    RecordMap::const_iterator found = records_.find(index);
    if (found == records_.end())
    {
        return false;
    }

    record.index = index;
    RecordMap updatedRecords = records_;
    updatedRecords[index] = record;
    rewriteFile(updatedRecords);
    records_ = updatedRecords;
    return true;
}

std::vector<ProductRecord> WarehouseStorage::findByWarehouseNumber(
    int warehouseNumber) const
{
    return findRecords(records_, [warehouseNumber](const ProductRecord& record)
    {
        return record.warehouseNumber == warehouseNumber;
    });
}

std::vector<ProductRecord> WarehouseStorage::findByProductCode(int productCode) const
{
    return findRecords(records_, [productCode](const ProductRecord& record)
    {
        return record.productCode == productCode;
    });
}

std::vector<ProductRecord> WarehouseStorage::findByArrivalDate(const Date& date) const
{
    return findRecords(records_, [&date](const ProductRecord& record)
    {
        return compareDates(record.arrivalDate, date) == 0;
    });
}

std::vector<ProductRecord> WarehouseStorage::findExpired(
    const Date& controlDate) const
{
    return findRecords(records_, [&controlDate](const ProductRecord& record)
    {
        return isExpired(record, controlDate);
    });
}

std::vector<ProductRecord> WarehouseStorage::findNotExpired(
    const Date& controlDate) const
{
    return findRecords(records_, [&controlDate](const ProductRecord& record)
    {
        return !isExpired(record, controlDate);
    });
}

void WarehouseStorage::saveSearchResults(
    const std::vector<ProductRecord>& results) const
{
    std::ofstream output(SEARCH_RESULTS_FILE_NAME,
                         std::ios::binary | std::ios::trunc);
    if (!output)
    {
        throw std::runtime_error("Cannot create search results file.");
    }

    for (std::vector<ProductRecord>::const_iterator iterator = results.begin();
         iterator != results.end(); ++iterator)
    {
        output.write(reinterpret_cast<const char*>(&*iterator),
                     sizeof(ProductRecord));
        if (!output)
        {
            throw std::runtime_error("Cannot write search results.");
        }
    }
}

void WarehouseStorage::createReport() const
{
    std::ofstream output(REPORT_FILE_NAME, std::ios::trunc);
    if (!output)
    {
        throw std::runtime_error("Cannot create warehouse report.");
    }

    output << std::left
           << std::setw(8) << "Index"
           << std::setw(12) << "Warehouse"
           << std::setw(10) << "Code"
           << std::setw(25) << "Name"
           << std::setw(12) << "Arrival"
           << std::setw(12) << "Shelf life"
           << std::setw(10) << "Quantity"
           << std::setw(14) << "Unit price"
           << "Total\n";

    double grandTotal = 0.0;
    for (RecordMap::const_iterator iterator = records_.begin();
         iterator != records_.end(); ++iterator)
    {
        const ProductRecord& record = iterator->second;
        double positionTotal = record.quantity * record.unitPrice;
        grandTotal += positionTotal;

        output << std::left
               << std::setw(8) << iterator->first
               << std::setw(12) << record.warehouseNumber
               << std::setw(10) << record.productCode
               << std::setw(25) << record.productName
               << std::setfill('0') << std::right
               << std::setw(2) << record.arrivalDate.day << '.'
               << std::setw(2) << record.arrivalDate.month << '.'
               << std::setw(4) << record.arrivalDate.year
               << std::setfill(' ') << std::left << "  "
               << std::setw(12) << record.shelfLifeDays
               << std::setw(10) << record.quantity
               << std::setw(14) << std::fixed << std::setprecision(2)
               << record.unitPrice
               << positionTotal << '\n';
    }

    output << "\nGrand total: " << std::fixed << std::setprecision(2)
           << grandTotal << '\n';

    if (!output)
    {
        throw std::runtime_error("Cannot write warehouse report.");
    }
}

void runMenu()
{
    WarehouseStorage storage;

    try
    {
        storage.load();
    }
    catch (const std::exception& error)
    {
        std::cout << "Error: " << error.what() << '\n';
        return;
    }

    int choice = -1;
    while (choice != 0)
    {
        std::cout << "\nWarehouse records\n"
                  << "1. Show records\n"
                  << "2. Add a record\n"
                  << "3. Delete a record\n"
                  << "4. Replace a record\n"
                  << "5. Find by warehouse number\n"
                  << "6. Find by product code\n"
                  << "7. Find by arrival date\n"
                  << "8. Find expired products\n"
                  << "9. Find not expired products\n"
                  << "10. Create text report\n"
                  << "0. Return\n";

        try
        {
            choice = readValue<int>("Choice: ");

            switch (choice)
            {
            case 1:
                printRecords(storage.records());
                break;
            case 2:
            {
                ProductRecord record = readProductRecord();
                std::size_t index = storage.add(record);
                std::cout << "Record added with index " << index << ".\n";
                break;
            }
            case 3:
            {
                std::size_t index = readIndex("Record index: ");
                if (storage.remove(index))
                {
                    std::cout << "Record deleted.\n";
                }
                else
                {
                    std::cout << "Record with this index does not exist.\n";
                }
                break;
            }
            case 4:
            {
                std::size_t index = readIndex("Record index: ");
                if (storage.records().find(index) == storage.records().end())
                {
                    std::cout << "Record with this index does not exist.\n";
                    break;
                }

                ProductRecord record = readProductRecord();
                storage.replace(index, record);
                std::cout << "Record replaced. Its index is unchanged.\n";
                break;
            }
            case 5:
            {
                int number = readNonNegativeValue<int>("Warehouse number: ");
                printAndSaveSearchResults(
                    storage, storage.findByWarehouseNumber(number));
                break;
            }
            case 6:
            {
                int code = readNonNegativeValue<int>("Product code: ");
                printAndSaveSearchResults(storage, storage.findByProductCode(code));
                break;
            }
            case 7:
            {
                Date date = readDate("Arrival date to find");
                printAndSaveSearchResults(storage, storage.findByArrivalDate(date));
                break;
            }
            case 8:
            {
                Date date = readDate("Control date");
                printAndSaveSearchResults(storage, storage.findExpired(date));
                break;
            }
            case 9:
            {
                Date date = readDate("Control date");
                printAndSaveSearchResults(storage, storage.findNotExpired(date));
                break;
            }
            case 10:
                storage.createReport();
                std::cout << "Report is saved to warehouse_report.txt.\n";
                break;
            case 0:
                break;
            default:
                std::cout << "Unknown menu item.\n";
            }
        }
        catch (const std::exception& error)
        {
            std::cout << "Error: " << error.what() << '\n';
            if (std::cin.eof())
            {
                return;
            }
        }
    }
}

}
