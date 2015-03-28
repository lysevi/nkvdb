#include "utils.h"

std::list<boost::filesystem::path> utils::ls(const std::string& path){
    std::list<boost::filesystem::path> result;

    std::copy(boost::filesystem::directory_iterator(path),
              boost::filesystem::directory_iterator(),
              std::back_inserter(result));

    return result;
}
