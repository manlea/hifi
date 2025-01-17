//
//  DomainHandler.h
//  libraries/networking/src
//
//  Created by Stephen Birarda on 2/18/2014.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_DomainHandler_h
#define hifi_DomainHandler_h

#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QUuid>
#include <QtCore/QUrl>
#include <QtNetwork/QHostInfo>

#include "HifiSockAddr.h"
#include "NetworkPeer.h"

const QString DEFAULT_DOMAIN_HOSTNAME = "sandbox.highfidelity.io";

const unsigned short DEFAULT_DOMAIN_SERVER_PORT = 40102;
const unsigned short DEFAULT_DOMAIN_SERVER_DTLS_PORT = 40103;
const quint16 DOMAIN_SERVER_HTTP_PORT = 40100;
const quint16 DOMAIN_SERVER_HTTPS_PORT = 40101;

class DomainHandler : public QObject {
    Q_OBJECT
public:
    DomainHandler(QObject* parent = 0);
    
    void clearConnectionInfo();
    void clearSettings();
    
    const QUuid& getUUID() const { return _uuid; }
    void setUUID(const QUuid& uuid);

    static QString hostnameWithoutPort(const QString& hostname);
    bool isCurrentHostname(const QString& hostname);
    const QString& getHostname() const { return _hostname; }
    
    const QHostAddress& getIP() const { return _sockAddr.getAddress(); }
    void setIPToLocalhost() { _sockAddr.setAddress(QHostAddress(QHostAddress::LocalHost)); }
    
    const HifiSockAddr& getSockAddr() { return _sockAddr; }
    void setSockAddr(const HifiSockAddr& sockAddr, const QString& hostname);
    
    unsigned short getPort() const { return _sockAddr.getPort(); }
    
    const QUuid& getAssignmentUUID() const { return _assignmentUUID; }
    void setAssignmentUUID(const QUuid& assignmentUUID) { _assignmentUUID = assignmentUUID; }
    
    const QUuid& getICEDomainID() const { return _iceDomainID; }
    
    const QUuid& getICEClientID() const { return _iceClientID; }
    
    bool requiresICE() const { return !_iceServerSockAddr.isNull(); }
    const HifiSockAddr& getICEServerSockAddr() const { return _iceServerSockAddr; }
    NetworkPeer& getICEPeer() { return _icePeer; }
    void activateICELocalSocket();
    void activateICEPublicSocket();
    
    bool isConnected() const { return _isConnected; }
    void setIsConnected(bool isConnected);
    
    bool hasSettings() const { return !_settingsObject.isEmpty(); }
    void requestDomainSettings();
    const QJsonObject& getSettingsObject() const { return _settingsObject; }
    
    void parseDTLSRequirementPacket(const QByteArray& dtlsRequirementPacket);
    void processICEResponsePacket(const QByteArray& icePacket);
    
    void softReset();
public slots:
    void setHostname(const QString& hostname);
    void setIceServerHostnameAndID(const QString& iceServerHostname, const QUuid& id);
    
private slots:
    void completedHostnameLookup(const QHostInfo& hostInfo);
    void settingsRequestFinished();
signals:
    void hostnameChanged(const QString& hostname);
    void connectedToDomain(const QString& hostname);
    void disconnectedFromDomain();
    void requestICEConnectionAttempt();
    
    void settingsReceived(const QJsonObject& domainSettingsObject);
    void settingsReceiveFail();
    
private:
    void hardReset();
    
    QUuid _uuid;
    QString _hostname;
    HifiSockAddr _sockAddr;
    QUuid _assignmentUUID;
    QUuid _iceDomainID;
    QUuid _iceClientID;
    HifiSockAddr _iceServerSockAddr;
    NetworkPeer _icePeer;
    bool _isConnected;
    QTimer* _handshakeTimer;
    QJsonObject _settingsObject;
    int _failedSettingsRequests;
};

#endif // hifi_DomainHandler_h
