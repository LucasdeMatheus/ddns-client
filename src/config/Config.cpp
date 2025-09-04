//config.cpp

#include "Config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool Config::load(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[Config] Erro ao abrir arquivo: " << filename << std::endl;
            return false;
        }

        json j;
        file >> j;

        if (j.contains("interval_seconds") && j["interval_seconds"].is_number_integer()) {
            interval_seconds = j["interval_seconds"];
        } else {
            std::cerr << "[Config] 'interval_seconds' inválido ou ausente. Usando padrão 60." << std::endl;
        }

        if (j.contains("api_base") && j["api_base"].is_string()) {
            api_base = j["api_base"];
        } else {
            std::cerr << "[Config] 'api_base' inválido ou ausente." << std::endl;
            return false;
        }

        if (j.contains("token_update") && j["token_update"].is_string()) {
            token_update = j["token_update"];
        } else {
            std::cerr << "[Config] 'token_update' inválido ou ausente." << std::endl;
            return false;
        }

        if (j.contains("api_key") && j["api_key"].is_string()) {
            api_key = j["api_key"];
        } else {
            api_key.clear(); // se não tiver, deixa vazio
        }

        return true;
    } catch (const json::exception& e) {
        std::cerr << "[Config] Exceção JSON: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "[Config] Erro desconhecido ao carregar configuração." << std::endl;
        return false;
    }
}

int Config::getIntervalSeconds() const {
    return interval_seconds;
}

const std::string& Config::getApiBase() const {
    return api_base;
}

const std::string& Config::getTokenUpdate() const {
    return token_update;
}

const std::string& Config::getApiKey() const {
    return api_key;
}

void Config::setApiKey(const std::string& key) {
    api_key = key;
}

void Config::setTokenUpdate(const std::string& token) {
    token_update = token;
}
