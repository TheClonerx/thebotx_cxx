#ifndef TCX_SSL_HANDSHAKE_HPP
#define TCX_SSL_HANDSHAKE_HPP

#include <boost/beast/ssl/ssl_stream.hpp>

namespace tcx {

template <typename NextLayer>
auto handshake(boost::beast::ssl_stream<NextLayer>& stream, typename boost::beast::ssl_stream<NextLayer>::handshake_type handshake_type)
{
    struct awaiter {
        boost::beast::ssl_stream<NextLayer>& m_stream;
        typename boost::beast::ssl_stream<NextLayer>::handshake_type m_handshake_type;

        std::coroutine_handle<> m_coroutine;

        boost::beast::error_code m_error;

        constexpr bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> coroutine)
        {
            m_stream.async_handshake(m_handshake_type, boost::beast::bind_front_handler(&awaiter::on_handshake, this));
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
    return awaiter { stream, handshake_type };
}

} // namespace tcx

#endif