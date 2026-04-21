#pragma once
#include <curl/curl.h>

#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace Steam::Networking::Web {
class CMFetcher {
 public:
  CMFetcher() = default;
  ~CMFetcher() = default;

  bool fetch_cm_servers();

  std::vector<std::pair<std::string, int>> get_servers();

 private:
  std::vector<std::pair<std::string, int>> servers_;
  std::mutex servers_mutex_;

  static size_t curl_write_callback(void* contents, size_t size, size_t nmemb,
                                    void* userp);
};
}  // namespace Steam::Networking::Web