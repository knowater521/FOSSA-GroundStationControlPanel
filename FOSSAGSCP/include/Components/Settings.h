#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <string>
#include <cstdint>

////////////////
/// Settings ///
////////////////
class Settings
{
public:
    virtual ~Settings() {}

    QString GetPortName() { return m_portName; }
    std::string GetCallsign() { return m_callsign; }

    void LoadKeyFromFile(char* filepath)
    {
        FILE* f = fopen(filepath, "r");

        if (f == nullptr)
        {
            throw std::runtime_error("Could not find key.txt");
        }

        uint8_t tempKey[16];

        for (int i = 0; i < 32; i+=2)
        {
            char asciiCharacterA = getc(f);
            char asciiCharacterB = getc(f);
            char byteHexStr[3];

            byteHexStr[0] = asciiCharacterA;
            byteHexStr[1] = asciiCharacterB;
            byteHexStr[2] = '\0';

            uint8_t hexValueAsByte= (uint8_t)strtol(byteHexStr, NULL, 16);
            tempKey[i/2] = hexValueAsByte;
        }

        fclose(f);

        this->SetKey(tempKey);
    }

    uint8_t* GetKey() { return m_key; }
    bool IsKeySet() { return m_keySet; }

    std::string GetPassword() { return m_password; }
    bool IsPasswordSet() { return m_passwordSet; }




    void SetPortName(QString portName)
    {
        m_portName = portName;
    }
    void SetCallsign(std::string callsign)
    {
        m_callsign = callsign;
    }
    void SetKey(uint8_t* key)
    {
        for (int i = 0; i < 16; i++)
        {
            uint8_t v = key[i];
            m_key[i] = v;
        }
    }
    void SetKeySet()
    {
        m_keySet = true;
    }
    void SetPassword(std::string password)
    {
        m_password = password;
    }
    void SetPasswordSet()
    {
        m_passwordSet = true;
    }
private:
    // selected port name.
    QString m_portName;
    std::string m_callsign = "FOSSASAT-1"; /// @todo implement a menu for this.

    // AES KEY
    uint8_t m_key[16];
    bool m_keySet = false;

    // Password.
    std::string m_password = "";
    bool m_passwordSet = false;
};

#endif // SETTINGS_H
