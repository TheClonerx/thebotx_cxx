#include <ch/Room.hpp>
#include <ch/RoomManager.hpp>
#include <ch/gen_uid.hpp>
#include <ch/get_server.hpp>

ch::Room::Room(ch::RoomManager* mgr, std::string_view name)
    : m_mgr { mgr }
    , m_socket { m_mgr->io_context(), m_mgr->ssl_context() }
    , m_strand { boost::asio::make_strand(m_socket.get_executor()) }
    , m_name { name }
    , m_host { ch::get_server(m_name) }
    , m_uid { ch::gen_uid() }
{
    m_socket.text(true); // chatango only uses text messages
}

void ch::Room::reset()
{
    m_first_command = true;
    m_ilogs.clear();
}
