#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <cstdint>

#include "../include/ConnectionMonitor.h"

using namespace std;

string ConnectionMonitor::hexToIP(const string &hex_Ip)
{
    if (hex_Ip.empty() || hex_Ip == "00000000")
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

int ConnectionMonitor::hexToPort(const string &hex_Port)
{
    if (hex_Port.empty() || hex_Port == "0000")
    {
        return 0;
    }
    return stoul(hex_Port, nullptr, 16);
}

string ConnectionMonitor::getStateName(const string &state_code)
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
            if (*endptr == '\0' && pid > 0)
            {
                // we check dir fd for process
                string fd_path = string("/proc/") + entry->d_name + "/fd"; //
                DIR *fd_dir = opendir(fd_path.c_str());
                if (fd_dir)
                {
                    struct dirent *fd_entry;
                    while ((fd_entry = readdir(fd_dir)) != nullptr)
                    {
                        if (fd_entry->d_type == DT_LNK)
                        {
                            char link_path[256];
                            snprintf(link_path, sizeof(link_path), "%s/%s", fd_path.c_str(), fd_entry->d_name);

                            char link_target[256];
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

string ConnectionMonitor::getProcessName(int pid)
{
    if (pid <= 0)
    {
        return "unknown";
    }
    // пробуем прочитать из /proc/pid/comm
    string comm_path = "/proc/" + to_string(pid) + "/comm";
    com
        ifstream comm_file(comm_path);
    if (comm_file.is_open())
    {
        string process_name;
        getline(comm_file, process_name);
        if (!process_name.empty())
        {
            return process_name;
        }
    }

    // если не получилось прочитать, читаем из /proc/pid/status
    string status_path = "/proc/" + to_string(pid) + "/status";
    ifstream status_file(status_path);
    if (status_file.is_open())
    {
        string line;
        while (getline(status_file, line))
        {
            if (line.find("Name:") == 0)
            {
                return line.substr(6);
            }
        }
    }

    return "unknown";
}

vector<NetworkConnection> ConnectionMonitor::getTCPConnections()
{
    vector<NetworkConnection> connections;
    ifstream file("/proc/net/tcp");
    if (!file.is_open())
    {
        cerr << "error: (TCP)" << endl;
        return connections;
    }
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);
        string token;

        NetworkConnection conn;
        conn.protocol = "TCP";
        // пропускаем номер строки
        if (!(iss >> token))
            continue;

        if (!(iss >> token))
            continue;

        size_t colon_pos = token.find(':');
        if (colon_pos == string::npos)
            continue;

        string local_ip_hex = token.substr(0, colon_pos);
        remote_ip_hex
            string local_port_hex = token.substr(colon_pos + 1);
        remote_ip_port
            conn.local_address = hexToIP(local_ip_hex) + ":" + to_string(hexToPort(local_port_hex));

        if (!(iss >> token))
            continue;

        colon_pos = token.find(':');
        if (colon_pos == string::npos)
            continue;

        string remote_ip_hex = token.substr(0, colon_pos);
        string remote_port_hex = token.substr(colon_pos + 1);
        conn.remote_address = hexToIP(remote_ip_hex) + ":" + to_string(hexToPort(remote_port_hex));

        if (!(iss >> token))
            continue;
        conn.state = getStateName(token);

        // пропускаем tx/rx
        if (!(iss >> token))
            continue; // tx_q
        if (!(iss >> token))
            continue; // rx_q
        // пропуск таймеров
        if (!(iss >> token))
            continue; // tr
        if (!(iss >> token))
            continue; // tm->when
        if (!(iss >> token))
            continue; // retrnsmt

        // UID
        if (!(iss >> token))
            continue;
        // пропуск timeout
        if (!(iss >> token))
            continue;

        if (!(iss >> token))
            continue;
        conn.inode = stoul(token);

        // получение PID and Name process
        conn.pid = findPIDByinode(conn.inode);
        conn.process_name = getProcessName(conn.pid);

        connections.push_back(conn);
    }
    return connections;
}

vector<NetworkConnection> ConnectionMonitor::getUDPConnections()
{
    vector<NetworkConnection> connections;
    ifstream file("/proc/net/udp");
    if (!file.is_open())
    {
        cerr << "error:(UDP)" << endl;
        return connections;
    }
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);
        string token;
        NetworkConnection conn;
        conn.protocol = "UDP";
        conn.state = "UDP";
        // пропускаем номер строки
        if (!(iss >> token))
            continue;

        if (!(iss >> token))
            continue;

        size_t colon_pos = token.find(':');
        if (colon_pos == string::npos)
            continue;

        string local_ip_hex = token.substr(0, colon_pos);
        string local_port_hex = token.substr(colon_pos + 1);
        conn.local_address = hexToIP(local_ip_hex) + ":" + to_string(hexToPort(local_port_hex));

        if (!(iss >> token))
            continue;

        colon_pos = token.find(':');
        if (colon_pos == string::npos)
            continue;

        string remote_ip_hex = token.substr(0, colon_pos);
        string remote_port_hex = token.substr(colon_pos + 1);
        conn.remote_address = hexToIP(remote_ip_hex) + ":" + to_string(hexToPort(remote_port_hex));

        // состояние(для UDP оно всегда 07)
        if (!(iss >> token))
            continue;
        // пропускаем tx/rx и тп поля
        for (int i = 0; i < 7; i++)
        {
            if (!(iss >> token))
                break;
        }

        if (!(iss >> token))
            continue;
        conn.inode = stoul(token);

        conn.pid = findPIDByinode(conn.inode);
        conn.process_name = getProcessName(conn.pid);

        connections.push_back(conn);
    }

    return connections;
}

// получение всех соединений
vector<NetworkConnection> ConnectionMonitor::getAllConnections()
{
    auto udp_Connections = getUDPConnections();
    auto tcp_Connections = getTCPConnections();

    tcp_Connections.insert(tcp_Connections.end(), udp_Connections.begin(), udp_Connections.end());

    return tcp_Connections;
}

// фильтрация по протоколу
vector<NetworkConnection> ConnectionMonitor::filterByProtocol(const vector<NetworkConnection> &connections, const string &protocol)
{
    vector<NetworkConnection> result;
    string upper_protocol = protocol;
    transform(upper_protocol.begin(), upper_protocol.end(), upper_protocol.begin(), ::toupper);

    copy_if(connections.begin(), connections.end(), back_inserter(result),
            [&upper_protocol](const NetworkConnection &conn)
            {
                string conn_protocol = conn.protocol;
                transform(conn_protocol.begin(), conn_protocol.end(), conn_protocol.begin(), ::toupper);
                return conn_protocol == upper_protocol;
            });
    return result;
}

vector<NetworkConnection> ConnectionMonitor::filterByState(const vector<NetworkConnection> &connections, const string &state)
{
    vector<NetworkConnection> result;
    string upper_state = state;
    transform(upper_state.begin(), upper_state.end(), upper_state.begin(), ::toupper);

    copy_if(connections.begin(), connections.end(), back_inserter(result),
            [&upper_state](const NetworkConnection &conn) // Исправлено: добавлен параметр conn
            {
                string conn_state = conn.state;
                transform(conn_state.begin(), conn_state.end(), conn_state.begin(), ::toupper);
                return conn_state == upper_state;
            });
    return result;
}

vector<NetworkConnection> ConnectionMonitor::filterByPort(const vector<NetworkConnection> &connections, int port)
{
    vector<NetworkConnection> result;
    copy_if(connections.begin(), connections.end(), back_inserter(result),
            [port](const NetworkConnection &conn)
            {
                // проверяем локальный порт
                size_t colon_pos = conn.local_address.find(':');
                if (colon_pos != string::npos)
                {
                    string port_str = conn.local_address.substr(colon_pos + 1);
                    try
                    {
                        return stoi(port_str) == port;
                    }
                    catch (...)
                    {
                        return false;
                    }
                }
                return false;
            });
    return result;
}

void ConnectionMonitor::printConnections(const vector<NetworkConnection> &connections)
{
    cout << "Found " << connections.size() << " connections" << endl;

    if (connections.empty())
    {
        cout << "No connections found." << endl;
        return;
    }

    cout << left
         << setw(8) << "Proto"
         << setw(25) << "Local Address"
         << setw(25) << "Remote Address"
         << setw(15) << "State"
         << setw(8) << "PID"
         << setw(15) << "Process"
         << setw(10) << "Inode"
         << endl;
    cout << string(100, '-') << endl;

    for (const auto &conn : connections)
    {
        cout << left
             << setw(8) << conn.protocol
             << setw(25) << conn.local_address
             << setw(25) << conn.remote_address
             << setw(15) << conn.state
             << setw(8) << (conn.pid > 0 ? to_string(conn.pid) : "N/A")
             << setw(15) << (conn.process_name.length() > 14 ? conn.process_name.substr(0, 14) : conn.process_name)
             << setw(10) << conn.inode
             << endl;
    }
}

map<string, int> ConnectionMonitor::getConnectionStatistics()
{
    auto all_connections = getAllConnections();
    map<string, int> stats;
    // статистика по протоколам
    stats["TCP"] = filterByProtocol(all_connections, "TCP").size();
    stats["UDP"] = filterByProtocol(all_connections, "UDP").size();
    // статистика по состояниям

    auto tcp_connections = filterByProtocol(all_connections, "TCP");
    for (const auto &conn : tcp_connections)
    {
        stats[conn.state]++;
    }
    // общее количество
    stats["TOTAL"] = all_connections.size();
    return stats;
}
