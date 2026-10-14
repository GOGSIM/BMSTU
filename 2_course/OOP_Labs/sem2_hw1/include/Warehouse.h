#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <cstddef>
#include <map>
#include <string>

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

class WarehouseStorage
{
public:
    explicit WarehouseStorage(const std::string& fileName = WAREHOUSE_FILE_NAME);

    void load();
    const RecordMap& records() const;

private:
    std::string fileName_;
    RecordMap records_;
};

void runMenu();

}

#endif
