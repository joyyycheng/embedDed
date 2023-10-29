#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/netstack.h"
#include "pico/sock/pico_socket.h"
#include "pico/time.h"

// Set your wifi ssid and password here
#define WIFI_SSID "Koi_Pond"
#define WIFI_PASSWORD "K01TH3f1shB@L!"

// Set the server address here like "1.2.3.4"
#define SERVER_ADDR "127.0.0.1"

// These constants should match the server
#define BUF_SIZE 2048
#define SERVER_PORT 4242
#define TEST_ITERATIONS 10

int main() {
    stdio_init_all();

    // Initialize networking stack
    net_init();

    // Connect to Wi-Fi
    int wifi_status = pico_net_wifi_connect(WIFI_SSID, WIFI_PASSWORD);
    if (wifi_status != PICO_NET_SUCCESS) {
        printf("Failed to connect to Wi-Fi, error code: %d\n", wifi_status);
        return wifi_status;
    } else {
        printf("Connected to Wi-Fi\n");
        char ip_address[16];
        pico_net_get_ip_address(ip_address, sizeof(ip_address));
        printf("IP Address: %s\n", ip_address);
    }

    // Create socket to the server
    struct pico_socket* sock = pico_socket_open(PICO_PROTO_IPV4, PICO_PROTO_TCP);
    if (!sock) {
        printf("Failed to create socket\n");
        return -1;
    }

    // Set up server address
    struct pico_ip4 addr;
    pico_string_to_ipv4(SERVER_ADDR, &addr);
    addr.addr = pico_htonl(addr.addr);
    struct pico_address server_address;
    server_address.addr.ipv4 = addr;

    // Connect to the server
    int connect_status = pico_socket_connect(sock, &server_address, SERVER_PORT);
    if (connect_status != 0) {
        printf("Failed to connect to server, error code: %d\n", connect_status);
        pico_socket_close(sock);
        return connect_status;
    } else {
        printf("Connected to server\n");
    }

    // Repeat test for a number of iterations
    for (int test_iteration = 0; test_iteration < TEST_ITERATIONS; ++test_iteration) {
        // Read BUF_SIZE bytes from the server
        uint8_t read_buf[BUF_SIZE];
        int read_len = pico_socket_recv(sock, read_buf, BUF_SIZE, 0);
        if (read_len <= 0) {
            printf("Error reading from server\n");
            break;
        }
        printf("Read %d bytes from server\n", read_len);

        // Send the data back to the server
        int write_len = pico_socket_send(sock, read_buf, read_len, 0);
        if (write_len != read_len) {
            printf("Error writing to server\n");
            break;
        }
        printf("Written %d bytes to server\n", write_len);
    }

    // Close the socket
    pico_socket_close(sock);
    printf("Test completed\n");

    return 0;
}
