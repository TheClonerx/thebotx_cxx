#ifndef TCX_CONNECT_HPP
#define TCX_CONNECT_HPP

#include <boost/beast/core/basic_stream.hpp>
#include <coroutine>

namespace tcx {
template <typename Protocol, typename Executor, typename RatePolicy, typename Endpoint>
auto connect(boost::beast::basic_stream<Protocol, Executor, RatePolicy>& stream, Endpoint const& endpoint)
{
    struct awaiter {
        boost::beast::basic_stream<Protocol, Executor, RatePolicy>& m_stream;
        Endpoint const& m_endpoint;

        std::coroutine_handle<> m_coroutine;

        boost::beast::error_code m_error;

        constexpr bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> coroutine)
        {
            m_coroutine = coroutine;
            m_stream.async_connect(m_endpoint, boost::beast::bind_front_handler(&awaiter::on_connect, this));
        }

        void on_connect(boost::beast::error_code ec, typename Protocol::endpoint const& endpoint)
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
    return awaiter { stream, endpoint };
}
} // namespace tcx

#endif
