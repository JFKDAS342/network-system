#include <iostream>
#include "../include/ConnectionMonitor.h"

using namespace std;

// Убрана неиспользуемая функция showMenu()
// int showMenu(); // Эта функция объявлена, но не определена и не используется

int main()
{
    ConnectionMonitor monitor;

    // Получить все соединения
    auto all_connections = monitor.getAllConnections();
    cout << "=== ALL CONNECTIONS ===" << endl;
    monitor.printConnections(all_connections);

    // Получить статистику
    auto stats = monitor.getConnectionStatistics();
    cout << "\n=== STATISTICS ===" << endl;
    for (const auto &stat : stats)
    {
        cout << stat.first << ": " << stat.second << endl;
    }

    // Фильтрация примеров
    cout << "\n=== TCP LISTENING PORTS ===" << endl;
    auto tcp_listen = monitor.filterByProtocol(
        monitor.filterByState(all_connections, "LISTEN"), "TCP");
    monitor.printConnections(tcp_listen);

    cout << "\n=== UDP CONNECTIONS ===" << endl;
    auto udp_connections = monitor.filterByProtocol(all_connections, "UDP");
    monitor.printConnections(udp_connections);

    // Поиск по порту (например, порт 22 для SSH)
    cout << "\n=== PORT 22 CONNECTIONS ===" << endl;
    auto port_22 = monitor.filterByPort(all_connections, 22);
    monitor.printConnections(port_22);

    return 0;
}