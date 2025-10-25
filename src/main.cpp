#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <stdlib.h>
#include <iomanip>
#include "../include/ConnectionMonitor.h"

using namespace std;

void sectionMenu()
{
    cout << string(100, '-');
    cout << "===Network-Monitor===" << endl;

    cout << "1.TCP" << endl;
    cout << "2.UDP" << endl;
    cout << "3.All" << endl;
    cout << "4.Port" << endl;
    cout << "5.Stat" << endl;
    cout << "6.Exit" << endl;
}
bool showMenu(ConnectionMonitor &monitor)
{

    string choiceUser;
    auto all_connections = monitor.getAllConnections();

    cout << "Enter section: ";
    cin >> choiceUser;

    vector<string> all_port = {"22", "23", "80", "443", "25", "21"};

    if ((choiceUser == "TCP") || (choiceUser == "tcp") || (choiceUser == "1"))
    {
        cout << "===TCP CONNECTION===" << endl;
        auto tcp_listen = monitor.filterByProtocol(
            monitor.filterByState(all_connections, "LISTEN"), "TCP");
        monitor.printConnections(tcp_listen);
    }
    else if ((choiceUser == "UDP") || (choiceUser == "udp") || (choiceUser == "2"))
    {
        cout << "===UDP CONNECTION===" << endl;
        auto udp_listen = monitor.filterByProtocol(all_connections, "UDP");
        monitor.printConnections(udp_listen);
    }
    else if ((choiceUser == "All") || (choiceUser == "all") || (choiceUser == "3"))
    {
        cout << "===ALL CONNECTION===" << endl;
        monitor.printConnections(all_connections);
    }
    else if ((choiceUser == "port") || (choiceUser == "Port") || (choiceUser == "4"))
    {
        cout << "===PORT===" << endl;
        string portChoice;
        cout << "Select port: 22(SSH),23(Telnet), 80 (HTTP), 443 (HTTPS), 25 (SMTP), 21 (FTP)" << endl;
        cout << "PORT: ";
        cin >> portChoice;

        if (find(all_port.begin(), all_port.end(), portChoice) != all_port.end())
        {

            int port_number = stoi(portChoice);
            auto port_listen = monitor.filterByPort(all_connections, port_number);
            cout << "You have selected a port: " << portChoice << endl;
            monitor.printConnections(port_listen));
        }
        else
        {
            cout << "Invalid port selection" << endl;
        }
    }
    else if ((choiceUser == "stat") || (choiceUser == "5"))
    {
        cout << "===STATISTICS===" << endl;
        auto stats = monitor.getConnectionStatistics();
        for (const auto &stat : stats)
        {
            cout << stat.first << ":" << stat.second << endl;
        }
    }
    else if ((choiceUser == "Exit") || (choiceUser == "exit") || (choiceUser == "6"))
    {
        cout << "Goodbye" << endl;
        return false;
    }
    else
    {
        cout << "Such a Directory dont search!Try again.";
    }
    return true;
}

int main()
{
    ConnectionMonitor monitor;
    bool running = true;

    while (running)
    {
        sectionMenu();
        running = showMenu();
    }
    return 0;
}
