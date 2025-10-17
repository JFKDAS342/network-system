#ifndef CONECTIONMONITOR_H
#define CONECTIONMONITOR_H
#include <vector>
#include <string>
#include <map>

using namespace std;
struct NetworkСonnection
{
    string protocol; // tcp or udp
    string local_address;
    string remote_address;
    string state;
    int pid;
    string process_name;
    unsigned long inode;
};
class ConnectionMonitor
{
private:
    string hexToIP(const string &hex_ip);
    int hexToPort(const string &hex_port);
    string getStateName(const string &state_code);
    int findPIDByinode(unsigned long inode);
    string getProccesName(int pid);

public:
    vector<NetworkСonnection> getTCPConnections();
    vector<NetworkСonnection> getUDPConnections();
    vector<NetworkСonnection> getAllConnections();

    vector<NetworkСonnection> filterByProtocol(
        const vector<NetworkСonnection> &connections,
        const string &protocol);

    vector<NetworkСonnection> filterByState(
        const vector<NetworkСonnection> &connections,
        const string &state);

    vector<NetworkСonnection> filterByPort(
        const vector<NetworkСonnection> &connections,
        int port);

    // Вспомогательные методы
    void printConnections(const vector<NetworkСonnection> &connections);
    map<string, int> getConnectionStatistics();
};
#endif