#include "dataset.hh"

using namespace eclipse;

static int DataSet::counter_ = 0;

DataSet::DataSet() {
  is_output_path_ = false;
}
DataSet::DataSet(std::string input_path) {
  SetInputPath(input_path);
}
DataSet::~DataSet() {
}
DataSet DataSet::Map(std::string map_function_name) {
  if (!IsOutputPath()) SetOutputPath(GetRandomOutputPath());
  // RequestToMaster(map_function_name, GetOutputPath());
  // WaitForMapFinished();
  DataSet output_data(GetOutputPath());
  return output_data;
}
DataSet DataSet::Reduce(std::string reduce_function_name) {
  // RequestToMaster(red_function_name, GetOutputPath());
  // WaitForRedFinished();
  DataSet output_data(GetOutputPath());
  return output_data;
}
void DataSet::SetInputPath(std::string input_path) {
  input_path_ = input_path;
}
std::string GetInputPath() {
  return input_path_;
}
void DataSet::SetOutputPath(std::string output_path) {
  output_path_ = output_path;
}
bool DataSet::IsOutputPath() {
  return is_output_path_;
}
std::string GetOutputPath() {
  return output_path_;
}
std::string GetRandomOutputPath() {
  stringstream ss;
  ss ".job" << job_id_ << "_idata" << counter_++;
  string output_path;
  output_path = ss.str();
  return output_path;
}
void DataSet::RequestToMaster(std::string func_name, std::string output_path) {
  // TODO:
  // 1. Send function name to master
  // 2. Send inputpath to master
  // 3. Send outputpath to master
}
