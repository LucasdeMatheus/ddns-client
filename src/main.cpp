#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "config/Config.h"
#include "dnsService/DnsService.h"
#include "DnsHost.h"

int main() {
    namespace fs = std::filesystem;

    const std::string basePath = "C:\\ProgramData\\ddns-service";
    const std::string configPath = basePath + "\\config.json";
    const std::string exeSource = "ddns-service.exe";       // main.exe e ddns-service.exe na mesma pasta
    const std::string exeDest = basePath + "\\ddns-service.exe";
    const std::string dllSource = "libcurl-x64.dll";        // DLL na mesma pasta que o instalador
    const std::string dllDest = basePath + "\\libcurl-x64.dll";

    std::cout << "[DEBUG] Iniciando instalador..." << std::endl;

    // 1. Cria pasta se não existir
    if (!fs::exists(basePath)) {
        std::cout << "[DEBUG] Criando pasta: " << basePath << std::endl;
        fs::create_directory(basePath);
    }

    // 2. Copia ddns-service.exe
    try {
        std::cout << "[DEBUG] Copiando executável para " << exeDest << std::endl;
        fs::copy_file(exeSource, exeDest, fs::copy_options::overwrite_existing);
    } catch (fs::filesystem_error& e) {
        std::cerr << "[ERRO] Falha ao copiar ddns-service.exe: " << e.what() << std::endl;
        return 1;
    }

    // 3. Copia libcurl-x64.dll
    try {
        std::cout << "[DEBUG] Copiando DLL para " << dllDest << std::endl;
        fs::copy_file(dllSource, dllDest, fs::copy_options::overwrite_existing);
    } catch (fs::filesystem_error& e) {
        std::cerr << "[ERRO] Falha ao copiar libcurl-x64.dll: " << e.what() << std::endl;
        return 1;
    }

    // 4. Solicita credenciais
    std::string apiKey;
    std::cout << "[DEBUG] Solicitando API Key..." << std::endl;
    std::cout << "Informe a API Key: ";
    std::getline(std::cin, apiKey);

    std::string apiBase = "https://api.juk.re"; // Pode alterar se quiser pedir ao usuário

    std::string intervalStr;
    int interval = 0;
    std::cout << "[DEBUG] Informe o intervalo de atualizacao (segundos): ";
    std::getline(std::cin, intervalStr);
    try {
        interval = std::stoi(intervalStr);
    } catch (...) {
        std::cerr << "[ERRO] Intervalo inválido!" << std::endl;
        return 1;
    }

    // 5. Lista hosts disponíveis
    std::cout << "[DEBUG] Listando hosts da API..." << std::endl;
    DnsService dnsService(apiBase, apiKey);
    std::vector<DnsHost> hosts;
    std::string error;

    if (!dnsService.listHosts(hosts, error)) {
        std::cerr << "[ERRO] Erro ao listar hosts: " << error << std::endl;
        return 1;
    }

    if (hosts.empty()) {
        std::cerr << "[ERRO] Nenhum host encontrado." << std::endl;
        return 1;
    }

    std::cout << "[DEBUG] Hosts recebidos: " << hosts.size() << std::endl;
    for (size_t i = 0; i < hosts.size(); ++i) {
        std::cout << i + 1 << ". " << hosts[i].label << " (" << hosts[i].fqdn << ")\n";
    }

    std::string input;
    std::cout << "[DEBUG] Aguardando escolha do host..." << std::endl;
    std::cout << "Escolha o host que deseja atualizar (numero): ";
    std::getline(std::cin, input);

    int escolha = 0;
    try {
        escolha = std::stoi(input);
    } catch (...) {
        std::cerr << "[ERRO] Entrada inválida!" << std::endl;
        return 1;
    }

    if (escolha < 1 || static_cast<size_t>(escolha) > hosts.size()) {
        std::cerr << "[ERRO] Escolha inválida!" << std::endl;
        return 1;
    }

    DnsHost selectedHost = hosts[escolha - 1];
    std::cout << "[DEBUG] Host selecionado: " << selectedHost.fqdn << std::endl;

    // 6. Sobrescreve config.json
    try {
        std::cout << "[DEBUG] Gravando config.json em " << configPath << std::endl;
        std::ofstream configFile(configPath);
        configFile << "{\n"
                   << "  \"api_key\": \"" << apiKey << "\",\n"
                   << "  \"api_base\": \"" << apiBase << "\",\n"
                   << "  \"interval_seconds\": " << interval << ",\n"
                   << "  \"token_update\": \"" << selectedHost.token_update << "\"\n"
                   << "}";
        configFile.close();
    } catch (...) {
        std::cerr << "[ERRO] Falha ao gerar config.json" << std::endl;
        return 1;
    }

    std::cout << "[DEBUG] Instalação concluída com sucesso!" << std::endl;
    std::cout << "O serviço irá atualizar o host: " << selectedHost.fqdn << std::endl;

    std::cout << "\nPressione ENTER para sair..." << std::flush;
    std::getline(std::cin, input); // Mantém o console aberto

    return 0;
}
