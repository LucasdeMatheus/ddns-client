//HttpClient.cpp


#include "HttpClient.h"
#include <curl/curl.h>
#include <iostream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}


HttpClient::HttpClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handle = curl_easy_init();

    if (curl_handle) {
        
        // Ativa verificação SSL (recomendado)
        curl_easy_setopt(static_cast<CURL*>(curl_handle), CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(static_cast<CURL*>(curl_handle), CURLOPT_SSL_VERIFYHOST, 2L);
        // Ativa debug para ajudar a diagnosticar
        curl_easy_setopt(static_cast<CURL*>(curl_handle), CURLOPT_VERBOSE, 1L);
    }
}




HttpClient::~HttpClient() {
    if (curl_handle) {
        curl_easy_cleanup(static_cast<CURL*>(curl_handle));
    }
    curl_global_cleanup();
}

bool HttpClient::get(const std::string& url, std::string& response, std::string& error,
                     const std::vector<std::string>& headers)
{
    if (!curl_handle) {
        error = "Curl não inicializado";
        return false;
    }

    CURL* curl = static_cast<CURL*>(curl_handle);
    curl_easy_reset(curl);

    // URL e saída
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // 🔑 Certificado CA (igual ao que você testou no curl.exe)
    curl_easy_setopt(curl, CURLOPT_CAINFO, "C:/Users/ffgus/Desktop/ddns-client/includes/cacert.pem");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // Headers opcionais
    struct curl_slist* headerList = nullptr;
    for (const auto& h : headers) {
        headerList = curl_slist_append(headerList, h.c_str());
    }
    if (headerList) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    }

    // Executa request
    CURLcode res = curl_easy_perform(curl);

    if (headerList) {
        curl_slist_free_all(headerList);
    }

    if (res != CURLE_OK) {
        error = curl_easy_strerror(res);
        return false;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
        error = "HTTP status code: " + std::to_string(http_code);
        return false;
    }

    return true;
}
