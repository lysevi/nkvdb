#include "asyncwriter.h"
#include "storage.h"
#include "page_manager.h"
#include <assert.h>
#include <algorithm>
using namespace storage;

struct MeasCmpByTime {
  bool operator()(storage::Meas a, storage::Meas b) { return a.time < b.time; }
};

AsyncWriter::AsyncWriter() {}

void AsyncWriter::setStorage(Storage *storage) { m_storage = storage; }

void AsyncWriter::call(const Cache::PCache data) {
  assert(m_storage != nullptr);

  auto output = data->asArray();

  //MeasCmpByTime time_cmp;
  //std::sort(output, output + data->size(), time_cmp);

  size_t meas_count = data->size();
  size_t to_write = data->size();

  while (to_write > 0) {
    size_t writed = PageManager::get()->getCurPage()->append(output + (meas_count - to_write), to_write);
    if (writed != to_write) {
		PageManager::get()->createNewPage();
    }
    to_write -= writed;
  }
  PageManager::get()->getCurPage()->flushWriteWindow();
  data->clear();
  data->sync_complete();
}
