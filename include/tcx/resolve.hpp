#ifndef TCX_RESOLVE_HPP
#define TCX_RESOLVE_HPP

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/basic_resolver.hpp>

#include <functional>
#include <string_view>

namespace tcx {

template <typename Protocol>
auto resolve(boost::asio::io_context& io_context, std::string_view host, std::string_view port, typename boost::asio::ip::resolver_base::flags resolve_flags = {})
{
    struct awaiter {
        awaiter(boost::asio::io_context& io_context, std::string_view host, std::string_view port, typename boost::asio::ip::resolver_base::flags resolve_flags)
            : m_resolver { io_context }
            , m_host { host }
            , m_port { port }
            , m_resolve_flags { resolve_flags }
        {
        }

        boost::asio::ip::basic_resolver<Protocol> m_resolver;
        std::string_view m_host;
        std::string_view m_port;
        boost::asio::ip::resolver_base::flags m_resolve_flags;

        std::coroutine_handle<> m_coroutine;

        boost::system::error_code m_error;
        boost::asio::ip::basic_resolver<Protocol>::results_type m_result;

        constexpr bool await_ready() const noexcept
        {
            return false;
        }

        void await_suspend(std::coroutine_handle<> coroutine)
        {
            m_coroutine = coroutine;
            m_resolver.async_resolve(m_host, m_port, m_resolve_flags, boost::beast::bind_front_handler(&awaiter::on_resolve, this));
        }

        void on_resolve(boost::system::error_code ec, boost::asio::ip::basic_resolver<Protocol>::results_type result)
        {
            m_error = ec;
            m_result = std::move(result);
            m_coroutine.resume();
        }

        boost::asio::ip::basic_resolver<Protocol>::results_type await_resume()
        {
            if (m_error)
                throw boost::system::system_error { m_error };
            return std::move(m_result);
        }
    };
    return awaiter { io_context, host, port, resolve_flags };
}

#if 0
template <typename Protocol>
auto resolve(boost::asio::io_context& io_context, std::string host, std::uint16_t port)
{
    std::string buf(std::numeric_limits<std::uint16_t>::digits10, '\0');
    auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), port);
    buf.reserve(ptr - buf.data());
    return resolve<Protocol>(io_context, std::move(host), std::move(buf));
}
#endif

} // namespace tcx

#endif