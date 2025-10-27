#ifndef CONNECTIONMONITOR_H
#define CONNECTIONMONITOR_H

#include <vector>
#include <string>
#include <map>

using namespace std;

struct NetworkConnection
{
    string protocol;
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
    string getProcessName(int pid);

public:
    vector<NetworkConnection> getTCPConnections();
    vector<NetworkConnection> getUDPConnections();
    vector<NetworkConnection> getAllConnections();

    vector<NetworkConnection> filterByProtocol(
        const vector<NetworkConnection> &connections,
        const string &protocol);

    vector<NetworkConnection> filterByState(
        const vector<NetworkConnection> &connections,
        const string &state);

    vector<NetworkConnection> filterByPort(
        const vector<NetworkConnection> &connections,
        int port);
    // вывод всех соединений
    void printConnections(const vector<NetworkConnection> &connections, bool use_color = false);

    string getColoredState(const string &state);

    map<string, int> getConnectionStatistics();
    // обнрвление статистики в реал времени
    void realTimeMonitoring(int duration_second = 30, int refresh_interval = 2);
    // сортировка
    vector<NetworkConnection> sortByLocalAddress(const vector<NetworkConnection> &connections);
    vector<NetworkConnection> sortByState(const vector<NetworkConnection> &connections);
    vector<NetworkConnection> sortByPID(const vector<NetworkConnection> &connections);
};

#endif