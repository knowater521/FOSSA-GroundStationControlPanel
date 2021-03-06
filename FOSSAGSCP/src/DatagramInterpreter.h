#ifndef Interpreter_H
#define Interpreter_H

////////////////////////////////
/// INTERPRETER              ///
/// 25/06/2020 - R.G.Bamford ///
/// 07/07/2020 - R.G.Bamford ///
/// 29/07/2020 - R.G.Bamford - Refactored into InboundDatagram, OutboundDatagram. //
////////////////////////////////

#include "InboundDatagram.h"
#include "OutboundDatagram.h"
#include "FCPFrame.h"

#include "Settings.h"
#include "CStructs.h"

#include "ui_mainwindow.h"
#include "ui_systeminformationpane.h"
#include "ui_messagelogframe.h"
#include "messagelogframe.h"

#include <FOSSA-Comms.h>
#include <QObject>
#include <vector>
#include <QDebug>

#include "VersionStrings.h"

/////////////////////
/// Direction Bit ///
/////////////////////
#define FCPI_DIR_TO_GROUND_STATION  0
#define FCPI_DIR_FROM_GROUND_STATION 1

////////////////////
/// Operation ID ///
////////////////////
//
// Handshake request/response. Sent from control panel to initialize the serial protocol,
// ground station sends this operation ID in response.
//
#define FCPI_HANDSHAKE_OP 0 // handshake

//
// FCP frame transfer. Payload is only the FCP frame to uplink when sent with direction = 0,
// or 2-byte RadioLib reception status code followed by the received FCP frame when sent with direction = 1.
//
#define FCPI_FRAME_OP 1 // fcp frame

//
// Ground station configuration change request/result. Payload is the ground station configuration to set when sent with direction = 0,
// or configuration result when sent with direction = 1 (typically a 2-byte RadioLib status code).
//
#define FCPI_CONFIG_OP 2
///
/// \brief The DatagramInterpreter class  takes the raw serial information and interprets it into a datagram.
///
class DatagramInterpreter : public QObject
{
    Q_OBJECT
public:
    DatagramInterpreter(Ui::MainWindow* mainWindowUI, MessageLogFrame* messageLogFrame);
    virtual ~DatagramInterpreter() {}

    void SetSystemInformationPane(Ui::systeminformationpane* systemInfoUI);

    IDatagram *SerialData_To_Datagram(QByteArray datagramData);
    IDatagram *Create_Datagram(char operationId, uint8_t functionId, uint8_t optDataLength, char *optData);

    void Interpret_Received_Datagram(IDatagram* datagram);
    void Interpret_Handshake(IDatagram* datagram);
    void Interpret_Config_Change(IDatagram* datagram);
    void Interpret_FCP_Frame(IDatagram* datagram);

    //////
    /// public unencrypted uplink message commands.
    /////
    IDatagram* Create_CMD_Ping();
    IDatagram* Create_CMD_Retransmit(uint32_t senderId, char * message);
    IDatagram* Create_CMD_Retransmit_Custom(uint8_t bandwidth, uint8_t spreadingFactor, uint8_t codingRate, uint16_t preambleSymbolLength, uint8_t crcEnabled, int8_t outputPower, uint32_t senderId, char * message);
    IDatagram* Create_CMD_Transmit_System_Info();
    IDatagram* Create_CMD_Get_Packet_Info();
    /// flagsB is for FOSSASAT-1B and flags is for FOSSASAT-2
    IDatagram *Create_CMD_Get_Statistics(char flagsB, char flags);
    IDatagram* Create_CMD_Get_Full_System_Info();
    IDatagram* Create_CMD_Store_And_Forward_Add(uint32_t messageId, char * message);
    IDatagram* Create_CMD_Store_And_Forward_Request(uint32_t messageId);
    IDatagram* Create_CMD_Request_Public_Picture(uint8_t pictureSlot, uint16_t picturePacketId); // only 80-100 slot numbers.

    /////
    /// Private commands (encrypted uplink messages)
    /////
    IDatagram* Create_CMD_Deploy();
    IDatagram* Create_CMD_Restart();
    IDatagram *Create_CMD_Wipe_EEPROM(char flags);
    IDatagram* Create_CMD_Set_Transmit_Enable(char transmitEnabled, char autoStatsEnabled, char fskMandatedEnabled);
    IDatagram* Create_CMD_Set_Callsign(char* callsign);
    IDatagram* Create_CMD_Set_SF_Mode(uint8_t spreadingFactor);
    IDatagram* Create_CMD_Set_MPPT_Mode(uint8_t temperatureSwitchEnabled, uint8_t keepAliveEnabled);
    IDatagram* Create_CMD_Set_Low_Power_Mode_Enable(uint8_t lowPowerModeEnabled);
    IDatagram* Create_CMD_Set_Receive_Windows(uint8_t fskReceiveWindowLength, uint8_t loraReceiveWindowLength);
    IDatagram* Create_CMD_Record_Solar_Cells(char numSamples, uint16_t samplingPeriod);
    IDatagram* Create_CMD_Camera_Capture(char pictureSlot, char lightMode, char pictureSize, char brightness, char saturation, char specialFilter, char contrast);
    IDatagram* Create_CMD_Set_Power_Limits(int16_t deploymentVoltageLimit, int16_t heaterVoltageLimit, int16_t cwBeepVoltageLimit, int16_t lowPowerVoltageLimit,  float heaterTemperatureLimit, float mpptSwitchtemperatureLimit, uint8_t heaterDutyCycle);
    IDatagram* Create_CMD_Set_RTC(uint8_t year, uint8_t month, uint8_t day, uint8_t dayOfWeek, uint8_t hours, uint8_t minutes, uint8_t seconds); // year is offset from 2000.
    IDatagram* Create_CMD_Record_IMU(uint16_t sampleNumber, uint16_t samplingPeriod, uint8_t flags);
    IDatagram* Create_CMD_Run_Manual_ACS(int8_t xHighBridgeMag, int8_t xLowBridgeMag, int8_t yHighBridgeMag, int8_t yLowBrightMag, int8_t zHBridgeHighMag, int8_t zBridgeLowMag, uint32_t xPulseLength, uint32_t yPulseLength, uint32_t zPulseLength, uint32_t maneuverDuration, uint8_t hbridgefaultFlags);
    IDatagram* Create_CMD_Log_GPS(uint32_t gpsLoggingDuration, uint32_t gpsLoggingStartOffset);
    IDatagram* Create_CMD_Get_GPS_Log(uint8_t newestEntriesFirst, uint16_t gpsLogOffsetAsNMEASeq, uint16_t NMEASentencesToDownlink); // NMEA sentences set to 0 means entire log.
    IDatagram* Create_CMD_Get_Flash_Contents(uint32_t flashAddress, uint8_t numBytes);
    IDatagram *Create_CMD_Get_Picture_Length(uint8_t pictureSlot);
    IDatagram* Create_CMD_Get_Picture_Burst(char pictureSlot, uint16_t pictureId, char fullOrScandata);
    IDatagram* Create_CMD_Route(char* fcpFrame);
    IDatagram* Create_CMD_Set_Flash_Contents(uint32_t flashAddress, char* data); // data MUST include a null terminator
    IDatagram* Create_CMD_Set_TLE(char* tle); // MUST include a null terminator.
    IDatagram* Create_CMD_Get_GPS_Log_State();
    IDatagram* Create_CMD_Run_GPS_Command(char* skyTraqBinaryProtocolMessage);
    IDatagram* Create_CMD_Set_Sleep_Intervals(std::vector<sleep_interval_t> sleepIntervals);
    IDatagram* Create_CMD_Abort();
    IDatagram* Create_CMD_Maneuver(uint8_t controlFlags, uint32_t maneuverLength);
    IDatagram* Create_CMD_Set_ADCS_Parameters(float maximumPulseIntensity, float maximumPulseLength, float detumblingAngularVelocity, float minimumIntertialMoment, float pulseAmplitude, float calculationTolerance, float activeControlEulerAngleChangeTolerance, float activeControlAngularVelocityChangeTolerance, float eclipseThreshold, float rotationMatrixWeightRatio, float rotationVerificationTriggerLevel, float kalmanFilterDisturbanceCovariance, float kalmanFilterNoiseCovariance, uint32_t adcsUpdateTimeStepPeriod, uint32_t hbridgeTimerUpdatePeriod, int8_t hbridgeValueHighOutput, int8_t hbridgeValueLowOutput, uint8_t numControllers);
    IDatagram* Create_CMD_Erase_Flash(uint32_t sectorAddress);
    IDatagram *Create_CMD_Set_ADCS_Controller(char controllerId, float controllerMatrix[3][6]);
    IDatagram* Create_CMD_Set_ADCS_Ephemerides(uint16_t chunkId, std::vector<ephemerides_t> ephemeridesDataQueue);
    IDatagram* Create_CMD_Detumble(uint8_t flags, uint32_t length);
    IDatagram* Create_CMD_Set_IMU_Offset(uint8_t xAxisGyroOffset, uint8_t yAxisGyroOffset, uint8_t zAxisGyroOffset, uint8_t xAxisAcceleOffset, uint8_t yAxisAcceleOffset, uint8_t zAxisAcceleOffset, uint8_t xAxisMagneticOffset, uint8_t yAxisMagneticOffset, uint8_t zAxisMagneticOffset);
    IDatagram* Create_CMD_Set_IMU_Calibration(float transformationMatrix[9], float biasVector[3]);
signals:
    void ReceivedHandshake();
    void ReceivedSatelliteSystemInformation(IDatagram* datagram);
protected:
    Ui::MainWindow* m_mainWindowUI = nullptr;
    Ui::systeminformationpane* m_systemInfoUI = nullptr;
    MessageLogFrame* m_messageLog = nullptr;
};

#endif // Interpreter_H
