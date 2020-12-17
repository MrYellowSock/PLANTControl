#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define indicator_led 12
#define server_live_minute 2
#define AP_con_timeout 10
const char* ssid     = "TP-Link@Home";
const char* password = "KuLKaMThorN";
const char* host = "script.google.com";
const int httpsPort = 443;
const char trustRoot[] PROGMEM = R"EOF(-----BEGIN CERTIFICATE-----
MIIJcDCCCFigAwIBAgIRANKfCY1Ja0QvCAAAAABdwbEwDQYJKoZIhvcNAQELBQAw
QjELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFUdvb2dsZSBUcnVzdCBTZXJ2aWNlczET
MBEGA1UEAxMKR1RTIENBIDFPMTAeFw0yMDEwMDYwNjM1NTdaFw0yMDEyMjkwNjM1
NTdaMGYxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYDVQQH
Ew1Nb3VudGFpbiBWaWV3MRMwEQYDVQQKEwpHb29nbGUgTExDMRUwEwYDVQQDDAwq
Lmdvb2dsZS5jb20wWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASCjourz6E67XXR
aWGDDSy/jVZun+uB+Cj5gD3hengXnPlO8b8ZFPQOWYn3hGe6XWo7t6djTFrfl7dc
BYDAdevpo4IHBjCCBwIwDgYDVR0PAQH/BAQDAgeAMBMGA1UdJQQMMAoGCCsGAQUF
BwMBMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFPs4hsH0rZ+Y1wlik+0kvfZKpL0m
MB8GA1UdIwQYMBaAFJjR+G4Q68+b7GCfGJAboOt9Cf0rMGgGCCsGAQUFBwEBBFww
WjArBggrBgEFBQcwAYYfaHR0cDovL29jc3AucGtpLmdvb2cvZ3RzMW8xY29yZTAr
BggrBgEFBQcwAoYfaHR0cDovL3BraS5nb29nL2dzcjIvR1RTMU8xLmNydDCCBMIG
A1UdEQSCBLkwggS1ggwqLmdvb2dsZS5jb22CDSouYW5kcm9pZC5jb22CFiouYXBw
ZW5naW5lLmdvb2dsZS5jb22CCSouYmRuLmRldoISKi5jbG91ZC5nb29nbGUuY29t
ghgqLmNyb3dkc291cmNlLmdvb2dsZS5jb22CGCouZGF0YWNvbXB1dGUuZ29vZ2xl
LmNvbYIGKi5nLmNvgg4qLmdjcC5ndnQyLmNvbYIRKi5nY3BjZG4uZ3Z0MS5jb22C
CiouZ2dwaHQuY26CDiouZ2tlY25hcHBzLmNughYqLmdvb2dsZS1hbmFseXRpY3Mu
Y29tggsqLmdvb2dsZS5jYYILKi5nb29nbGUuY2yCDiouZ29vZ2xlLmNvLmlugg4q
Lmdvb2dsZS5jby5qcIIOKi5nb29nbGUuY28udWuCDyouZ29vZ2xlLmNvbS5hcoIP
Ki5nb29nbGUuY29tLmF1gg8qLmdvb2dsZS5jb20uYnKCDyouZ29vZ2xlLmNvbS5j
b4IPKi5nb29nbGUuY29tLm14gg8qLmdvb2dsZS5jb20udHKCDyouZ29vZ2xlLmNv
bS52boILKi5nb29nbGUuZGWCCyouZ29vZ2xlLmVzggsqLmdvb2dsZS5mcoILKi5n
b29nbGUuaHWCCyouZ29vZ2xlLml0ggsqLmdvb2dsZS5ubIILKi5nb29nbGUucGyC
CyouZ29vZ2xlLnB0ghIqLmdvb2dsZWFkYXBpcy5jb22CDyouZ29vZ2xlYXBpcy5j
boIRKi5nb29nbGVjbmFwcHMuY26CFCouZ29vZ2xlY29tbWVyY2UuY29tghEqLmdv
b2dsZXZpZGVvLmNvbYIMKi5nc3RhdGljLmNugg0qLmdzdGF0aWMuY29tghIqLmdz
dGF0aWNjbmFwcHMuY26CCiouZ3Z0MS5jb22CCiouZ3Z0Mi5jb22CFCoubWV0cmlj
LmdzdGF0aWMuY29tggwqLnVyY2hpbi5jb22CECoudXJsLmdvb2dsZS5jb22CEyou
d2Vhci5na2VjbmFwcHMuY26CFioueW91dHViZS1ub2Nvb2tpZS5jb22CDSoueW91
dHViZS5jb22CFioueW91dHViZWVkdWNhdGlvbi5jb22CESoueW91dHViZWtpZHMu
Y29tggcqLnl0LmJlggsqLnl0aW1nLmNvbYIaYW5kcm9pZC5jbGllbnRzLmdvb2ds
ZS5jb22CC2FuZHJvaWQuY29tghtkZXZlbG9wZXIuYW5kcm9pZC5nb29nbGUuY26C
HGRldmVsb3BlcnMuYW5kcm9pZC5nb29nbGUuY26CBGcuY2+CCGdncGh0LmNuggxn
a2VjbmFwcHMuY26CBmdvby5nbIIUZ29vZ2xlLWFuYWx5dGljcy5jb22CCmdvb2ds
ZS5jb22CD2dvb2dsZWNuYXBwcy5jboISZ29vZ2xlY29tbWVyY2UuY29tghhzb3Vy
Y2UuYW5kcm9pZC5nb29nbGUuY26CCnVyY2hpbi5jb22CCnd3dy5nb28uZ2yCCHlv
dXR1LmJlggt5b3V0dWJlLmNvbYIUeW91dHViZWVkdWNhdGlvbi5jb22CD3lvdXR1
YmVraWRzLmNvbYIFeXQuYmUwIQYDVR0gBBowGDAIBgZngQwBAgIwDAYKKwYBBAHW
eQIFAzAzBgNVHR8ELDAqMCigJqAkhiJodHRwOi8vY3JsLnBraS5nb29nL0dUUzFP
MWNvcmUuY3JsMIIBAwYKKwYBBAHWeQIEAgSB9ASB8QDvAHYAxlKg7EjOs/yrFwmS
xDqHQTMJ6ABlomJSQBujNioXxWUAAAF0/NafTwAABAMARzBFAiAdejREqnuijh/6
zo/i0+LoBOHMZey3o9RWZ9NvK6cp4QIhANZmAilX58AcB48F0B2NPnYr2RgJOs//
m1ge8bPo/yVgAHUAB7dcG+V9aP/xsMYdIxXHuuZXfFeUt2ruvGE6GmnTohwAAAF0
/NafdQAABAMARjBEAiBlZP3lCmUHeyavc4PGiSvp/bQzinLocqmHTqB3DTKtJwIg
RDUUs33DJSyNdVMvYL90+tZb5XYK8L4VzceleoL/ul0wDQYJKoZIhvcNAQELBQAD
ggEBAC8j2vGVmgwW3NbjP5b8R5c4KFAJ/fPm0kc9GLq0I6cT39GcnCCVR9cSgyjz
Do4euyRGRPdMPAo2L2wRkTvWadNglXmIJ7dBgNwYL7rz/h349Bqy43xs35F3TWtW
IcXTc8YgXJvQ2hsmekeKZmF/cYS7vdfRNqKp2muzc9kycTgS/6rg2R58hHBeRLGb
GZTpTJ4DGGMM8oWMhmpAKxy2x3Z98Kj7/ViS79aZKHPOsu/yTxC91Y8K09u7Oc8R
V04lE+8030NZgIiYTqRVtKPscGisQhS1D8wzRpEiN7srPgsOBg+zGFLhdJUzPthK
6Nv1PLhU15bifZp359UPMqaCuXQ=
-----END CERTIFICATE-----
)EOF";
X509List cert(trustRoot);

bool conn = false;
void checkNconnect(){
    if(WiFi.status() == WL_CONNECTED){
        return;
    }
    else
    {
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);

        int total = 0;
        while (WiFi.status() != WL_CONNECTED && total/1000 < AP_con_timeout) {
            digitalWrite(indicator_led,HIGH);
            delay(250);
            digitalWrite(indicator_led,LOW);
            delay(250);
            total += 500;
        }

        if(WiFi.status() != WL_CONNECTED){
            Serial.println("timed out, aborted!");
            conn = false;
        }
        else{
            Serial.print("connected, IP: ");
            Serial.println(WiFi.localIP());
            digitalWrite(indicator_led,HIGH);
            conn = true;
        }
    }
}
void httpGet(String URL){

  // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

    WiFiClientSecure client;
     client.setTrustAnchors(&cert);


    if (client.connect(host, httpsPort)) {  
        client.print(String("GET ") + URL + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
        while (client.connected()) {
            String line = client.readStringUntil('\n');
            if (line == "\r") {
                Serial.println("Headers received");
                break;
            }
        }
        String line = client.readStringUntil('\n');
        Serial.print("REPLY:");
        Serial.println(line);
        
    } else {
        Serial.println("CON FAILED.");
    }
}