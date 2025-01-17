//
//  OctreeServer.h
//  assignment-client/src/octree
//
//  Created by Brad Hefta-Gaub on 8/21/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_OctreeServer_h
#define hifi_OctreeServer_h

#include <QStringList>
#include <QDateTime>
#include <QtCore/QCoreApplication>

#include <HTTPManager.h>

#include <ThreadedAssignment.h>
#include <EnvironmentData.h>

#include "OctreePersistThread.h"
#include "OctreeSendThread.h"
#include "OctreeServerConsts.h"
#include "OctreeInboundPacketProcessor.h"

const int DEFAULT_PACKETS_PER_INTERVAL = 2000; // some 120,000 packets per second total

/// Handles assignments of type OctreeServer - sending octrees to various clients.
class OctreeServer : public ThreadedAssignment, public HTTPRequestHandler {
    Q_OBJECT
public:
    OctreeServer(const QByteArray& packet);
    ~OctreeServer();

    /// allows setting of run arguments
    void setArguments(int argc, char** argv);

    bool wantsDebugSending() const { return _debugSending; }
    bool wantsDebugReceiving() const { return _debugReceiving; }
    bool wantsVerboseDebug() const { return _verboseDebug; }

    Octree* getOctree() { return _tree; }
    JurisdictionMap* getJurisdiction() { return _jurisdiction; }

    int getPacketsPerClientPerInterval() const { return std::min(_packetsPerClientPerInterval, 
                                std::max(1, getPacketsTotalPerInterval() / std::max(1, getCurrentClientCount()))); }

    int getPacketsPerClientPerSecond() const { return getPacketsPerClientPerInterval() * INTERVALS_PER_SECOND; }
    int getPacketsTotalPerInterval() const { return _packetsTotalPerInterval; }
    int getPacketsTotalPerSecond() const { return getPacketsTotalPerInterval() * INTERVALS_PER_SECOND; }
    
    static int getCurrentClientCount() { return _clientCount; }
    static void clientConnected() { _clientCount++; }
    static void clientDisconnected() { _clientCount--; }

    bool isInitialLoadComplete() const { return (_persistThread) ? _persistThread->isInitialLoadComplete() : true; }
    bool isPersistEnabled() const { return (_persistThread) ? true : false; }
    quint64 getLoadElapsedTime() const { return (_persistThread) ? _persistThread->getLoadElapsedTime() : 0; }

    // Subclasses must implement these methods
    virtual OctreeQueryNode* createOctreeQueryNode() = 0;
    virtual Octree* createTree() = 0;
    virtual char getMyNodeType() const = 0;
    virtual PacketType getMyQueryMessageType() const = 0;
    virtual const char* getMyServerName() const = 0;
    virtual const char* getMyLoggingServerTargetName() const = 0;
    virtual const char* getMyDefaultPersistFilename() const = 0;
    virtual PacketType getMyEditNackType() const = 0;

    // subclass may implement these method
    virtual void beforeRun() { }
    virtual bool hasSpecialPacketToSend(const SharedNodePointer& node) { return false; }
    virtual int sendSpecialPacket(const SharedNodePointer& node, OctreeQueryNode* queryNode, int& packetsSent) { return 0; }

    static void attachQueryNodeToNode(Node* newNode);
    
    static float SKIP_TIME; // use this for trackXXXTime() calls for non-times

    static void trackLoopTime(float time) { _averageLoopTime.updateAverage(time); }
    static float getAverageLoopTime() { return _averageLoopTime.getAverage(); }

    static void trackEncodeTime(float time);
    static float getAverageEncodeTime() { return _averageEncodeTime.getAverage(); }

    static void trackInsideTime(float time) { _averageInsideTime.updateAverage(time); }
    static float getAverageInsideTime() { return _averageInsideTime.getAverage(); }

    static void trackTreeWaitTime(float time);
    static float getAverageTreeWaitTime() { return _averageTreeWaitTime.getAverage(); }

    static void trackNodeWaitTime(float time) { _averageNodeWaitTime.updateAverage(time); }
    static float getAverageNodeWaitTime() { return _averageNodeWaitTime.getAverage(); }

    static void trackCompressAndWriteTime(float time);
    static float getAverageCompressAndWriteTime() { return _averageCompressAndWriteTime.getAverage(); }

    static void trackPacketSendingTime(float time);
    static float getAveragePacketSendingTime() { return _averagePacketSendingTime.getAverage(); }

    static void trackProcessWaitTime(float time);
    static float getAverageProcessWaitTime() { return _averageProcessWaitTime.getAverage(); }
    
    // these methods allow us to track which threads got to various states
    static void didProcess(OctreeSendThread* thread);
    static void didPacketDistributor(OctreeSendThread* thread);
    static void didHandlePacketSend(OctreeSendThread* thread);
    static void didCallWriteDatagram(OctreeSendThread* thread);
    static void stopTrackingThread(OctreeSendThread* thread);

    static int howManyThreadsDidProcess(quint64 since = 0);
    static int howManyThreadsDidPacketDistributor(quint64 since = 0);
    static int howManyThreadsDidHandlePacketSend(quint64 since = 0);
    static int howManyThreadsDidCallWriteDatagram(quint64 since = 0);

    bool handleHTTPRequest(HTTPConnection* connection, const QUrl& url, bool skipSubHandler);

    virtual void aboutToFinish();
    void forceNodeShutdown(SharedNodePointer node);
    
public slots:
    /// runs the voxel server assignment
    void run();
    void nodeAdded(SharedNodePointer node);
    void nodeKilled(SharedNodePointer node);
    void sendStatsPacket();
    
    void readPendingDatagrams() { }; // this will not be called since our datagram processing thread will handle
    void readPendingDatagram(const QByteArray& receivedPacket, const HifiSockAddr& senderSockAddr);

protected:
    void parsePayload();
    void initHTTPManager(int port);
    void resetSendingStats();
    QString getUptime();
    QString getFileLoadTime();
    QString getConfiguration();
    QString getStatusLink();

    void setupDatagramProcessingThread();
    
    int _argc;
    const char** _argv;
    char** _parsedArgV;

    HTTPManager* _httpManager;
    int _statusPort;
    QString _statusHost;

    char _persistFilename[MAX_FILENAME_LENGTH];
    int _packetsPerClientPerInterval;
    int _packetsTotalPerInterval;
    Octree* _tree; // this IS a reaveraging tree
    bool _wantPersist;
    bool _debugSending;
    bool _debugReceiving;
    bool _verboseDebug;
    JurisdictionMap* _jurisdiction;
    JurisdictionSender* _jurisdictionSender;
    OctreeInboundPacketProcessor* _octreeInboundPacketProcessor;
    OctreePersistThread* _persistThread;

    static OctreeServer* _instance;

    time_t _started;
    quint64 _startedUSecs;
    QString _safeServerName;
    
    static int _clientCount;
    static SimpleMovingAverage _averageLoopTime;

    static SimpleMovingAverage _averageEncodeTime;
    static SimpleMovingAverage _averageShortEncodeTime;
    static SimpleMovingAverage _averageLongEncodeTime;
    static SimpleMovingAverage _averageExtraLongEncodeTime;
    static int _extraLongEncode;
    static int _longEncode;
    static int _shortEncode;
    static int _noEncode;

    static SimpleMovingAverage _averageInsideTime;

    static SimpleMovingAverage _averageTreeWaitTime;
    static SimpleMovingAverage _averageTreeShortWaitTime;
    static SimpleMovingAverage _averageTreeLongWaitTime;
    static SimpleMovingAverage _averageTreeExtraLongWaitTime;
    static int _extraLongTreeWait;
    static int _longTreeWait;
    static int _shortTreeWait;
    static int _noTreeWait;

    static SimpleMovingAverage _averageNodeWaitTime;

    static SimpleMovingAverage _averageCompressAndWriteTime;
    static SimpleMovingAverage _averageShortCompressTime;
    static SimpleMovingAverage _averageLongCompressTime;
    static SimpleMovingAverage _averageExtraLongCompressTime;
    static int _extraLongCompress;
    static int _longCompress;
    static int _shortCompress;
    static int _noCompress;

    static SimpleMovingAverage _averagePacketSendingTime;
    static int _noSend;

    static SimpleMovingAverage _averageProcessWaitTime;
    static SimpleMovingAverage _averageProcessShortWaitTime;
    static SimpleMovingAverage _averageProcessLongWaitTime;
    static SimpleMovingAverage _averageProcessExtraLongWaitTime;
    static int _extraLongProcessWait;
    static int _longProcessWait;
    static int _shortProcessWait;
    static int _noProcessWait;

    static QMap<OctreeSendThread*, quint64> _threadsDidProcess;
    static QMap<OctreeSendThread*, quint64> _threadsDidPacketDistributor;
    static QMap<OctreeSendThread*, quint64> _threadsDidHandlePacketSend;
    static QMap<OctreeSendThread*, quint64> _threadsDidCallWriteDatagram;

    static QMutex _threadsDidProcessMutex;
    static QMutex _threadsDidPacketDistributorMutex;
    static QMutex _threadsDidHandlePacketSendMutex;
    static QMutex _threadsDidCallWriteDatagramMutex;
};

#endif // hifi_OctreeServer_h
