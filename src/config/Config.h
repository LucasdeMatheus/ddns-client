//config.h

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
public:
    bool load(const std::string& filename);

    int getIntervalSeconds() const;
    const std::string& getApiBase() const;
    const std::string& getTokenUpdate() const;

    const std::string& getApiKey() const;   // novo getter
    void setApiKey(const std::string& key); // novo setter
    void setTokenUpdate(const std::string& token); // novo setter

private:
    int interval_seconds = 60;
    std::string api_base;
    std::string token_update;
    std::string api_key; // novo campo
};

#endif // CONFIG_H
