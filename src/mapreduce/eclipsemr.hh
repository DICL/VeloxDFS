#pragma once
#include <string>
#include "dataset.hh"

namespace eclipse {

// Variables for tasks
extern std::string 

// User functions
void InitEclipseMr();
void FinalizeEclipseMr();
bool IsNextRecord();
std::string GetNextRecord();
void WriteKeyValue(std::string key, std::string value);
std::string GetKey();
bool IsNextValue();
std::string GetNextValue()
void WriteRecord(std::string record);

}
