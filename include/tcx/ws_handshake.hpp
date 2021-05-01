#ifndef TCX_WS_HANDSHAKE_HPP
#define TCX_WS_HANDSHAKE_HPP

#include <boost/beast/websocket/stream.hpp>

#include <string_view>

namespace tcx {

template <typename NextLayer>
auto handshake(boost::beast::websocket::stream<NextLayer>& stream, boost::string_view host, boost::string_view target)
{
    struct awaiter {
        boost::beast::websocket::stream<NextLayer>& m_stream;
        boost::string_view m_host;
        boost::string_view m_target;

        std::coroutine_handle<> m_coroutine;

        boost::beast::error_code m_error;

        constexpr bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> coroutine)
        {
            m_coroutine = coroutine;
            m_stream.async_handshake(m_host, m_target, boost::beast::bind_front_handler(&awaiter::on_handshake, this));
        }

        void on_handshake(boost::beast::error_code ec)
        {
            m_error = ec;
            m_coroutine.resume();
        }

        void await_resume()
        {
            if (m_error)
                throw boost::beast::system_error { m_error };
        }
    };
    return awaiter { stream, host, target };
}

} // namespace tcx

#endif
