# Wifi SSID / Password

create a `secrets.h` file in the `include/` directory:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

const char* SECRET_SSID = "SSID";
const char* SECRET_PASSWORD = "PASS";
#endif
```

change the `platformio.ini` to allow the OTA upload:
```ini
upload_protocol = espota
upload_port = 10.0.1.76
```