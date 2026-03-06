#include "web/cmfetcher.hpp"

using namespace Steam::Networking;

size_t Web::CMFetcher::curl_write_callback(
    void* contents, 
    size_t size, 
    size_t nmemb, 
    void* userp
) {
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

bool Web::CMFetcher::fetch_cm_servers() {
    const std::string url =
        "https://api.steampowered.com/ISteamDirectory/GetCMListForConnect/v1/?cellid=0&cmtype=netfilter";

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) return false;

    ctl::json root;
    if (!root.parse(response)) {
        return false;
    }

    auto& resp_node = root["response"];
    auto& list_node = resp_node["serverlist"];

    std::lock_guard<std::mutex> lock(servers_mutex_);
    servers_.clear();

    for (size_t i = 0; i < list_node.size(); ++i) {
        std::string endpoint = list_node[i]["endpoint"].str();
        auto pos = endpoint.find(':');
        if (pos == std::string::npos) continue;

        std::string ip = endpoint.substr(0, pos);
        int port = std::stoi(endpoint.substr(pos + 1));
        servers_.emplace_back(ip, port);
    }

    return !servers_.empty();
}

std::vector<std::pair<std::string, int>> Web::CMFetcher::get_servers() {
    std::lock_guard<std::mutex> lock(servers_mutex_);
    return servers_;
}