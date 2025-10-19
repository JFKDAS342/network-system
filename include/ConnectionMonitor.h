#ifndef CONNECTIONMONITOR_H
#define CONNECTIONMONITOR_H

#include <vector>
#include <string>
#include <map>

using namespace std;

struct NetworkConnection // ЛАТИНСКАЯ 'C'
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
    string getProcessName(int pid); // ОДНА 'c' - getProcessName

public:
    vector<NetworkConnection> getTCPConnections(); // ЛАТИНСКАЯ 'C'
    vector<NetworkConnection> getUDPConnections(); // ЛАТИНСКАЯ 'C'
    vector<NetworkConnection> getAllConnections(); // ЛАТИНСКАЯ 'C'

    vector<NetworkConnection> filterByProtocol(       // ЛАТИНСКАЯ 'C'
        const vector<NetworkConnection> &connections, // ЛАТИНСКАЯ 'C'
        const string &protocol);

    vector<NetworkConnection> filterByState(          // ЛАТИНСКАЯ 'C'
        const vector<NetworkConnection> &connections, // ЛАТИНСКАЯ 'C'
        const string &state);

    vector<NetworkConnection> filterByPort(           // ЛАТИНСКАЯ 'C'
        const vector<NetworkConnection> &connections, // ЛАТИНСКАЯ 'C'
        int port);

    void printConnections(const vector<NetworkConnection> &connections); // ЛАТИНСКАЯ 'C'
    map<string, int> getConnectionStatistics();
};

#endif