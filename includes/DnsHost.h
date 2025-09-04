#ifndef DNSHOST_H
#define DNSHOST_H

#include <string>

struct DnsHost {
    int id;
    std::string fqdn;
    std::string label;
    std::string token_update;
    bool ativo;
};

#endif // DNSHOST_H
