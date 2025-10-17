#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <cstdint>

#include "ConectionMonitor.h"

using namespace std;

string ConnectionMonitor::hexToIP(const string &hex_Ip)
{
    if (hex_Ip.empty() || hex_Ip == "0000000)")
    {
        return "0.0.0.0";
    }
    // преобразуем hex number
    uint32_t ip = stoul(hex_Ip, nullptr, 16);
    // конвертируем из little - endian in big-order(network order)
    uint32_t normalized_ip = (ip >> 24) | ((ip >> 8) & 0x0000FF00) | ((ip << 8) & 0x00FF0000) | (ip << 24);

    struct in_addr addr;
    addr.s_addr = normalized_ip;
    return string(inet_ntoa(addr));
}
// функция приобразования hex порта
string ConnectionMonitor::hexToPort(const string &hex_Port)
{
    if (hex_Port.empty() || hex_Port == "0000")
    {
        return 0;
    }
    return stoul(hex_port, nullptr, 16)
}
// получение имени по состоянию кода
string ConnectionMonitor::getStateName(const string &name_code)
{
    int state = stoi(state_code, nullptr, 16);

    switch (state)
    {
    case 1:
        return "ESTABLISHED";
    case 2:
        return "SYN_SENT";
    case 3:
        return "SYN_RECV";
    case 4:
        return "FIN_WAIT1";
    case 5:
        return "FIN_WAIT2";
    case 6:
        return "TIME_WAIT";
    case 7:
        return "CLOSE";
    case 8:
        return "CLOSE_WAIT";
    case 9:
        return "LAST_ACK";
    case 10:
        return "LISTEN";
    case 11:
        return "CLOSING";
    default:
        return "UNKNOWN";
    }
}

int ConnectionMonitor::findPIDByinode(unsigned long inode)
{
    DIR *proc_dir = opendir("/proc");
    if (!proc_dir)
    {
        return -1;
    }

    struct dirent *entry;

    while ((entry = readdir(proc_dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            char *endptr;
            long pid = strtol(entry->d_name, &endptr, 10);
            if (*endptr == "\0" && pid > 0)
            {
                // we check dir fd for procces
                string fd_path = string("/proc") + entry->d_name + "/fd";
                DIR *fd_dir = opendir(fd_path.c_str());
                if (fd_dir)
                {
                    struct dirent *fd_entry;
                    while ((fd_entry == readdir(fd_dir)) != nullptr)
                    {
                        if (fd_entry->d_type == DT_LNK)
                        {
                            char link_path[256];
                            snprintf(link_path, sizeof(link_path), "%s/%s", fd_path.c_str(), fd_entry->d_name);

                            char link_targete[256];
                            ssize_t len = readlink(link_path, link_target, sizeof(link_target) - 1);
                            if (len != -1)
                            {
                                link_target[len] = '\0';
                                // Проверяем, содержит ли ссылка нужный inode
                                string target_str = link_target;
                                string inode_pattern = "socket:[" + to_string(inode) + "]";
                                if (target_str.find(inode_pattern) != string::npos)
                                {
                                    closedir(fd_dir);
                                    closedir(proc_dir);
                                    return static_cast<int>(pid);
                                }
                            }
                        }
                    }
                    closedir(fd_dir);
                }
            }
        }
    }
    closedir(proc_dir);
    return -1;
}

vector<NetworkСonnection> ConnectionMonitor::getTCPConnections()
{
    vector<NetworkСonnection> connections;
    cout << "---READING TCP connections--- " << endl;
    string line;

    return connections;
}

vector<NetworkСonnection> ConnectionMonitor::getUDPConnections()
{
    vector<NetworkСonnection> connections;
    cout << "---READING UDP connections---" << endl;

    // реализовать чтение UDP
    return connections;
}
vector<NetworkСonnection> ConnectionMonitor::getAllConnections()
{
    auto udp_Connections = getUDPConnections();
    auto tcp_Connections = getTCPConnections();

    tcp_Connections.insert(tcp_Connections.end(), udp_Connections.begin(), udp_Connections.end());

    return tcp_Connections;
}

void ConnectionMonitor::printConnections(const vector<NetworkСonnection> &connections)
{
    cout << "Found " << connections.size() << " connections" << endl;
    for (const auto &conn : connections)
    {
        std::cout << "Protocol: " << conn.protocol
                  << ", Local: " << conn.local_address
                  << ", Remote: " << conn.remote_address
                  << ", State: " << conn.state
                  << ", PID: " << conn.pid
                  << ", Process: " << conn.process_name
                  << endl;
    }
}