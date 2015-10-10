#pragma once

#include <string>
#include <nkvdb.h>

namespace nkvdb_test {
const size_t sizeInMb10 = 10 * 1024 * 1024;
const std::string storage_path = "dstorage";
const std::string test_page_name = "test_page.db";

void strage_test_add(nkvdb::MetaStorage*ms,nkvdb::Time from, nkvdb::Time to, nkvdb::Time step);
}
