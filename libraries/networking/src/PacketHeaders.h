//
//  PacketHeaders.h
//  libraries/networking/src
//
//  Created by Stephen Birarda on 4/8/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_PacketHeaders_h
#define hifi_PacketHeaders_h

#include <QtCore/QCryptographicHash>
#include <QtCore/QSet>
#include <QtCore/QUuid>

#include "UUID.h"

// NOTE: if adding a new packet type, you can replace one marked usable or add at the end
// NOTE: if you want the name of the packet type to be available for debugging or logging, update nameForPacketType() as well
enum PacketType {
    PacketTypeUnknown,
    PacketTypeStunResponse,
    PacketTypeDomainList,
    PacketTypePing,
    PacketTypePingReply,
    PacketTypeKillAvatar,
    PacketTypeAvatarData,
    PacketTypeInjectAudio,
    PacketTypeMixedAudio,
    PacketTypeMicrophoneAudioNoEcho,
    PacketTypeMicrophoneAudioWithEcho,
    PacketTypeBulkAvatarData,
    PacketTypeSilentAudioFrame,
    PacketTypeEnvironmentData,
    PacketTypeDomainListRequest,
    PacketTypeRequestAssignment,
    PacketTypeCreateAssignment,
    PacketTypeDomainConnectionDenied,
    PacketTypeMuteEnvironment,
    PacketTypeAudioStreamStats,
    PacketTypeDataServerConfirm,
    PacketTypeVoxelQuery,
    PacketTypeVoxelData,
    PacketTypeVoxelSet,
    PacketTypeVoxelSetDestructive,
    PacketTypeVoxelErase,
    PacketTypeOctreeStats, // 26
    PacketTypeJurisdiction,
    PacketTypeJurisdictionRequest,
    UNUSED_1,
    UNUSED_2,
    UNUSED_3,
    UNUSED_4,
    UNUSED_5,
    PacketTypeMetavoxelData,
    PacketTypeAvatarIdentity,
    PacketTypeAvatarBillboard,
    PacketTypeDomainConnectRequest,
    PacketTypeDomainServerRequireDTLS,
    PacketTypeNodeJsonStats,
    PacketTypeEntityQuery,
    PacketTypeEntityData, // 41
    PacketTypeEntityAddOrEdit,
    PacketTypeEntityErase,
    PacketTypeEntityAddResponse,
    PacketTypeOctreeDataNack, // 45
    PacketTypeVoxelEditNack,
    UNUSED_6,
    PacketTypeEntityEditNack, // 48
    PacketTypeSignedTransactionPayment,
    PacketTypeIceServerHeartbeat,
    PacketTypeIceServerHeartbeatResponse,
    PacketTypeUnverifiedPing,
    PacketTypeUnverifiedPingReply
};

typedef char PacketVersion;

const QSet<PacketType> NON_VERIFIED_PACKETS = QSet<PacketType>()
    << PacketTypeDomainServerRequireDTLS << PacketTypeDomainConnectRequest
    << PacketTypeDomainList << PacketTypeDomainListRequest << PacketTypeDomainConnectionDenied
    << PacketTypeCreateAssignment << PacketTypeRequestAssignment << PacketTypeStunResponse
    << PacketTypeNodeJsonStats << PacketTypeVoxelQuery << PacketTypeEntityQuery
    << PacketTypeOctreeDataNack << PacketTypeVoxelEditNack << PacketTypeEntityEditNack
    << PacketTypeIceServerHeartbeat << PacketTypeIceServerHeartbeatResponse
    << PacketTypeUnverifiedPing << PacketTypeUnverifiedPingReply;

const int NUM_BYTES_MD5_HASH = 16;
const int NUM_STATIC_HEADER_BYTES = sizeof(PacketVersion) + NUM_BYTES_RFC4122_UUID;
const int MAX_PACKET_HEADER_BYTES = sizeof(PacketType) + NUM_BYTES_MD5_HASH + NUM_STATIC_HEADER_BYTES;

PacketVersion versionForPacketType(PacketType type);
QString nameForPacketType(PacketType type);

const QUuid nullUUID = QUuid();

QByteArray byteArrayWithPopulatedHeader(PacketType type, const QUuid& connectionUUID = nullUUID);
int populatePacketHeader(QByteArray& packet, PacketType type, const QUuid& connectionUUID = nullUUID);
int populatePacketHeader(char* packet, PacketType type, const QUuid& connectionUUID = nullUUID);

int numHashBytesInPacketHeaderGivenPacketType(PacketType type);

int numBytesForPacketHeader(const QByteArray& packet);
int numBytesForPacketHeader(const char* packet);
int numBytesForPacketHeaderGivenPacketType(PacketType type);

QUuid uuidFromPacketHeader(const QByteArray& packet);

QByteArray hashFromPacketHeader(const QByteArray& packet);
QByteArray hashForPacketAndConnectionUUID(const QByteArray& packet, const QUuid& connectionUUID);
void replaceHashInPacketGivenConnectionUUID(QByteArray& packet, const QUuid& connectionUUID);

PacketType packetTypeForPacket(const QByteArray& packet);
PacketType packetTypeForPacket(const char* packet);

int arithmeticCodingValueFromBuffer(const char* checkValue);
int numBytesArithmeticCodingFromBuffer(const char* checkValue);

const PacketVersion VERSION_ENTITIES_HAVE_ANIMATION = 1;
const PacketVersion VERSION_ROOT_ELEMENT_HAS_DATA = 2;
const PacketVersion VERSION_ENTITIES_SUPPORT_SPLIT_MTU = 3;
const PacketVersion VERSION_ENTITIES_HAS_FILE_BREAKS = VERSION_ENTITIES_SUPPORT_SPLIT_MTU;
const PacketVersion VERSION_ENTITIES_SUPPORT_DIMENSIONS = 4;
const PacketVersion VERSION_VOXELS_HAS_FILE_BREAKS = 1;

#endif // hifi_PacketHeaders_h
