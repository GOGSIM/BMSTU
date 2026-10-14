#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace warehouse
{

const std::string WAREHOUSE_FILE_NAME = "warehouse.dat";
const std::string TEMPORARY_FILE_NAME = "warehouse.tmp";
const std::string SEARCH_RESULTS_FILE_NAME = "search_results.dat";
const std::string REPORT_FILE_NAME = "warehouse_report.txt";
const std::size_t PRODUCT_NAME_SIZE = 64;

struct Date
{
    int day;
    int month;
    int year;
};

struct ProductRecord
{
    std::size_t index;
    int warehouseNumber;
    int productCode;
    char productName[PRODUCT_NAME_SIZE];
    Date arrivalDate;
    int shelfLifeDays;
    int quantity;
    double unitPrice;
};

typedef std::map<std::size_t, ProductRecord> RecordMap;

bool isLeapYear(int year);
int daysInMonth(int month, int year);
bool isValidDate(const Date& date);
long long dateToDays(const Date& date);
int compareDates(const Date& left, const Date& right);
bool isExpired(const ProductRecord& record, const Date& controlDate);

class WarehouseStorage
{
public:
    explicit WarehouseStorage(const std::string& fileName = WAREHOUSE_FILE_NAME);

    void load();
    const RecordMap& records() const;
    std::size_t add(ProductRecord record);
    bool remove(std::size_t index);
    bool replace(std::size_t index, ProductRecord record);
    std::vector<ProductRecord> findByWarehouseNumber(int warehouseNumber) const;
    std::vector<ProductRecord> findByProductCode(int productCode) const;
    std::vector<ProductRecord> findByArrivalDate(const Date& date) const;
    std::vector<ProductRecord> findExpired(const Date& controlDate) const;
    std::vector<ProductRecord> findNotExpired(const Date& controlDate) const;

private:
    std::size_t nextIndex() const;
    void appendRecord(const ProductRecord& record) const;
    void rewriteFile(const RecordMap& updatedRecords) const;

    std::string fileName_;
    RecordMap records_;
};

void runMenu();

}

#endif
