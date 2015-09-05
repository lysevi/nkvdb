#pragma once

#include <utils/asyncworker.h>
#include "cache.h"

namespace storage {

class DataStorage;

class AsyncWriter : public utils::AsyncWorker<Cache::PCache> {
public:
  AsyncWriter();
  void setStorage(DataStorage *storage);
  void call(const Cache::PCache data) override;

private:
  DataStorage *m_storage;
};
}
