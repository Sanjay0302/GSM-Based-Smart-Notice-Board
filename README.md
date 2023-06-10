# GSM-Based-Smart-Notice-Board

- [FIRMWARE for GSM](https://github.com/Sanjay0302/GSM-Based-Smart-Notice-Board/releases/tag/v0.2.0)
 
- [FIRMWARE for WIFI](https://github.com/Sanjay0302/GSM-Based-Smart-Notice-Board/releases/tag/v0.1.1)

- [FIRMWARE for GPT](https://github.com/Sanjay0302/GSM-Based-Smart-Notice-Board/blob/main/Programs/openai/ver7.ino)


### Dependencies

- **Blynk** by Volodymyr Shymanskyy Version 1.1.0
- **BlynkGSM_Manager** by Khoi Hoang Version 1.2.0
- **MD MAX72XX** by majicDesigns Version 3.4.1
- **MD_Parola** by majicDesigns Version 3.6.2
- **TinyGSM** by Volodymyr Shymanskyy Version 0.11.3

### Led Connections
// vcc - Esp32 3v3 - Grey
//gnd  - Esp32 gnd - Red
#define DATA_PIN 19  // orange 
#define CS_PIN 5     // yellow
#define CLK_PIN 18   // green

### SIM800L connection
Vcc - Battery 3.7v
Gnd - Battery gnd - Esp32 Gnd
Tx - Rx2 of Esp32
Rx - Tx2 of Esp32
