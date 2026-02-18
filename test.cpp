
#include <curl/curl.h>
#include <iostream>
#include <string>

// Callback function to write the data to an std::string
static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

int main() {
  CURL *curl;
  CURLcode res;
  std::string readBuffer;
  std::string url =
      "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/Tom Hanks";

  curl = curl_easy_init();
  if (curl) {
    // Set the URL to fetch
    curl_easy_setopt(
        curl, CURLOPT_URL,
        "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/Tom%20Hanks");

    // Set the write function callback
    //   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    // Pass the std::string buffer to the callback function
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the request, res will get the return code
    res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
    } else {
      // Print the fetched data (or process it further)
      std::cout << "--- Response from " << url << " ---" << std::endl;
      std::cout << readBuffer << std::endl;
    }

    // Clean up the curl handle
    curl_easy_cleanup(curl);
  }

  // In a real application, you might want to call curl_global_cleanup()
  // when you are done using libcurl entirely (e.g., at the end of your
  // program). curl_global_cleanup();

  return 0;
}
