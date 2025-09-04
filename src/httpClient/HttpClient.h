//HttpClient.h
#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>
#include <vector>

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    // Adiciona parâmetro opcional headers
    bool get(const std::string& url, std::string& response, std::string& error,
             const std::vector<std::string>& headers = {});

private:
    void* curl_handle;
};

#endif // HTTPCLIENT_H
