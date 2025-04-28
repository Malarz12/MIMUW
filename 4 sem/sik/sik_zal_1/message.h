// message.h
#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <cstdint>
#include <chrono>

#include "list.h"
// The NodeRecord structure represents a single peer's address and port information.
// The peer_address_length field specifies the number of bytes in peer_address.
// The peer_address vector stores the raw address bytes in network byte order.
// The peer_port is stored in host byte order and represents the UDP port.
struct NodeRecord {
    uint8_t               peer_address_length;  // Number of bytes in peer_address
    std::vector<uint8_t>  peer_address;         // Address bytes in network order
    uint16_t              peer_port;            // Port in host byte order
};

// The Message structure encapsulates a full protocol message.
// It contains a message type, a count of NodeRecord entries, the list of nodes,
// a timestamp for synchronization, and a synchronization level.
// The refresh_count() method ensures that the count matches nodes.size().
struct Message {
    uint8_t                 message;       // Message type identifier
    uint16_t                count;         // Number of NodeRecord entries
    std::vector<NodeRecord> nodes;         // Dynamic list of NodeRecord entries
    uint64_t                timestamp;     // Timestamp in host byte order
    uint8_t                 synchronized;  // Synchronization level indicator

    // Updates the count field to match the number of entries in nodes.
    void refresh_count() {
        count = static_cast<uint16_t>(nodes.size());
    }
};

// Serializes a Message into a vector of bytes, using network byte order
// for multi-byte fields. Throws std::runtime_error on any size violation.
std::vector<uint8_t> serialize(const Message& msg);

// Deserializes a byte buffer into a Message object, performing bounds checking
// and converting from network byte order. Throws std::runtime_error on error.
Message deserialize(const uint8_t* data, size_t len);

// Factory functions for creating protocol messages with appropriate initial values.
// Each function returns a Message object with its fields set according to the protocol.
Message make_HELLO();
Message make_HELLO_REPLY(const ListOfSockaddr &contacts);
Message make_CONNECT();
Message make_ACK_CONNECT();
Message make_GET_TIME();
Message make_TIME(int synchronized,
                 std::chrono::steady_clock::time_point start_time,
                 std::chrono::steady_clock::duration offset);
Message make_LEADER(int synchronized);
Message make_SYNC_START(int synchronized,
                       std::chrono::steady_clock::time_point start_time,
                       std::chrono::steady_clock::duration offset);
Message make_DELAY_REQUEST();
Message make_DELAY_RESPONSE(int synchronized,
                            std::chrono::steady_clock::time_point start_time,
                            std::chrono::steady_clock::duration offset);

#endif // MESSAGE_H
