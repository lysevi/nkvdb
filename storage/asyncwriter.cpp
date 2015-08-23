#include "asyncwriter.h"
#include "storage.h"

#include <utils/ProcessLogger.h>

#include <assert.h>

using namespace storage;

AsyncWriter::AsyncWriter()
{

}

void AsyncWriter::setStorage(DataStorage* storage){
    m_storage=storage;
}

void AsyncWriter::call(const Cache::PCache data){
    assert(m_storage!=nullptr);

    auto output=data->asArray();
    size_t meas_count = data->size();
    size_t to_write = data->size();

    while (to_write > 0) {
       size_t writed = m_storage->getCurPage()->append(output + (meas_count - to_write), to_write);
       if (writed != to_write) {
            m_storage->createNewPage();
       }
      to_write -= writed;
    }
    data->clear();
    data->sync_complete();
}
