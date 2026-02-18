#include <cstddef>
#include <curl/curl.h>
#include <iostream>
#include <queue>
#include <rapidjson/document.h>
#include <string>
#include <unordered_set>
#include <vector>

const std::string baseUrl =
    "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/";

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
  size_t total = size * nmemb;
  auto *s = static_cast<std::string *>(userp);
  s->append(static_cast<char *>(buffer), total);
  return total;
}

std::vector<std::string> FetchNeighbors(CURL *curl, std::string &PNode) {
  std::string enc = curl_easy_escape(curl, PNode.c_str(), 0);
  std::string encodedURL = baseUrl + enc;

  std::vector<std::string> neighbors;
  std::string buffer;
  curl_easy_setopt(curl, CURLOPT_URL, encodedURL.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

  CURLcode rc = curl_easy_perform(curl);

  rapidjson::Document doc;
  doc.Parse(buffer.c_str());

  for (auto &v : doc["neighbors"].GetArray()) {
    if (v.IsString())
      neighbors.emplace_back(v.GetString());
  }
  return neighbors;
}

std::vector<std::string> BFS(CURL *curl, const std::string &start,
                             int max_depth) {
  std::vector<std::string> visited_order;

  std::unordered_set<std::string> visited; // helps prevent duplicated
  std::queue<std::pair<std::string, int>> q;

  visited.insert(start);
  visited_order.push_back(start);
  q.push({start, 0});

  while (!q.empty()) {
    auto [node, depth] = q.front();
    q.pop();

    if (depth >= max_depth)
      continue;

    auto neighbors = FetchNeighbors(curl, node);
    for (auto neighbor : neighbors) {
      if (visited.insert(neighbor).second) {
        visited_order.push_back(neighbor);
        q.push({neighbor, depth + 1});
      }
    }
  }
  return visited_order;
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0]
              << " \"Start Node\" <depth>\n Make sure to escape spaces";
    return 1;
  }
  std::string buffer;

  const std::string start = argv[1];
  const int depth = std::atoi(argv[2]);

  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();

  using clock = std::chrono::steady_clock;
  auto t0 = clock::now();
  auto nodes = BFS(curl, start, depth);
  auto t1 = clock::now();

  std::chrono::duration<double> elapsed = t1 - t0;
  std::cout << "Visited " << nodes.size() << " nodes within depth " << depth
            << " from \"" << start << "\"\n";
  for (const auto &n : nodes) {
    std::cout << n << "\n";
  }

  const auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
  std::cerr << "Nodes " << nodes.size() << "Time: " << elapsed.count();

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  return 0;
}
