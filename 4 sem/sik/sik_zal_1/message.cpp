#include "message.h"
#include "list.h"
#include <chrono>
#include <cstring>
#include <arpa/inet.h>  // htons, ntohs
#include <endian.h>     // htobe64, be64toh

using namespace std;
using namespace std::chrono;


// Factory functions to construct protocol messages of various types.
// Each function initializes the fields appropriately and returns a Message.

Message make_HELLO() {
    Message res{};
    res.message = 1;
    res.nodes.clear();
    res.refresh_count();
    res.timestamp = 0;
    res.synchronized = 0;
    return res;
}

Message make_HELLO_REPLY(const ListOfSockaddr &contacts) {
    Message res{};
    res.message = 2;
    res.nodes.clear();

    // Get the list of sockaddr_in entries from our contacts object.
    const auto &addrList = contacts.getList();
    // We cannot encode more than 65535 entries.
    size_t numContacts = std::min(addrList.size(), static_cast<size_t>(UINT16_MAX));

    for (size_t i = 0; i < numContacts; ++i) {
        const auto &sin = addrList[i];
        NodeRecord nr;
        constexpr size_t ADDR_LEN = sizeof(sin.sin_addr.s_addr);
        const uint8_t *rawBytes = reinterpret_cast<const uint8_t *>(&sin.sin_addr.s_addr);
        nr.peer_address.assign(rawBytes, rawBytes + ADDR_LEN);
        nr.peer_address_length = static_cast<uint8_t>(nr.peer_address.size());
        nr.peer_port = sin.sin_port;
        res.nodes.push_back(std::move(nr));
    }
    res.refresh_count();
    res.timestamp    = 0;
    res.synchronized = 0;
    return res;
}


Message make_CONNECT() {
    Message res{};
    res.message = 3;
    res.nodes.clear();
    res.refresh_count();
    res.timestamp = 0;
    res.synchronized = 0;
    return res;
}

Message make_ACK_CONNECT() {
    Message res{};
    res.message = 4;
    res.nodes.clear();
    res.refresh_count();
    res.timestamp = 0;
    res.synchronized = 0;
    return res;
}

Message make_GET_TIME() {
    Message res{};
    res.message = 31;
    res.nodes.clear();
    res.refresh_count();
    res.timestamp = 0;
    res.synchronized = 0;
    return res;
}

Message make_TIME(int synchronized,
                 steady_clock::time_point start_time,
                 steady_clock::duration offset) {
    Message res{};
    res.message = 32;
    res.nodes.clear();
    res.refresh_count();
    res.synchronized = static_cast<uint8_t>(synchronized);
    auto now = steady_clock::now();
    auto base_offset = (synchronized == 0 || synchronized == 255)
                       ? milliseconds(0)
                       : duration_cast<milliseconds>(offset);
    auto elapsed = duration_cast<milliseconds>(now - start_time - base_offset);
    res.timestamp = static_cast<uint64_t>(elapsed.count());
    return res;
}

Message make_LEADER(int synchronized) {
    Message res{};
    res.message = 21;
    res.nodes.clear();
    res.refresh_count();
    res.timestamp = 0;
    res.synchronized = static_cast<uint8_t>(synchronized);
    return res;
}

Message make_SYNC_START(int synchronized,
                       steady_clock::time_point start_time,
                       steady_clock::duration offset) {
    Message res{};
    res.message = 11;
    res.nodes.clear();
    res.refresh_count();
    res.synchronized = static_cast<uint8_t>(synchronized);
    auto now = steady_clock::now();
    auto base_offset = (synchronized == 0 || synchronized == 255)
                       ? milliseconds(0)
                       : duration_cast<milliseconds>(offset);
    auto elapsed = duration_cast<milliseconds>(now - start_time - base_offset);
    res.timestamp = static_cast<uint64_t>(elapsed.count());
    return res;
}

Message make_DELAY_REQUEST() {
    Message res{};
    res.message = 12;
    res.nodes.clear();
    res.refresh_count();
    res.timestamp = 0;
    res.synchronized = 0;
    return res;
}

Message make_DELAY_RESPONSE(int synchronized,
                            steady_clock::time_point start_time,
                            steady_clock::duration offset) {
    Message res{};
    res.message = 13;
    res.nodes.clear();
    res.refresh_count();
    res.synchronized = static_cast<uint8_t>(synchronized);
    auto now = steady_clock::now();
    auto base_offset = (synchronized == 0 || synchronized == 255)
                       ? milliseconds(0)
                       : duration_cast<milliseconds>(offset);
    auto elapsed = duration_cast<milliseconds>(now - start_time - base_offset);
    res.timestamp = static_cast<uint64_t>(elapsed.count());
    return res;
}


// Serializes a Message into a byte vector suitable for UDP transmission.
// It checks for node count overflow and UDP payload size limits.
vector<uint8_t> serialize(const Message& msg_in) {
    // Ensure count fits in 16 bits
    if (msg_in.nodes.size() > numeric_limits<uint16_t>::max()) {
        throw runtime_error("Too many nodes to encode (exceeds 65535)");
    }

    Message msg = msg_in;
    msg.refresh_count();

    // Compute exact total size
    size_t total = 1   // message
                 + 2   // count
                 + 8   // timestamp
                 + 1;  // synchronized

    for (auto const& nr : msg.nodes) {
        total += 1                           // peer_address_length
               + nr.peer_address_length     // address bytes
               + 2;                          // port
    }

    // Verify that the payload will fit within a single UDP datagram.
    constexpr size_t MAX_UDP_PAYLOAD = 65507;
    if (total > MAX_UDP_PAYLOAD) {
        throw runtime_error("Serialized message too large for UDP");
    }

    vector<uint8_t> buf;
    buf.reserve(total);

    // Append the message type byte.
    buf.push_back(msg.message);

    // Append the 2-byte count in network byte order.
    uint16_t cnt_n = htons(msg.count);
    buf.push_back(static_cast<uint8_t>(cnt_n >> 8));
    buf.push_back(static_cast<uint8_t>(cnt_n & 0xFF));

   // Append each NodeRecord sequentially.
    for (auto const& nr : msg.nodes) {
        buf.push_back(nr.peer_address_length);
        buf.insert(buf.end(), nr.peer_address.begin(), nr.peer_address.end());
        uint16_t port_n = htons(nr.peer_port);
        buf.push_back(static_cast<uint8_t>(port_n >> 8));
        buf.push_back(static_cast<uint8_t>(port_n & 0xFF));
    }

    // Append the 8-byte timestamp in big-endian order.
    uint64_t ts_n = htobe64(msg.timestamp);
    for (int i = 7; i >= 0; --i) {
        buf.push_back(static_cast<uint8_t>((ts_n >> (i*8)) & 0xFF));
    }

    // Append the final synchronized byte.
    buf.push_back(msg.synchronized);

    return buf;
}

// Deserializes a byte buffer back into a Message structure.
// It performs bounds checking and throws runtime_error on any inconsistency.
Message deserialize(const uint8_t* data, size_t len) {
    size_t pos = 0;
    // Verify minimal header length: message + count + timestamp + sync.
    if (len < 1 + 2 + 8 + 1) throw runtime_error("Buffer too short");

    Message msg;
    msg.message = data[pos++];

    // Read count and convert from network byte order.
    uint16_t cnt_n = (static_cast<uint16_t>(data[pos]) << 8) | data[pos+1];
    msg.count = ntohs(cnt_n);
    pos += 2;

    // Reserve space for node records and read each one.
    msg.nodes.clear();
    msg.nodes.reserve(msg.count);
    for (uint16_t i = 0; i < msg.count; ++i) {
        if (pos >= len) throw runtime_error("Incomplete node data");
        NodeRecord nr;
        nr.peer_address_length = data[pos++];
        if (pos + nr.peer_address_length + 2 > len) throw runtime_error("Node record too large");
        nr.peer_address.assign(data + pos, data + pos + nr.peer_address_length);
        pos += nr.peer_address_length;
        uint16_t port_n = (static_cast<uint16_t>(data[pos]) << 8) | data[pos+1];
        nr.peer_port = ntohs(port_n);
        pos += 2;
        msg.nodes.push_back(move(nr));
    }

    // Read the 8-byte timestamp.
    if (pos + 8 > len) throw runtime_error("Missing timestamp");
    uint64_t ts = 0;
    for (int i = 0; i < 8; ++i) ts = (ts << 8) | data[pos++];
    msg.timestamp = be64toh(ts);

    // Read the final synchronized byte.
    if (pos >= len) throw runtime_error("Missing synchronized byte");
    msg.synchronized = data[pos++];

    // Final consistency check for node count.
    if (msg.nodes.size() != msg.count) throw runtime_error("Node count mismatch");

    return msg;
}