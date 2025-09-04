
// Updater.cpp
#include "Updater.h"
#include <nlohmann/json.hpp>
#include "../logger/Logger.h"
#include <iostream>

using json = nlohmann::json;

Updater::Updater(const std::string& apiBase, const std::string& tokenUpdate)
    : api_base(apiBase), token_update(tokenUpdate) {}


bool Updater::updateHost(const DnsHost& host, const std::string& apiKey) {
    // Monta a URL do endpoint usando o token de atualização do host
    std::string url = api_base + "/v2/ddns/update?token=" + host.token_update;

    std::string response;
    std::string error;

    // Chamada GET passando o header X-API-KEY
    // O HttpClient precisa suportar vetor de headers
    if (!httpClient.get(url, response, error, { "X-API-KEY: " + apiKey })) {
        Logger::log(LogLevel::ERROR, "Falha ao atualizar host " + host.fqdn + ": " + error);
        return false;
    }

    Logger::log(LogLevel::INFO, "Host atualizado: " + host.fqdn);
    return true;
}
