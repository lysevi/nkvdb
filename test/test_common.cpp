#include "test_common.h"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <logger.h>

namespace nkvdb_test {

// [1,2,3,4,...n]... half..[1,2,3,1,2,3]
void storage_test_io(nkvdb::MetaStorage*ms,nkvdb::Time from, nkvdb::Time to, nkvdb::Time step){
    auto total_count=nkvdb::Time((to-from)/step);
    auto half_count=nkvdb::Time(total_count/2);
    const size_t id_count=3;
    nkvdb::Meas::MeasArray marray(total_count);
    int pos=0;

    std::list<nkvdb::Id> id_before_half{};
	std::list<nkvdb::Id> id_after_half{};
	std::list<nkvdb::Id> id_total{};
    nkvdb::Meas m{};
    for(size_t i=1;i<=total_count+1;i+=step){
        if(i==half_count){
            continue;
        }

        if (i<half_count){
            m.id=nkvdb::Id(i);
            id_before_half.push_back(m.id);
        }

        if (i>half_count){
            m.id=nkvdb::Id(i%id_count);
			id_after_half.push_back(m.id);
        }

        m.time=nkvdb::Time(i);
        m.flag=nkvdb::Flag(0);
        m.source=nkvdb::Flag(0);
        m.setValue(i);

		id_total.push_back(m.id);
        marray[pos]=m;
        pos++;
    }

    ms->append(marray);

    BOOST_CHECK_EQUAL(ms->minTime(),nkvdb::Time(1));
	BOOST_CHECK_EQUAL(ms->maxTime(), m.time);

    // TIME POINT CHECKS
    auto half_reader=ms->readInTimePoint(half_count);
    nkvdb::Meas::MeasList output{};
    half_reader->readAll(&output);

    //BOOST_CHECK_EQUAL(output.size(),half_count);
    for(auto id:id_before_half){
        auto find_res=std::find_if(std::begin(output),std::end(output),
                                   [id](const nkvdb::Meas&m){return m.id==id;});
		if (find_res == std::end(output)) {
			logger("id:"<<id<<" not found");
			BOOST_CHECK(false);
		}
    }


    auto full_time_point_reader=ms->readInTimePoint(m.time+1);
    output.clear();
    full_time_point_reader->readAll(&output);

    //BOOST_CHECK_EQUAL(output.size(),half_count);

    for(auto id:id_before_half){
        auto find_res=std::find_if(std::begin(output),std::end(output),
                                   [id](const nkvdb::Meas&m){return m.id==id;});
		if (find_res == std::end(output)) {
			logger("id:" << id << " not found");
			BOOST_CHECK(false);
		}
    }

    auto max_time_elem=*std::max_element(std::begin(output),std::end(output),
                                         [](const nkvdb::Meas&a,const nkvdb::Meas&b){return a.time<b.time;});
    nkvdb::Time max_time=max_time_elem.time;
    BOOST_CHECK(max_time>from);
    BOOST_CHECK(max_time<=m.time);
    BOOST_CHECK_EQUAL(m.id,max_time_elem.id);

    // INTERVAL CHECKS
    auto full_interval_reader=ms->readInterval(from,to);
    output.clear();
    full_interval_reader->readAll(&output);

	for (auto id : id_total) {
		auto find_res = std::find_if(std::begin(output), std::end(output),
									 [id](const nkvdb::Meas&m){return m.id == id; });
		if (find_res == std::end(output)) {
			logger("id:" << id << " not found");
			BOOST_CHECK(false);
		}
	}


    auto half_interval_reader=ms->readInterval(half_count,to);
    output.clear();
    half_interval_reader->readAll(&output);

    for (auto id : id_after_half) {
		auto find_res = std::find_if(std::begin(output), std::end(output),
									 [id](const nkvdb::Meas&m){return m.id == id; });
		if (find_res == std::end(output)) {
			logger("id:" << id << " not found");
			BOOST_CHECK(false);
		}
	}
}

}
