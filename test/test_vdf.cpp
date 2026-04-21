#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <stdexcept>
#include <steamclient/utilities/vdf.hpp>
#include <string>


namespace VDF = Steam::Utils::VDF;

const char* TEST_STRING = R"""( // Root comment
"AppState"
{
    "appid"      "440"
    "name"       "Team Fortress 2"
    "installdir" "Team Fortress 2"

    // nested object
    "UserConfig"
    {
        "language" "english"
        "beta"     "public"
    }

    "Mounts"
    {
        "0"
        {
            "name"  "tf"
            "path"  "tf/"
        }

        "1"
        {
            "name"  "hl2"
            "path"  "hl2/"
        }
    }

    // escape test
    "Message" "Line1\nLine2\tTabbed \"quote\""
} )""";

const uint8_t TEST_BIN[] = {
    // int32
    0x02, 'a', 'p', 'p', 'i', 'd', 0x00, 0xB8, 0x01, 0x00, 0x00,
    // float
    0x03, 'r', 'a', 't', 'i', 'o', 0x00, 0x00, 0x00, 0xC0, 0x3F,
    // uint64
    0x07, 's', 't', 'e', 'a', 'm', 'i', 'd', 0x00, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x01,
    // string
    0x01, 'n', 'a', 'm', 'e', 0x00, 'T', 'F', '2', 0x00, 0x08};

BOOST_AUTO_TEST_CASE(VdfTextParsing) {
  auto vdf_obj = VDF::parse_text(TEST_STRING);
  auto& app_state = vdf_obj["AppState"];
  auto appid = app_state.get_string_or("appid", "");

  auto escaped_msg = app_state.get_string_or("Message", "");

  BOOST_TEST(appid == "440");
  BOOST_TEST(escaped_msg == "Line1\nLine2\tTabbed \"quote\"");
}

BOOST_AUTO_TEST_CASE(VdfBinaryParsing) {
  auto obj = VDF::parse_binary(TEST_BIN, sizeof(TEST_BIN));

  BOOST_TEST(std::get<int32_t>(obj["appid"].value) == 440);
  BOOST_TEST(std::get<float>(obj["ratio"].value) == 1.5f);
  BOOST_TEST(std::get<std::string>(obj["name"].value) == "TF2");
}

BOOST_AUTO_TEST_CASE(VdfBinaryOverflow) {
  const uint8_t bad[] = {0x02, 'a', 0x00};

  BOOST_REQUIRE_THROW(VDF::parse_binary(bad, sizeof(bad)), std::runtime_error);
}