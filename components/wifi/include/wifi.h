#ifndef WIFI_H
#define WIFI_H

#define WIFI_SSID "esp"
#define WIFI_PASS "123456789"
#define WIFI_CHAN 1
#define MAX_STA_CONN 10

void wifi_init_softap(void);
void start_wifi(void);
#endif // !WIFI_H
#define WIFI_H
