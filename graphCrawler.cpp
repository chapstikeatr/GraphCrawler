#include <cstddef>
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <string>

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
  size_t total = size * nmemb;
  auto *s = static_cast<std::string *>(userp);
  s->append(static_cast<char *>(buffer), total);
  return total;
}

int main(int argc, char *argv[]) {
  CURL *curl;
  std::string buffer;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(
        curl, CURLOPT_URL,
        "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/Tom%20Hanks");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
  }

  curl_easy_perform(curl);

  rapidjson::Document doc;
  doc.Parse(buffer.c_str());

  // Using a reference for consecutive access is handy and faster.
  const rapidjson::Value &a = doc["neighbors"];
  assert(a.IsArray());
  for (rapidjson::SizeType i = 0; i < a.Size();
       i++) // Uses SizeType instead of size_t
    printf("a[%u] = %s\n", i, a[i].GetString());

  // TODO Make a buffer to store the json
  // TODO Make a call back function to handle putting json into said buffer

  curl_global_cleanup();
  return 0;
}
