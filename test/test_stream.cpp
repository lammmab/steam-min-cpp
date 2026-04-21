#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <stdexcept>
#include <steamclient/utilities/stream.hpp>


using Stream = Steam::Utils::Stream;
std::vector<unsigned char> EXPECTED{
    0x54, 0x45, 0x53, 0x54  // 0x54534554 (LE)
};

BOOST_AUTO_TEST_CASE(StreamWriting) {
  Stream s;
  s.Write(0x54534554);
  BOOST_TEST(s.Length() == 4);
  BOOST_TEST(s.GetBuffer() == EXPECTED, boost::test_tools::per_element());

  BOOST_REQUIRE_THROW(s.Read<unsigned char>(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(StreamReading) {
  Stream s(EXPECTED);
  BOOST_TEST(s.Length() == 4);
  BOOST_TEST(s.ReadBytes(4) == EXPECTED, boost::test_tools::per_element());

  BOOST_REQUIRE_THROW(s.Write(uint32_t(123)), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(StreamUnderflow) {
  Stream s({(uint8_t)0x01});
  BOOST_REQUIRE_THROW(s.Read<uint32_t>(), std::runtime_error);
}
