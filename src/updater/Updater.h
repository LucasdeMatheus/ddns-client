// Updater.h
#ifndef UPDATER_H
#define UPDATER_H

#include <string>
#include <vector>
#include "../httpClient/HttpClient.h"
#include "DnsHost.h"

class Updater {
public:
    Updater(const std::string& apiBase, const std::string& tokenUpdate);

    bool fetchHosts(std::vector<DnsHost>& hosts);

    // Atualização agora recebe apiKey
    bool updateHost(const DnsHost& host, const std::string& apiKey);

private:
    std::string api_base;
    std::string token_update;
    HttpClient httpClient;
};

#endif // UPDATER_H
