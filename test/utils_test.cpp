#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Main
#include <boost/test/unit_test.hpp>

#include <utils.h>
#include <asyncworker.h>
#include <search.h>

BOOST_AUTO_TEST_CASE(UtilsEmpty) {
  BOOST_CHECK(nkvdb::utils::inInterval(1, 5, 1));
  BOOST_CHECK(nkvdb::utils::inInterval(1, 5, 2));
  BOOST_CHECK(nkvdb::utils::inInterval(1, 5, 5));
  BOOST_CHECK(!nkvdb::utils::inInterval(1, 5, 0));
  BOOST_CHECK(!nkvdb::utils::inInterval(0, 1, 2));
}

BOOST_AUTO_TEST_CASE(FileUtils) {
  std::string filename = "foo/bar/test.txt";
  BOOST_CHECK_EQUAL(nkvdb::utils::filename(filename), "test");
  BOOST_CHECK_EQUAL(nkvdb::utils::parent_path(filename), "foo/bar");
}

class TestWorker : public nkvdb::utils::AsyncWorker<int> {
public:
  int value;
  TestWorker() : value(0) {}

  void call(const int data) override { value += data; }
};

BOOST_AUTO_TEST_CASE(Worker) {

  TestWorker worker;

  worker.start();

  BOOST_CHECK(!worker.isBusy());

  worker.pause_work();

  worker.add(1);
  worker.add(2);
  worker.add(3);
  worker.add(4);

  BOOST_CHECK(worker.isBusy());
  BOOST_CHECK_EQUAL(worker.value, (int)0);

  worker.continue_work();

  while (true) {
    if (!worker.isBusy()) {
      break;
    }
  }
  worker.stop();
  BOOST_CHECK(worker.stoped());
  BOOST_CHECK_EQUAL(worker.value, (int)1 + 2 + 3 + 4);
}

BOOST_AUTO_TEST_CASE(find_begin) {
  {
    std::vector<int> a = {0, 1, 2, 4, 5, 6, 7};

    for (int i = 0; i <= 7; i++) {
      auto res = utils::find_begin(a.begin(), a.end(), i,
                                   [](int r, int l) {
                                     if (r < l)
                                       return -1;
                                     if (r == l)
                                       return 0;

                                     return 1;
                                   },
                                   [](int r, int l) { return r - l; });
      BOOST_CHECK(*res >= i);
    }
  }

  {
    std::vector<int> a = {0, 2, 4, 6, 7};

    for (int i = 0; i <= 7; i++) {
      auto res = utils::find_begin(a.begin(), a.end(), i,
                                   [](int r, int l) {
                                     if (r < l)
                                       return -1;
                                     if (r == l)
                                       return 0;

                                     return 1;
                                   },
                                   [](int r, int l) { return r - l; });
      BOOST_CHECK(*res >= i);
    }
  }
}
