#include "Warehouse.h"

#include <cstdio>
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
