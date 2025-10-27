#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <stdlib.h>
#include <iomanip>
#include <unistd.h>
#include <chrono>
#include <thread>
#include "../include/ConnectionMonitor.h"

using namespace std;
void printDelay(const string &text, int mcDelay = 80)
{
    for (char c : text)
    {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(mcDelay));
    }
    cout << endl;
}

void sectionMenu()
{

    cout << string(60, '-') << endl;

    printDelay("===NetworkMonitor===");

    cout << "1. TCP Connections" << endl;
    cout << "2. UDP Connections" << endl;
    cout << "3. All Connections" << endl;
    cout << "4. Filter by Port" << endl;
    cout << "5. Statistics" << endl;
    cout << "6. Real-time Monitoring" << endl;
    cout << "7. Sorted View" << endl;
    cout << "8. Exit" << endl;
    cout << string(100, '-') << endl;
}
void showMenu(ConnectionMonitor &monitor)
{
    string choiceUser;
    auto all_connections = monitor.getAllConnections();

    cout << "Enter choice: ";
    cin >> choiceUser;

    vector<string> all_port = {"22", "23", "80", "443", "25", "21"};

    if ((choiceUser == "TCP") || (choiceUser == "tcp") || (choiceUser == "1"))
    {
        printDelay("===TCP CONNECTION===");

        auto tcp_connections = monitor.filterByProtocol(all_connections, "TCP");
        monitor.printConnections(tcp_connections, true); // С цветом
    }
    else if ((choiceUser == "UDP") || (choiceUser == "udp") || (choiceUser == "2"))
    {
        cout << "=== UDP CONNECTIONS ===" << endl;
        auto udp_connections = monitor.filterByProtocol(all_connections, "UDP");
        monitor.printConnections(udp_connections, true); // С цветом
    }
    else if ((choiceUser == "All") || (choiceUser == "all") || (choiceUser == "3"))
    {
        cout << "=== ALL CONNECTIONS ===" << endl;
        monitor.printConnections(all_connections, true); // С цветом
    }
    else if ((choiceUser == "port") || (choiceUser == "Port") || (choiceUser == "4"))
    {
        cout << "=== FILTER BY PORT ===" << endl;
        string portChoice;
        cout << "Select port: 22(SSH), 23(Telnet), 80(HTTP), 443(HTTPS), 25(SMTP), 21(FTP)" << endl;
        cout << "PORT: ";
        cin >> portChoice;

        if (find(all_port.begin(), all_port.end(), portChoice) != all_port.end())
        {
            int port_number = stoi(portChoice);
            auto port_connections = monitor.filterByPort(all_connections, port_number);
            cout << "Port " << portChoice << " connections:" << endl;
            monitor.printConnections(port_connections, true); // С цветом
        }
        else
        {
            cout << "Invalid port selection" << endl;
        }
    }
    else if ((choiceUser == "stat") || (choiceUser == "5"))
    {
        cout << "=== STATISTICS ===" << endl;
        auto stats = monitor.getConnectionStatistics();
        for (const auto &stat : stats)
        {
            cout << stat.first << ": " << stat.second << endl;
        }
    }
    else if ((choiceUser == "realtime") || (choiceUser == "6"))
    {

        monitor.realTimeMonitoring(30, 2);
    }
    else if ((choiceUser == "sorted") || (choiceUser == "7"))
    {
        cout << "=== SORTED VIEW ===" << endl;
        cout << "Sort by: 1-Local Address, 2-State, 3-PID: ";
        string sortChoice;
        cin >> sortChoice;

        vector<NetworkConnection> sorted;
        if (sortChoice == "1")
        {
            sorted = monitor.sortByLocalAddress(all_connections);
            cout << "Sorted by Local Address:" << endl;
        }
        else if (sortChoice == "2")
        {
            sorted = monitor.sortByState(all_connections);
            cout << "Sorted by State:" << endl;
        }
        else if (sortChoice == "3")
        {
            sorted = monitor.sortByPID(all_connections);
            cout << "Sorted by PID:" << endl;
        }
        else
        {
            cout << "Invalid sort option" << endl;
            return;
        }
        monitor.printConnections(sorted, true);
    }
    else if ((choiceUser == "Exit") || (choiceUser == "exit") || (choiceUser == "8"))
    {
        cout << "Goodbye!" << endl;
        exit(0);
    }
    else
    {
        cout << "Invalid option! Try again." << endl;
    }
}

int main()
{
    ConnectionMonitor monitor;
    bool running = true;

    cout << "NETWORK MONITOR STARTED!" << endl;
    cout << "Features: Color coding, Real-time monitoring, Sorting" << endl;

    while (running)
    {
        sectionMenu();
        showMenu(monitor);
    }
    return 0;
}
