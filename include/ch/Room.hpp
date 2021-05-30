#include <ch/HistoryMessage.hpp>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/container/flat_map.hpp>

#if __has_include(<version>)
#include <version>
#endif
#if defined(__cpp_lib_format) && __cpp_lib_format > 201907L
#include <format>
#define FMT std
#else
#include <fmt/format.h>
#define FMT fmt
#endif

#include <queue>

namespace ch {
class RoomManager;
} // namespace ch

namespace ch {

class Room {
public:
    Room(RoomManager* mgr, std::string_view name);

    // Connects as anon
    // You can login later
    boost::asio::awaitable<void> connect();

    // Connects as anon with temporal name
    boost::asio::awaitable<void> connect(std::string_view name);

    // Connects as user
    boost::asio::awaitable<void> connect(std::string_view name, std::string_view password);

    void login(std::string_view name);
    void login(std::string_view name, std::string_view password);
    void logout();

    boost::asio::awaitable<void> disconnect();

    std::string_view name() const noexcept
    {
        return m_name;
    }

    std::string_view server() const noexcept
    {
        return m_host;
    }

    enum class Status {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        DISCONNECTING
    };

private:
    template <typename... Args>
    std::string make_command(std::string_view cmd, Args&&... args)
    {
        std::array<char, (sizeof...(Args) + 1) * 3 + 4> braces {};
        constexpr char const c[4] = "{}:";
        std::size_t i = 0;
        for (; i != braces.size() - 5; ++i) {
            braces[i] = c[i % 3];
        }
        if (m_first_command) {
            braces[i++] = '\0';
            m_first_command = false;
        } else {
            braces[i++] = '\r';
            braces[i++] = '\n';
            braces[i++] = '\0';
        }
        auto command = FMT::format(std::string_view { braces.data(), i }, cmd, std::forward<Args>(args)...);
        return command;
    }

    template <typename... Args>
    boost::asio::awaitable<void> send_command(std::string_view cmd, Args&&... args)
    {
        using boost::asio::use_awaitable;
        auto executor = co_await boost::asio::this_coro::executor;
        auto command = make_command(cmd, std::forward<Args>(args)...);
        co_await boost::asio::post(m_strand, use_awaitable); // start executing in an strand
        co_await m_socket.async_write(boost::asio::buffer(command), use_awaitable);
        co_await boost::asio::post(executor, use_awaitable); // go back to the original executor
    }

private:
    /*
    *   RECEIVED COMMANDS
    */
#define RCMD(cmd) void rcmd_##cmd(std::u8string)
    RCMD(pong);
    RCMD(v);
    RCMD(i);
    RCMD(ok);
#undef RCMD

    /*
    * implementation defined
    */
    boost::asio::awaitable<std::error_code> do_connect();
    boost::asio::awaitable<void> do_read();
    boost::asio::awaitable<void> do_ping();

    void cb_on_read(std::error_code ec, std::size_t bytes_read);

    void reset();

private:
    static boost::container::flat_map<std::u8string_view, decltype(&ch::Room::rcmd_pong)> const cmd_to_func;

    RoomManager* m_mgr;

    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> m_socket; //{ io_context, ssl_context };
    boost::asio::strand<boost::asio::any_io_executor> m_strand;

    std::string m_name;
    std::string m_host;
    std::uint64_t m_uid;

    Status m_status = Status::DISCONNECTED;
    bool m_first_command = true;

    std::vector<HistoryMessage> m_ilogs;
};

} // namespace ch
