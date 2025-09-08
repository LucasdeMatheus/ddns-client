
#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include "config/Config.h"
#include "updater/Updater.h"
#include "logger/Logger.h"
#include "dnsService/DnsService.h"
#include "DnsHost.h"

int main()
{
    Logger::log(LogLevel::INFO, "Iniciando cliente DDNS...");

    Config config;
    if (!config.load("C:\\ProgramData\\ddns-service\\config.json")) {
        Logger::log(LogLevel::LOG_ERROR, "Falha ao carregar configuração.");
        std::cout << "Falha ao carregar config.json. Pressione Enter para sair...";
        std::cin.get();
        return 1;
    }

    // Solicita API_KEY se não estiver definida
    if (config.getApiKey().empty()) {
        std::string apiKey;
        std::cout << "Insira sua API_KEY: ";
        std::getline(std::cin, apiKey);

        if (apiKey.empty()) {
            std::cerr << "Nenhuma API_KEY fornecida.\n";
            std::cout << "Pressione Enter para sair...";
            std::cin.get();
            return 1;
        }

        config.setApiKey(apiKey);
    }

    std::cout << "API_KEY carregada: " << config.getApiKey() << std::endl;

    // Inicializa DnsService
    DnsService dnsService(config.getApiBase(), config.getApiKey());

    std::vector<DnsHost> dnsHosts;
    std::string error;

    if (!dnsService.listHosts(dnsHosts, error)) {
        Logger::log(LogLevel::LOG_ERROR, "Erro ao listar hosts: " + error);
        std::cout << "Erro ao listar hosts: " << error << "\nPressione Enter para sair...";
        std::cin.get();
        return 1;
    }

    if (dnsHosts.empty()) {
        std::cout << "Você não possui registros de DNS.\n"
                     "Crie um registro no painel: https://juk.re/dns/new\n";
        std::cout << "Pressione Enter para sair...";
        std::cin.get();
        return 0;
    }

    std::cout << "Hosts existentes:\n";
    for (auto& h : dnsHosts) {
        std::cout << h.id << " - " << h.fqdn << " - " << h.token_update << "\n";
    }

    // Pergunta qual host atualizar
    int chosenId = askHostId();
    auto it = std::find_if(dnsHosts.begin(), dnsHosts.end(), [chosenId](const DnsHost& h) {
        return h.id == chosenId;
    });

    if (it == dnsHosts.end()) {
        std::cerr << "ID inválido.\n";
        std::cout << "Pressione Enter para sair...";
        std::cin.get();
        return 1;
    }

    DnsHost selectedHost = *it;
    config.setTokenUpdate(selectedHost.token_update);
    std::cout << "Você escolheu: " << selectedHost.fqdn << "\n";

    // Inicializa Updater
    Updater updater(config.getApiBase(), config.getTokenUpdate());

    // Loop de atualização apenas para o host selecionado
    while (true) {
        if (!updater.updateHost(selectedHost, config.getApiKey())) {
            Logger::log(LogLevel::WARNING, "Falha ao atualizar host " + selectedHost.fqdn);
        }

        Logger::log(LogLevel::INFO, "Aguardando " + std::to_string(config.getIntervalSeconds()) + " segundos para próxima atualização.");
        std::this_thread::sleep_for(std::chrono::seconds(config.getIntervalSeconds()));
    }

    // Nunca deve chegar aqui, mas mantém aberto
    std::cout << "Pressione Enter para encerrar...";
    std::cin.get();
    return 0;
}
