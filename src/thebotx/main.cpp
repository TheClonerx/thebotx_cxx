#include <ch/Room.hpp>
#include <ch/RoomManager.hpp>

#include <spdlog/spdlog.h>

using namespace std::literals;

struct Login {
    int id;
    std::string user_name;
    std::optional<std::string> password;
};

class TheBotx : public ch::RoomManager {
public:
    TheBotx(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_context)
        : ch::RoomManager(io_context, ssl_context)
    {
    }

    ~TheBotx() override;

    boost::asio::awaitable<void> onInit() override
    {
        spdlog::info("Bot Inited"sv);
        co_return;
    }

    boost::asio::awaitable<void> onConnectFail(ch::Room& room, std::error_code ec) override
    {
        spdlog::error("Failed to connect to room {} ({}):\n\t{}: {}"sv, room.name(), room.server(), ec.category().name(), ec.message());
        co_return;
    }

    boost::asio::awaitable<void> onConnect(ch::Room& room) override
    {
        spdlog::info("Connected to room {} ({})"sv, room.name(), room.server());
        co_return;
    }

private:
    std::vector<Login> m_logins;
};

TheBotx::~TheBotx()
{
}

int main()
{
    boost::asio::io_context io_context;
    boost::asio::ssl::context ssl_context { boost::asio::ssl::context::method::sslv23 };
    ssl_context.set_default_verify_paths();
    auto thebotx = TheBotx(io_context, ssl_context);
    thebotx.joinRoom("clonerx");
    thebotx.joinRoom("pythonrpg");
    thebotx.exec();
    io_context.run();
}
