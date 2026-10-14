#include "Warehouse.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <type_traits>

namespace warehouse
{

static_assert(std::is_trivially_copyable<ProductRecord>::value,
              "ProductRecord must be written as one binary object");

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

void runMenu()
{
    try
    {
        WarehouseStorage storage;
        storage.load();
        std::cout << "Warehouse records loaded: " << storage.records().size() << '\n';
    }
    catch (const std::exception& error)
    {
        std::cout << "Error: " << error.what() << '\n';
    }
}

}
