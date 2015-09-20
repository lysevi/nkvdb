#pragma once

namespace mdb{

class PageReaderInterval:public PageReader
{
    typedef std::pair<uint64_t,uint64_t> from_to_pos;
public:
    PageReaderInterval(Page::Page_ptr page);

    virtual bool isEnd() const override;
    virtual void readNext(Meas::MeasList*output)override;
    /// add {from,to} position to read.
    void addReadPos(uint64_t begin,uint64_t end);
public:
    Time from;
    Time to;
        bool isWindowReader;
private:
        bool checkValueInterval(const Meas&m)const;
private:
    std::list<from_to_pos> m_read_pos_list;
    uint64_t m_cur_pos_begin;
    uint64_t m_cur_pos_end;

        bool m_wwWindowReader_read_end;
        bool values_in_point_reader;
        friend class StorageReader;
};

class PageReader_TimePoint:public PageReader{
public:
    PageReader_TimePoint(Page::Page_ptr page);
    virtual bool isEnd() const override;
    virtual void readNext(Meas::MeasList*output)override;
    Time time_point;
private:
    bool m_wwWindowReader_read_end;
};

}
