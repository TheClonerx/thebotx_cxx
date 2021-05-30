#include <ch/Room.hpp>
#include <ch/RoomManager.hpp>

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include <thread>

#include <tcx/unmangled_name.hpp>

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

    auto work = [&]() {
        spdlog::info("Spinning up thread {}", std::this_thread::get_id());
        for (;;) {
            try {
                io_context.run();
                break;
            } catch (std::exception const& e) {
                spdlog::error("{}: {}"sv, tcx::unmangled_name(typeid(e)), e.what());
            }
        }
    };

    std::vector<std::thread> work_threads;

    for (std::size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
        work_threads.emplace_back() = std::thread(work);
    for (auto& thread : work_threads)
        thread.join();
}
