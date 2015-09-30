#include "page.h"
#include "readers.h"
#include "exception.h"

#include <sstream>

using namespace mdb;

PageReaderInterval::PageReaderInterval(Page::Page_ptr page):PageReader(page),
    from(0),
    to(0),
    m_read_pos_list()
{
    m_cur_pos_end=m_cur_pos_begin=0;

    isWindowReader = false;
    m_wwWindowReader_read_end = false;
    values_in_point_reader = false;
}

void PageReaderInterval::addReadPos(uint64_t begin,uint64_t end){
    m_read_pos_list.push_back(std::make_pair(begin,end));
}

bool PageReaderInterval::isEnd() const{
    if (isWindowReader) {
        return m_wwWindowReader_read_end;
    }
    if(m_read_pos_list.size()==0){
        return m_cur_pos_begin==m_cur_pos_end;
    }else{
        return false;
    }
}

void PageReaderInterval::readNext(Meas::MeasList*output){
    if (isEnd()) {
        return;
    }

    if (this->from > this->m_page->getHeader().maxTime) {
        for (auto wwIt : this->m_page->getWriteWindow()) {
            auto readedValue = wwIt;
            if (checkValueFlags(readedValue)) {
                output->push_back(readedValue);
            }
        }
        m_wwWindowReader_read_end = true;
        return;
    }

    if (from > this->m_page->getHeader().minTime) {
        if (!values_in_point_reader) {
            timePointRead(from,output);
            values_in_point_reader = true;
        }
    }

    if(m_cur_pos_begin==m_cur_pos_end){
        /// get next read interval
        auto pos=m_read_pos_list.front();
        m_read_pos_list.pop_front();

        m_cur_pos_begin=pos.first;
        m_cur_pos_end=pos.second;
    }
    auto read_to=(m_cur_pos_begin+PageReader::ReadSize);
    uint64_t i=0;
    for (i = m_cur_pos_begin; i < read_to; ++i) {
        if(i==m_cur_pos_end){
            break;
        }
        mdb::Meas readedValue;
        if (!m_page->read(&readedValue, i)) {
            std::stringstream ss;
            ss << "PageReader::readNext: "
               << " file name: " << m_page->fileName()
               << " readPos: " << i
               << " size: " << m_page->getHeader().size;

            throw MAKE_EXCEPTION(ss.str());
        }

        if ((checkValueInterval(readedValue)) && checkValueFlags(readedValue)){
            output->push_back(readedValue);
        }

    }
    m_cur_pos_begin=i;
}


bool PageReaderInterval::checkValueInterval(const Meas&m)const {
    if (utils::inInterval(from, to, m.time)) {
        return true;
    } else {
        return false;
    }
}

PageReader_TimePoint::PageReader_TimePoint(Page::Page_ptr page):PageReader(page),
    m_wwWindowReader_read_end(false){
    time_point = 0;
}

bool PageReader_TimePoint::isEnd() const{
    return m_wwWindowReader_read_end;
}

void PageReader_TimePoint::readNext(Meas::MeasList*output){
    if(isEnd()){
        return;
    }
   this->timePointRead(time_point,output);
   m_wwWindowReader_read_end=true;
}


