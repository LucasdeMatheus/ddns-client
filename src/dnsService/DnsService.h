#ifndef DNSSERVICE_H
#define DNSSERVICE_H

#include <string>
#include <vector>
#include <iostream>      // Para std::cin/std::cout
#include <limits>        // Para std::numeric_limits
#include "httpClient/HttpClient.h"
#include "DnsHost.h"

class DnsService {
public:
    DnsService(const std::string& baseUrl, const std::string& apiKey);

    bool listHosts(std::vector<DnsHost>& hosts, std::string& error);

private:
    HttpClient http;
    std::string baseUrl;
    std::string apiKey;
};

// Tornando inline para evitar múltipla definição
inline int askHostId() {
    int id;
    std::cout << "Qual deseja atualizar? ";
    while (!(std::cin >> id)) {
        std::cin.clear(); // limpa estado de erro
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // descarta entrada inválida
        std::cout << "Entrada inválida. Digite o ID do host: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // limpa o buffer
    return id;
}

#endif // DNSSERVICE_H
