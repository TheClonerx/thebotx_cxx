#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

#if __has_include(<version>)
#include <version>
#endif
#if defined(__cpp_lib_format) &&  __cpp_lib_format > 201907L
#include <format>
#define FMT std
#else
#include <fmt/format.h>
#define FMT fmt
#endif

#include <memory>
#include <queue>
#include <span>

namespace ch {
class RoomManager;
} // namespace ch

namespace ch {

class Room {
public:
    Room(RoomManager* mgr, std::string_view name);

    // Connects as anon
    // You can login later
    void connect();

    // Connects as anon with temporal name
    void connect(std::string_view name);

    // Connects as user
    void connect(std::string_view name, std::string_view password);

    void login(std::string_view name);
    void login(std::string_view name, std::string_view password);
    void logout();

    void disconnect();

    std::string_view name() const noexcept
    {
        return m_name;
    }

    std::string_view server() const noexcept
    {
        return m_host;
    }

private:
    template <typename... Args>
    void send_command(std::string_view cmd, Args&&... args)
    {
        std::array<char, (sizeof...(Args) + 1) * 3 + 4> braces {};
        constexpr char const c[4] = "{}:";
        std::size_t i = 0;
        for (; i != braces.size() - 1; ++i) {
            braces[i] = c[i % 3];
        }
        if (m_first_command)
            braces[i++] = '\0';
        else {
            braces[i++] = '\r';
            braces[i++] = '\n';
            braces[i++] = '\0';
        }
        auto command = FMT::format(std::string_view { braces.data(), i }, cmd, std::forward<Args>(args)...);
        m_sending.emplace(std::move(command));
        ensure_writing();
    }

    void cb_on_resolve(std::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
    void cb_on_connect(std::error_code ec, boost::asio::ip::tcp::endpoint);
    void cb_on_tls_handshake(std::error_code ec);
    void cb_on_ws_handshake(std::error_code ec);
    void cb_on_write(std::error_code ec, std::size_t bytes_transferred); // called by ensure_writing
    void cb_on_read(std::error_code ec, std::size_t bytes_read);

    void ensure_writing();

private:
    RoomManager* m_mgr;

    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> m_socket; //{ io_context, ssl_context };
    std::string m_name;
    std::string m_host;
    std::uint64_t m_uid;

    bool m_connecting = false;
    bool m_connected = false;
    bool m_first_command = true;
    bool m_writing = false;

    std::queue<std::string> m_sending;

    std::vector<char> m_receiving;
    boost::asio::dynamic_vector_buffer<char, std::vector<char>::allocator_type> m_receiving_buf;
    // auto m_receiving_buf = boost::asio::dynamic_buffer(m_receiving);
};

} // namespace ch
