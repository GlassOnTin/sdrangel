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


#include "SWGChannelActions.h"

#include "SWGHelpers.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>
#include <QDebug>

namespace SWGSDRangel {

SWGChannelActions::SWGChannelActions(QString* json) {
    init();
    this->fromJson(*json);
}

SWGChannelActions::SWGChannelActions() {
    channel_type = nullptr;
    m_channel_type_isSet = false;
    direction = 0;
    m_direction_isSet = false;
    originator_device_set_index = 0;
    m_originator_device_set_index_isSet = false;
    originator_channel_index = 0;
    m_originator_channel_index_isSet = false;
    ais_mod_actions = nullptr;
    m_ais_mod_actions_isSet = false;
    apt_demod_actions = nullptr;
    m_apt_demod_actions_isSet = false;
    file_sink_actions = nullptr;
    m_file_sink_actions_isSet = false;
    file_source_actions = nullptr;
    m_file_source_actions_isSet = false;
    freq_scanner_actions = nullptr;
    m_freq_scanner_actions_isSet = false;
    ieee_802_15_4_mod_actions = nullptr;
    m_ieee_802_15_4_mod_actions_isSet = false;
    packet_mod_actions = nullptr;
    m_packet_mod_actions_isSet = false;
    psk31_mod_actions = nullptr;
    m_psk31_mod_actions_isSet = false;
    radio_astronomy_actions = nullptr;
    m_radio_astronomy_actions_isSet = false;
    rtty_mod_actions = nullptr;
    m_rtty_mod_actions_isSet = false;
    sig_mf_file_sink_actions = nullptr;
    m_sig_mf_file_sink_actions_isSet = false;
}

SWGChannelActions::~SWGChannelActions() {
    this->cleanup();
}

void
SWGChannelActions::init() {
    channel_type = new QString("");
    m_channel_type_isSet = false;
    direction = 0;
    m_direction_isSet = false;
    originator_device_set_index = 0;
    m_originator_device_set_index_isSet = false;
    originator_channel_index = 0;
    m_originator_channel_index_isSet = false;
    ais_mod_actions = new SWGAISModActions();
    m_ais_mod_actions_isSet = false;
    apt_demod_actions = new SWGAPTDemodActions();
    m_apt_demod_actions_isSet = false;
    file_sink_actions = new SWGFileSinkActions();
    m_file_sink_actions_isSet = false;
    file_source_actions = new SWGFileSourceActions();
    m_file_source_actions_isSet = false;
    freq_scanner_actions = new SWGFreqScannerActions();
    m_freq_scanner_actions_isSet = false;
    ieee_802_15_4_mod_actions = new SWGIEEE_802_15_4_ModActions();
    m_ieee_802_15_4_mod_actions_isSet = false;
    packet_mod_actions = new SWGPacketModActions();
    m_packet_mod_actions_isSet = false;
    psk31_mod_actions = new SWGPSK31ModActions();
    m_psk31_mod_actions_isSet = false;
    radio_astronomy_actions = new SWGRadioAstronomyActions();
    m_radio_astronomy_actions_isSet = false;
    rtty_mod_actions = new SWGRTTYModActions();
    m_rtty_mod_actions_isSet = false;
    sig_mf_file_sink_actions = new SWGSigMFFileSinkActions();
    m_sig_mf_file_sink_actions_isSet = false;
}

void
SWGChannelActions::cleanup() {
    if(channel_type != nullptr) { 
        delete channel_type;
    }



    if(ais_mod_actions != nullptr) { 
        delete ais_mod_actions;
    }
    if(apt_demod_actions != nullptr) { 
        delete apt_demod_actions;
    }
    if(file_sink_actions != nullptr) { 
        delete file_sink_actions;
    }
    if(file_source_actions != nullptr) { 
        delete file_source_actions;
    }
    if(freq_scanner_actions != nullptr) { 
        delete freq_scanner_actions;
    }
    if(ieee_802_15_4_mod_actions != nullptr) { 
        delete ieee_802_15_4_mod_actions;
    }
    if(packet_mod_actions != nullptr) { 
        delete packet_mod_actions;
    }
    if(psk31_mod_actions != nullptr) { 
        delete psk31_mod_actions;
    }
    if(radio_astronomy_actions != nullptr) { 
        delete radio_astronomy_actions;
    }
    if(rtty_mod_actions != nullptr) { 
        delete rtty_mod_actions;
    }
    if(sig_mf_file_sink_actions != nullptr) { 
        delete sig_mf_file_sink_actions;
    }
}

SWGChannelActions*
SWGChannelActions::fromJson(QString &json) {
    QByteArray array (json.toStdString().c_str());
    QJsonDocument doc = QJsonDocument::fromJson(array);
    QJsonObject jsonObject = doc.object();
    this->fromJsonObject(jsonObject);
    return this;
}

void
SWGChannelActions::fromJsonObject(QJsonObject &pJson) {
    ::SWGSDRangel::setValue(&channel_type, pJson["channelType"], "QString", "QString");
    
    ::SWGSDRangel::setValue(&direction, pJson["direction"], "qint32", "");
    
    ::SWGSDRangel::setValue(&originator_device_set_index, pJson["originatorDeviceSetIndex"], "qint32", "");
    
    ::SWGSDRangel::setValue(&originator_channel_index, pJson["originatorChannelIndex"], "qint32", "");
    
    ::SWGSDRangel::setValue(&ais_mod_actions, pJson["AISModActions"], "SWGAISModActions", "SWGAISModActions");
    
    ::SWGSDRangel::setValue(&apt_demod_actions, pJson["APTDemodActions"], "SWGAPTDemodActions", "SWGAPTDemodActions");
    
    ::SWGSDRangel::setValue(&file_sink_actions, pJson["FileSinkActions"], "SWGFileSinkActions", "SWGFileSinkActions");
    
    ::SWGSDRangel::setValue(&file_source_actions, pJson["FileSourceActions"], "SWGFileSourceActions", "SWGFileSourceActions");
    
    ::SWGSDRangel::setValue(&freq_scanner_actions, pJson["FreqScannerActions"], "SWGFreqScannerActions", "SWGFreqScannerActions");
    
    ::SWGSDRangel::setValue(&ieee_802_15_4_mod_actions, pJson["IEEE_802_15_4_ModActions"], "SWGIEEE_802_15_4_ModActions", "SWGIEEE_802_15_4_ModActions");
    
    ::SWGSDRangel::setValue(&packet_mod_actions, pJson["PacketModActions"], "SWGPacketModActions", "SWGPacketModActions");
    
    ::SWGSDRangel::setValue(&psk31_mod_actions, pJson["PSK31ModActions"], "SWGPSK31ModActions", "SWGPSK31ModActions");
    
    ::SWGSDRangel::setValue(&radio_astronomy_actions, pJson["RadioAstronomyActions"], "SWGRadioAstronomyActions", "SWGRadioAstronomyActions");
    
    ::SWGSDRangel::setValue(&rtty_mod_actions, pJson["RTTYModActions"], "SWGRTTYModActions", "SWGRTTYModActions");
    
    ::SWGSDRangel::setValue(&sig_mf_file_sink_actions, pJson["SigMFFileSinkActions"], "SWGSigMFFileSinkActions", "SWGSigMFFileSinkActions");
    
}

QString
SWGChannelActions::asJson ()
{
    QJsonObject* obj = this->asJsonObject();

    QJsonDocument doc(*obj);
    QByteArray bytes = doc.toJson();
    delete obj;
    return QString(bytes);
}

QJsonObject*
SWGChannelActions::asJsonObject() {
    QJsonObject* obj = new QJsonObject();
    if(channel_type != nullptr && *channel_type != QString("")){
        toJsonValue(QString("channelType"), channel_type, obj, QString("QString"));
    }
    if(m_direction_isSet){
        obj->insert("direction", QJsonValue(direction));
    }
    if(m_originator_device_set_index_isSet){
        obj->insert("originatorDeviceSetIndex", QJsonValue(originator_device_set_index));
    }
    if(m_originator_channel_index_isSet){
        obj->insert("originatorChannelIndex", QJsonValue(originator_channel_index));
    }
    if((ais_mod_actions != nullptr) && (ais_mod_actions->isSet())){
        toJsonValue(QString("AISModActions"), ais_mod_actions, obj, QString("SWGAISModActions"));
    }
    if((apt_demod_actions != nullptr) && (apt_demod_actions->isSet())){
        toJsonValue(QString("APTDemodActions"), apt_demod_actions, obj, QString("SWGAPTDemodActions"));
    }
    if((file_sink_actions != nullptr) && (file_sink_actions->isSet())){
        toJsonValue(QString("FileSinkActions"), file_sink_actions, obj, QString("SWGFileSinkActions"));
    }
    if((file_source_actions != nullptr) && (file_source_actions->isSet())){
        toJsonValue(QString("FileSourceActions"), file_source_actions, obj, QString("SWGFileSourceActions"));
    }
    if((freq_scanner_actions != nullptr) && (freq_scanner_actions->isSet())){
        toJsonValue(QString("FreqScannerActions"), freq_scanner_actions, obj, QString("SWGFreqScannerActions"));
    }
    if((ieee_802_15_4_mod_actions != nullptr) && (ieee_802_15_4_mod_actions->isSet())){
        toJsonValue(QString("IEEE_802_15_4_ModActions"), ieee_802_15_4_mod_actions, obj, QString("SWGIEEE_802_15_4_ModActions"));
    }
    if((packet_mod_actions != nullptr) && (packet_mod_actions->isSet())){
        toJsonValue(QString("PacketModActions"), packet_mod_actions, obj, QString("SWGPacketModActions"));
    }
    if((psk31_mod_actions != nullptr) && (psk31_mod_actions->isSet())){
        toJsonValue(QString("PSK31ModActions"), psk31_mod_actions, obj, QString("SWGPSK31ModActions"));
    }
    if((radio_astronomy_actions != nullptr) && (radio_astronomy_actions->isSet())){
        toJsonValue(QString("RadioAstronomyActions"), radio_astronomy_actions, obj, QString("SWGRadioAstronomyActions"));
    }
    if((rtty_mod_actions != nullptr) && (rtty_mod_actions->isSet())){
        toJsonValue(QString("RTTYModActions"), rtty_mod_actions, obj, QString("SWGRTTYModActions"));
    }
    if((sig_mf_file_sink_actions != nullptr) && (sig_mf_file_sink_actions->isSet())){
        toJsonValue(QString("SigMFFileSinkActions"), sig_mf_file_sink_actions, obj, QString("SWGSigMFFileSinkActions"));
    }

    return obj;
}

QString*
SWGChannelActions::getChannelType() {
    return channel_type;
}
void
SWGChannelActions::setChannelType(QString* channel_type) {
    this->channel_type = channel_type;
    this->m_channel_type_isSet = true;
}

qint32
SWGChannelActions::getDirection() {
    return direction;
}
void
SWGChannelActions::setDirection(qint32 direction) {
    this->direction = direction;
    this->m_direction_isSet = true;
}

qint32
SWGChannelActions::getOriginatorDeviceSetIndex() {
    return originator_device_set_index;
}
void
SWGChannelActions::setOriginatorDeviceSetIndex(qint32 originator_device_set_index) {
    this->originator_device_set_index = originator_device_set_index;
    this->m_originator_device_set_index_isSet = true;
}

qint32
SWGChannelActions::getOriginatorChannelIndex() {
    return originator_channel_index;
}
void
SWGChannelActions::setOriginatorChannelIndex(qint32 originator_channel_index) {
    this->originator_channel_index = originator_channel_index;
    this->m_originator_channel_index_isSet = true;
}

SWGAISModActions*
SWGChannelActions::getAisModActions() {
    return ais_mod_actions;
}
void
SWGChannelActions::setAisModActions(SWGAISModActions* ais_mod_actions) {
    this->ais_mod_actions = ais_mod_actions;
    this->m_ais_mod_actions_isSet = true;
}

SWGAPTDemodActions*
SWGChannelActions::getAptDemodActions() {
    return apt_demod_actions;
}
void
SWGChannelActions::setAptDemodActions(SWGAPTDemodActions* apt_demod_actions) {
    this->apt_demod_actions = apt_demod_actions;
    this->m_apt_demod_actions_isSet = true;
}

SWGFileSinkActions*
SWGChannelActions::getFileSinkActions() {
    return file_sink_actions;
}
void
SWGChannelActions::setFileSinkActions(SWGFileSinkActions* file_sink_actions) {
    this->file_sink_actions = file_sink_actions;
    this->m_file_sink_actions_isSet = true;
}

SWGFileSourceActions*
SWGChannelActions::getFileSourceActions() {
    return file_source_actions;
}
void
SWGChannelActions::setFileSourceActions(SWGFileSourceActions* file_source_actions) {
    this->file_source_actions = file_source_actions;
    this->m_file_source_actions_isSet = true;
}

SWGFreqScannerActions*
SWGChannelActions::getFreqScannerActions() {
    return freq_scanner_actions;
}
void
SWGChannelActions::setFreqScannerActions(SWGFreqScannerActions* freq_scanner_actions) {
    this->freq_scanner_actions = freq_scanner_actions;
    this->m_freq_scanner_actions_isSet = true;
}

SWGIEEE_802_15_4_ModActions*
SWGChannelActions::getIeee802154ModActions() {
    return ieee_802_15_4_mod_actions;
}
void
SWGChannelActions::setIeee802154ModActions(SWGIEEE_802_15_4_ModActions* ieee_802_15_4_mod_actions) {
    this->ieee_802_15_4_mod_actions = ieee_802_15_4_mod_actions;
    this->m_ieee_802_15_4_mod_actions_isSet = true;
}

SWGPacketModActions*
SWGChannelActions::getPacketModActions() {
    return packet_mod_actions;
}
void
SWGChannelActions::setPacketModActions(SWGPacketModActions* packet_mod_actions) {
    this->packet_mod_actions = packet_mod_actions;
    this->m_packet_mod_actions_isSet = true;
}

SWGPSK31ModActions*
SWGChannelActions::getPsk31ModActions() {
    return psk31_mod_actions;
}
void
SWGChannelActions::setPsk31ModActions(SWGPSK31ModActions* psk31_mod_actions) {
    this->psk31_mod_actions = psk31_mod_actions;
    this->m_psk31_mod_actions_isSet = true;
}

SWGRadioAstronomyActions*
SWGChannelActions::getRadioAstronomyActions() {
    return radio_astronomy_actions;
}
void
SWGChannelActions::setRadioAstronomyActions(SWGRadioAstronomyActions* radio_astronomy_actions) {
    this->radio_astronomy_actions = radio_astronomy_actions;
    this->m_radio_astronomy_actions_isSet = true;
}

SWGRTTYModActions*
SWGChannelActions::getRttyModActions() {
    return rtty_mod_actions;
}
void
SWGChannelActions::setRttyModActions(SWGRTTYModActions* rtty_mod_actions) {
    this->rtty_mod_actions = rtty_mod_actions;
    this->m_rtty_mod_actions_isSet = true;
}

SWGSigMFFileSinkActions*
SWGChannelActions::getSigMfFileSinkActions() {
    return sig_mf_file_sink_actions;
}
void
SWGChannelActions::setSigMfFileSinkActions(SWGSigMFFileSinkActions* sig_mf_file_sink_actions) {
    this->sig_mf_file_sink_actions = sig_mf_file_sink_actions;
    this->m_sig_mf_file_sink_actions_isSet = true;
}


bool
SWGChannelActions::isSet(){
    bool isObjectUpdated = false;
    do{
        if(channel_type && *channel_type != QString("")){
            isObjectUpdated = true; break;
        }
        if(m_direction_isSet){
            isObjectUpdated = true; break;
        }
        if(m_originator_device_set_index_isSet){
            isObjectUpdated = true; break;
        }
        if(m_originator_channel_index_isSet){
            isObjectUpdated = true; break;
        }
        if(ais_mod_actions && ais_mod_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(apt_demod_actions && apt_demod_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(file_sink_actions && file_sink_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(file_source_actions && file_source_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(freq_scanner_actions && freq_scanner_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(ieee_802_15_4_mod_actions && ieee_802_15_4_mod_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(packet_mod_actions && packet_mod_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(psk31_mod_actions && psk31_mod_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(radio_astronomy_actions && radio_astronomy_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(rtty_mod_actions && rtty_mod_actions->isSet()){
            isObjectUpdated = true; break;
        }
        if(sig_mf_file_sink_actions && sig_mf_file_sink_actions->isSet()){
            isObjectUpdated = true; break;
        }
    }while(false);
    return isObjectUpdated;
}
}

