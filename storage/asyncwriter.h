#pragma once

#include "utils/asyncworker.h"
#include "cache.h"

namespace storage {

class Storage;

class AsyncWriter : public utils::AsyncWorker<Cache::PCache> {
public:
  AsyncWriter();
  void setStorage(Storage *storage);
  void call(const Cache::PCache data) override;

private:
  Storage *m_storage;
};
}
