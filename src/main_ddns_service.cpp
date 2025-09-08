#define UNICODE
#undef UNICODE
#include <windows.h>
#include <atomic>
#include <string>
#include "config/Config.h"
#include "updater/Updater.h"
#include "dnsService/DnsService.h"
#include "DnsHost.h"

// ----------------------- Variáveis Globais -----------------------
std::atomic<bool> running(true);  // sinaliza se o serviço deve continuar
SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;
#define SERVICE_NAME "DDNSClientService"

// ----------------------- Prototipos -----------------------
void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);
void ddnsClientLoop();
void LogEvent(const std::string& message, WORD type = EVENTLOG_INFORMATION_TYPE);

// ----------------------- Main do Serviço -----------------------
int main() {
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        { (LPSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };
    StartServiceCtrlDispatcher(ServiceTable);
    return 0;
}

// ----------------------- Implementação do Serviço -----------------------
void ServiceMain(int argc, char** argv) {
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hStatus = RegisterServiceCtrlHandler(SERVICE_NAME, (LPHANDLER_FUNCTION)ControlHandler);
    if (!hStatus) return;

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);

    HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);

    ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hStatus, &ServiceStatus);
}

// ----------------------- Controle do Serviço -----------------------
void ControlHandler(DWORD request) {
    switch(request) {
        case SERVICE_CONTROL_STOP:
            ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
            SetServiceStatus(hStatus, &ServiceStatus);
            running.store(false); // para o loop
            ExitProcess(0);
            break;
        default:
            break;
    }
}

// ----------------------- Thread do Serviço -----------------------
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam) {
    while (running.load()) {
        ddnsClientLoop();
        Sleep(1000); // Aguarda antes de repetir
    }
    return 0;
}

// ----------------------- Função Principal do Cliente DDNS -----------------------
void ddnsClientLoop() {
    Config config;
    if (!config.load("C:\\ProgramData\\ddns-service\\config.json")) {
    LogEvent("Falha ao carregar configuracao", EVENTLOG_ERROR_TYPE);
    return;
    }


    if (config.getApiKey().empty()) {
        LogEvent("API_KEY nao definida", EVENTLOG_ERROR_TYPE);
        return;
    }

    DnsService dnsService(config.getApiBase(), config.getApiKey());
    std::vector<DnsHost> dnsHosts;
    std::string error;

    if (!dnsService.listHosts(dnsHosts, error)) {
        LogEvent("Erro ao listar hosts: " + error, EVENTLOG_ERROR_TYPE);
        return;
    }

    if (dnsHosts.empty()) {
        LogEvent("Nenhum registro de DNS encontrado", EVENTLOG_WARNING_TYPE);
        return;
    }

    DnsHost selectedHost = dnsHosts[0]; // pega o primeiro host
    config.setTokenUpdate(selectedHost.token_update);
    LogEvent("Atualizando host: " + selectedHost.fqdn, EVENTLOG_INFORMATION_TYPE);

    Updater updater(config.getApiBase(), config.getTokenUpdate());

    while (running.load()) {
        if (!updater.updateHost(selectedHost, config.getApiKey())) {
            LogEvent("Falha ao atualizar host: " + selectedHost.fqdn, EVENTLOG_WARNING_TYPE);
        } else {
            LogEvent("Host atualizado: " + selectedHost.fqdn, EVENTLOG_INFORMATION_TYPE);
        }
        Sleep(config.getIntervalSeconds() * 1000);
    }

    LogEvent("Serviço DDNS finalizado", EVENTLOG_INFORMATION_TYPE);
}

// ----------------------- Função de Logging -----------------------
void LogEvent(const std::string& message, WORD type) {
    HANDLE hEventSource = RegisterEventSource(NULL, SERVICE_NAME);
    if (hEventSource) {
        const char* msg = message.c_str();
        ReportEventA(hEventSource, type, 0, 0, NULL, 1, 0, &msg, NULL);
        DeregisterEventSource(hEventSource);
    }
}
