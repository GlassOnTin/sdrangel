/**
 * SDRangel
 * This is the web REST/JSON API of SDRangel SDR software. SDRangel is an Open Source Qt5/OpenGL 3.0+ (4.3+ in Windows) GUI and server Software Defined Radio and signal analyzer in software. It supports Airspy, BladeRF, HackRF, LimeSDR, PlutoSDR, RTL-SDR, SDRplay RSP1 and FunCube    ---   Limitations and specifcities:    * In SDRangel GUI the first Rx device set cannot be deleted. Conversely the server starts with no device sets and its number of device sets can be reduced to zero by as many calls as necessary to /sdrangel/deviceset with DELETE method.   * Preset import and export from/to file is a server only feature.   * Device set focus is a GUI only feature.   * The following channels are not implemented (status 501 is returned): ATV and DATV demodulators, Channel Analyzer NG, LoRa demodulator   * The device settings and report structures contains only the sub-structure corresponding to the device type. The DeviceSettings and DeviceReport structures documented here shows all of them but only one will be or should be present at a time   * The channel settings and report structures contains only the sub-structure corresponding to the channel type. The ChannelSettings and ChannelReport structures documented here shows all of them but only one will be or should be present at a time    --- 
 *
 * OpenAPI spec version: 7.0.0
 * Contact: f4exb06@gmail.com
 *
 * NOTE: This class is auto generated by the swagger code generator program.
 * https://github.com/swagger-api/swagger-codegen.git
 * Do not edit the class manually.
 */

/*
 * SWGRadiosondeDemodSettings.h
 *
 * RadiosondeDemod
 */

#ifndef SWGRadiosondeDemodSettings_H_
#define SWGRadiosondeDemodSettings_H_

#include <QJsonObject>


#include "SWGChannelMarker.h"
#include "SWGGLScope.h"
#include "SWGRollupState.h"
#include <QString>

#include "SWGObject.h"
#include "export.h"

namespace SWGSDRangel {

class SWG_API SWGRadiosondeDemodSettings: public SWGObject {
public:
    SWGRadiosondeDemodSettings();
    SWGRadiosondeDemodSettings(QString* json);
    virtual ~SWGRadiosondeDemodSettings();
    void init();
    void cleanup();

    virtual QString asJson () override;
    virtual QJsonObject* asJsonObject() override;
    virtual void fromJsonObject(QJsonObject &json) override;
    virtual SWGRadiosondeDemodSettings* fromJson(QString &jsonString) override;

    qint32 getBaud();
    void setBaud(qint32 baud);

    qint64 getInputFrequencyOffset();
    void setInputFrequencyOffset(qint64 input_frequency_offset);

    float getRfBandwidth();
    void setRfBandwidth(float rf_bandwidth);

    float getFmDeviation();
    void setFmDeviation(float fm_deviation);

    float getCorrelationThreshold();
    void setCorrelationThreshold(float correlation_threshold);

    qint32 getUdpEnabled();
    void setUdpEnabled(qint32 udp_enabled);

    QString* getUdpAddress();
    void setUdpAddress(QString* udp_address);

    qint32 getUdpPort();
    void setUdpPort(qint32 udp_port);

    QString* getLogFilename();
    void setLogFilename(QString* log_filename);

    qint32 getLogEnabled();
    void setLogEnabled(qint32 log_enabled);

    qint32 getUseFileTime();
    void setUseFileTime(qint32 use_file_time);

    qint32 getRgbColor();
    void setRgbColor(qint32 rgb_color);

    QString* getTitle();
    void setTitle(QString* title);

    qint32 getStreamIndex();
    void setStreamIndex(qint32 stream_index);

    qint32 getUseReverseApi();
    void setUseReverseApi(qint32 use_reverse_api);

    QString* getReverseApiAddress();
    void setReverseApiAddress(QString* reverse_api_address);

    qint32 getReverseApiPort();
    void setReverseApiPort(qint32 reverse_api_port);

    qint32 getReverseApiDeviceIndex();
    void setReverseApiDeviceIndex(qint32 reverse_api_device_index);

    qint32 getReverseApiChannelIndex();
    void setReverseApiChannelIndex(qint32 reverse_api_channel_index);

    SWGGLScope* getScopeConfig();
    void setScopeConfig(SWGGLScope* scope_config);

    SWGChannelMarker* getChannelMarker();
    void setChannelMarker(SWGChannelMarker* channel_marker);

    SWGRollupState* getRollupState();
    void setRollupState(SWGRollupState* rollup_state);


    virtual bool isSet() override;

private:
    qint32 baud;
    bool m_baud_isSet;

    qint64 input_frequency_offset;
    bool m_input_frequency_offset_isSet;

    float rf_bandwidth;
    bool m_rf_bandwidth_isSet;

    float fm_deviation;
    bool m_fm_deviation_isSet;

    float correlation_threshold;
    bool m_correlation_threshold_isSet;

    qint32 udp_enabled;
    bool m_udp_enabled_isSet;

    QString* udp_address;
    bool m_udp_address_isSet;

    qint32 udp_port;
    bool m_udp_port_isSet;

    QString* log_filename;
    bool m_log_filename_isSet;

    qint32 log_enabled;
    bool m_log_enabled_isSet;

    qint32 use_file_time;
    bool m_use_file_time_isSet;

    qint32 rgb_color;
    bool m_rgb_color_isSet;

    QString* title;
    bool m_title_isSet;

    qint32 stream_index;
    bool m_stream_index_isSet;

    qint32 use_reverse_api;
    bool m_use_reverse_api_isSet;

    QString* reverse_api_address;
    bool m_reverse_api_address_isSet;

    qint32 reverse_api_port;
    bool m_reverse_api_port_isSet;

    qint32 reverse_api_device_index;
    bool m_reverse_api_device_index_isSet;

    qint32 reverse_api_channel_index;
    bool m_reverse_api_channel_index_isSet;

    SWGGLScope* scope_config;
    bool m_scope_config_isSet;

    SWGChannelMarker* channel_marker;
    bool m_channel_marker_isSet;

    SWGRollupState* rollup_state;
    bool m_rollup_state_isSet;

};

}

#endif /* SWGRadiosondeDemodSettings_H_ */
