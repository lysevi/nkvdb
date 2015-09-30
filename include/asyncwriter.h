#pragma once

#include "asyncworker.h"
#include "cache.h"

namespace mdb {

class Storage;

class AsyncWriter : public utils::AsyncWorker<Cache::PCache> {
public:
  AsyncWriter();
  void setStorage(Storage *mdb);
  void call(const Cache::PCache data) override;

private:
  Storage *m_storage;
};
}
