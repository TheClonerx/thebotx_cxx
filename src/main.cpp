#include <ch/Room.hpp>
#include <ch/RoomManager.hpp>

#include <spdlog/spdlog.h>

using namespace std::literals;

class TheBotx : public ch::RoomManager {
public:
    using ch::RoomManager::RoomManager;

    void onInit() override
    {
        spdlog::info("Bot Inited"sv);
    }

    void onConnectFail(ch::Room& room, std::error_code ec) override
    {
        spdlog::error("Failed to connect to room {} ({}):\n\t{}: {}"sv, room.name(), room.server(), ec.category().name(), ec.message());
    }

    void onConnect(ch::Room& room) override
    {
        spdlog::info("Connected to room {} ({})"sv, room.name(), room.server());
    }
};

int main()
{
    boost::asio::io_context io_context;
    boost::asio::ssl::context ssl_context { boost::asio::ssl::context::method::sslv23 };
    ssl_context.set_default_verify_paths();
    auto thebotx = TheBotx(io_context, ssl_context);
    thebotx.joinRoom("clonerx"sv);
    thebotx.joinRoom("pythonrpg"sv);
    thebotx.exec();
    io_context.run();
}