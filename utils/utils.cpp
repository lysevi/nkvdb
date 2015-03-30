#include "utils.h"
#include "ProcessLogger.h"

std::list<boost::filesystem::path> utils::ls(const std::string& path){
    std::list<boost::filesystem::path> result;

    std::copy(boost::filesystem::directory_iterator(path),
              boost::filesystem::directory_iterator(),
              std::back_inserter(result));

    return result;
}

bool utils::rm(const std::string&rm_path){
    if(!boost::filesystem::exists(rm_path))
        return true;
	try {
		boost::filesystem::path path_to_remove(rm_path);
		for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it != end_dir_it; ++it) {
			if (!boost::filesystem::remove_all(it->path())) {
				return false;
			}
		}
		return true;
	} catch (boost::filesystem::filesystem_error& ex) {
		std::string msg = ex.what();
		logger << "utils::rm exception: " << msg;
		throw;
	}
}
