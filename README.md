# GSM-Based-Smart-Notice-Board

This was a College mini-project submitted @RNSIT during 5th semester; Project was on a GSM and WiFi based Smart notice board integrated with API based controller called Blynk widget app. This prototype also integrated ChatGPT API for text generation based on the user input. And also allows the user to control the display and other system setting via a slider and terminal widget.  

| Firmware        | Link                                                                                     |
|------------------|------------------------------------------------------------------------------------------|
| FIRMWARE for GSM | [Download](https://github.com/Sanjay0302/GSM-Based-Smart-Notice-Board/releases/tag/v0.2.0) |
| FIRMWARE for WIFI| [Download](https://github.com/Sanjay0302/GSM-Based-Smart-Notice-Board/releases/tag/v0.1.1)|
| FIRMWARE for GPT | [Download](https://github.com/Sanjay0302/GSM-Based-Smart-Notice-Board/blob/main/Programs/openai/ver7.ino) |


| Dependency                | Author                     | Version  |
|:---------------------------|:----------------------------|:----------:|
| Blynk                     | Volodymyr Shymanskyy      | 1.1.0    |
| BlynkGSM_Manager          | Khoi Hoang                 | 1.2.0    |
| MD MAX72XX                | majicDesigns               | 3.4.1    |
| MD_Parola                 | majicDesigns               | 3.6.2    |
| TinyGSM                   | Volodymyr Shymanskyy      | 0.11.3   |

| LED Connection   | ESP Pin |
|:--------------|:---------|
| VCC          | 3v3    |
| GND          | gnd    |
| DATA_PIN     | 19     |
| CS_PIN       | 5      |
| CLK_PIN      | 18     |

| SIM800L Connection | ESP Pin        |
|:------------|:----------------|
| VCC        | BAT 3.7v      |
| GND        | Bat GND & ESP GND |
| TX         | RX2 of ESP32   |
| RX         | TX2 of ESP32   |

