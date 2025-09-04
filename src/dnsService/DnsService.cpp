// DnsService.cpp

#include "DnsService.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

DnsService::DnsService(const std::string& baseUrl, const std::string& apiKey)
    : baseUrl(baseUrl), apiKey(apiKey) {}

bool DnsService::listHosts(std::vector<DnsHost>& hosts, std::string& error) {
    std::string url = baseUrl + "/v2/ddns/hosts";
    std::string response;

    // Requisição com Header de autenticação
    if (!http.get(url, response, error, { "X-API-KEY: " + apiKey })) {
        return false;
    }

    try {
        auto j = json::parse(response);
        if (!j.is_array()) {
            error = "Resposta inesperada da API";
            return false;
        }

        hosts.clear();
        hosts.clear();
        for (auto& item : j) {
            DnsHost h;
            if (item.contains("id")) h.id = item["id"].get<int>();
            if (item.contains("fqdn")) h.fqdn = item["fqdn"].get<std::string>();
            if (item.contains("label")) h.label = item["label"].get<std::string>();
            if (item.contains("token_update")) h.token_update = item["token_update"].get<std::string>();
            if (item.contains("ativo")) h.ativo = item["ativo"].get<bool>();
            hosts.push_back(h);
        }


        

        return true;
    } catch (const json::exception& e) {
        error = e.what();
        return false;
    }
}
