
#include <curl/curl.h>
#include <rapidjson/document.h>

#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

// ---- libcurl callback: append response bytes into std::string ----
static size_t WriteToString(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  size_t total = size * nmemb;
  auto *s = static_cast<std::string *>(userp);
  s->append(static_cast<char *>(contents), total);
  return total;
}

// ---- helper: URL-encode node names safely (handles spaces, commas, etc.) ----
static std::string UrlEncode(CURL *curl, const std::string &raw) {
  char *encoded =
      curl_easy_escape(curl, raw.c_str(), static_cast<int>(raw.size()));
  if (!encoded)
    return "";
  std::string out(encoded);
  curl_free(encoded);
  return out;
}

// ---- fetch neighbors via HTTP GET, parse JSON, return vector<string> ----
static std::vector<std::string> FetchNeighbors(CURL *curl,
                                               const std::string &node) {
  const std::string base =
      "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";
  std::string response;

  // Build final URL: /neighbors/{node}
  std::string url = base + UrlEncode(curl, node);

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToString);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  CURLcode rc = curl_easy_perform(curl);
  if (rc != CURLE_OK) {
    std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(rc) << "\n";
    return {};
  }

  long http_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  if (http_code != 200) {
    std::cerr << "HTTP error: " << http_code << " for URL: " << url << "\n";
    return {};
  }

  // Parse JSON: { "neighbors": [...], "node": "..." }
  // :contentReference[oaicite:3]{index=3}
  rapidjson::Document doc;
  doc.Parse(response.c_str());
  if (doc.HasParseError() || !doc.IsObject()) {
    std::cerr << "JSON parse error\n";
    return {};
  }

  if (!doc.HasMember("neighbors") || !doc["neighbors"].IsArray()) {
    std::cerr << "JSON missing 'neighbors' array\n";
    return {};
  }

  std::vector<std::string> neighbors;
  for (const auto &v : doc["neighbors"].GetArray()) {
    if (v.IsString())
      neighbors.emplace_back(v.GetString());
  }
  return neighbors;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " \"Node Name\"\n";
    return 1;
  }

  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "curl_easy_init failed\n";
    curl_global_cleanup();
    return 1;
  }

  std::string start = argv[1];
  auto n = FetchNeighbors(curl, start);

  std::cout << "Neighbors of " << start << ":\n";
  for (const auto &x : n)
    std::cout << "  " << x << "\n";

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  return 0;
}
