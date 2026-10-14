#ifndef SEQUENTIAL_PART_H
#define SEQUENTIAL_PART_H

#include <string>

namespace sequential
{
const std::string INPUT_FILE_NAME = "input.txt";
const std::string OUTPUT_FILE_NAME = "output.txt";

void createDataWithStreams(const std::string& fileName);
void runMenu();
}

#endif

