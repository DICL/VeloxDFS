#pragma once
#include <string>

namespace eclipse {

class DataSet {
  public:
    DataSet();
    DataSet(std::string input_path);
    ~DataSet();
    DataSet Map(std::string map_function_name);
    DataSet Reduce(std::string reduce_function_name);
    void SetInputPath(std::string input_path);
    std::string GetInputPath();
    void SetOutputPath(std::string output_path);
    bool IsOutputPath();
    std::string GetOutputPath();
    std::string GetRandomOutputPath();
  
  private:
    int job_id_;
    std::string input_path_;
    std::string output_path_;
    bool is_output_path_;
    static int counter_;
    void ReqeustToMaster(std::string func_name, std::string output_path);
};

}
