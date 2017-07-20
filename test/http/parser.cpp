//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Test that header file is self-contained.
#include <beast/http/parser.hpp>

#include "test_parser.hpp"

#include <beast/unit_test/suite.hpp>
#include <beast/test/string_istream.hpp>
#include <beast/test/string_ostream.hpp>
#include <beast/test/yield_to.hpp>
#include <beast/core/consuming_buffers.hpp>
#include <beast/core/flat_buffer.hpp>
#include <beast/core/multi_buffer.hpp>
#include <beast/core/ostream.hpp>
#include <beast/http/read.hpp>
#include <beast/http/string_body.hpp>
#include <boost/system/system_error.hpp>

namespace beast {
namespace http {

class parser_test
    : public beast::unit_test::suite
    , public beast::test::enable_yield_to
{
public:
    template<bool isRequest>
    using parser_type =
        parser<isRequest, string_body>;

    static
    boost::asio::const_buffers_1
    buf(string_view s)
    {
        return {s.data(), s.size()};
    }

    template<class ConstBufferSequence,
        bool isRequest, class Derived>
    static
    void
    put(ConstBufferSequence const& buffers,
        basic_parser<isRequest, Derived>& p,
            error_code& ec)
    {
        using boost::asio::buffer_size;
        consuming_buffers<ConstBufferSequence> cb{buffers};
        for(;;)
        {
            auto const used = p.put(cb, ec);
            cb.consume(used);
            if(ec)
                return;
            if(p.need_eof() &&
                buffer_size(cb) == 0)
            {
                p.put_eof(ec);
                if(ec)
                    return;
            }
            if(p.is_done())
                break;
        }
    }

    template<bool isRequest, class F>
    void
    doMatrix(string_view s0, F const& f)
    {
        using boost::asio::buffer;
        // parse a single buffer
        {
            auto s = s0;
            error_code ec;
            parser_type<isRequest> p;
            put(buffer(s.data(), s.size()), p, ec);
            if(! BOOST_BEAST_EXPECTS(! ec, ec.message()))
                return;
            f(p);
        }
        // parse two buffers
        for(auto n = s0.size() - 1; n >= 1; --n)
        {
            auto s = s0;
            error_code ec;
            parser_type<isRequest> p;
            p.eager(true);
            auto used =
                p.put(buffer(s.data(), n), ec);
            s.remove_prefix(used);
            if(ec == error::need_more)
                ec.assign(0, ec.category());
            if(! BOOST_BEAST_EXPECTS(! ec, ec.message()))
                continue;
            BOOST_BEAST_EXPECT(! p.is_done());
            used = p.put(
                buffer(s.data(), s.size()), ec);
            s.remove_prefix(used);
            if(! BOOST_BEAST_EXPECTS(! ec, ec.message()))
                continue;
            BOOST_BEAST_EXPECT(s.empty());
            if(p.need_eof())
            {
                p.put_eof(ec);
                if(! BOOST_BEAST_EXPECTS(! ec, ec.message()))
                    continue;
            }
            if(BOOST_BEAST_EXPECT(p.is_done()))
                f(p);
        }
    }

    void
    testParse()
    {
        doMatrix<false>(
            "HTTP/1.0 200 OK\r\n"
            "Server: test\r\n"
            "\r\n"
            "Hello, world!",
            [&](parser_type<false> const& p)
            {
                auto const& m = p.get();
                BOOST_BEAST_EXPECT(! p.is_chunked());
                BOOST_BEAST_EXPECT(p.need_eof());
                BOOST_BEAST_EXPECT(p.content_length() == boost::none);
                BOOST_BEAST_EXPECT(m.version == 10);
                BOOST_BEAST_EXPECT(m.result() == status::ok);
                BOOST_BEAST_EXPECT(m.reason() == "OK");
                BOOST_BEAST_EXPECT(m["Server"] == "test");
                BOOST_BEAST_EXPECT(m.body == "Hello, world!");
            }
        );
        doMatrix<false>(
            "HTTP/1.1 200 OK\r\n"
            "Server: test\r\n"
            "Expect: Expires, MD5-Fingerprint\r\n"
            "Transfer-Encoding: chunked\r\n"
            "\r\n"
            "5\r\n"
            "*****\r\n"
            "2;a;b=1;c=\"2\"\r\n"
            "--\r\n"
            "0;d;e=3;f=\"4\"\r\n"
            "Expires: never\r\n"
            "MD5-Fingerprint: -\r\n"
            "\r\n",
            [&](parser_type<false> const& p)
            {
                auto const& m = p.get();
                BOOST_BEAST_EXPECT(! p.need_eof());
                BOOST_BEAST_EXPECT(p.is_chunked());
                BOOST_BEAST_EXPECT(p.content_length() == boost::none);
                BOOST_BEAST_EXPECT(m.version == 11);
                BOOST_BEAST_EXPECT(m.result() == status::ok);
                BOOST_BEAST_EXPECT(m.reason() == "OK");
                BOOST_BEAST_EXPECT(m["Server"] == "test");
                BOOST_BEAST_EXPECT(m["Transfer-Encoding"] == "chunked");
                BOOST_BEAST_EXPECT(m["Expires"] == "never");
                BOOST_BEAST_EXPECT(m["MD5-Fingerprint"] == "-");
                BOOST_BEAST_EXPECT(m.body == "*****--");
            }
        );
        doMatrix<false>(
            "HTTP/1.0 200 OK\r\n"
            "Server: test\r\n"
            "Content-Length: 5\r\n"
            "\r\n"
            "*****",
            [&](parser_type<false> const& p)
            {
                auto const& m = p.get();
                BOOST_BEAST_EXPECT(m.body == "*****");
            }
        );
        doMatrix<true>(
            "GET / HTTP/1.1\r\n"
            "User-Agent: test\r\n"
            "\r\n",
            [&](parser_type<true> const& p)
            {
                auto const& m = p.get();
                BOOST_BEAST_EXPECT(m.method() == verb::get);
                BOOST_BEAST_EXPECT(m.target() == "/");
                BOOST_BEAST_EXPECT(m.version == 11);
                BOOST_BEAST_EXPECT(! p.need_eof());
                BOOST_BEAST_EXPECT(! p.is_chunked());
                BOOST_BEAST_EXPECT(p.content_length() == boost::none);
            }
        );
        doMatrix<true>(
            "GET / HTTP/1.1\r\n"
            "User-Agent: test\r\n"
            "X: \t x \t \r\n"
            "\r\n",
            [&](parser_type<true> const& p)
            {
                auto const& m = p.get();
                BOOST_BEAST_EXPECT(m["X"] == "x");
            }
        );

        // test eager(true)
        {
            error_code ec;
            parser_type<true> p;
            p.eager(true);
            p.put(buf(
                "GET / HTTP/1.1\r\n"
                "User-Agent: test\r\n"
                "Content-Length: 1\r\n"
                "\r\n"
                "*")
                , ec);
            auto const& m = p.get();
            BOOST_BEAST_EXPECT(! ec);
            BOOST_BEAST_EXPECT(p.is_done());
            BOOST_BEAST_EXPECT(p.is_header_done());
            BOOST_BEAST_EXPECT(! p.need_eof());
            BOOST_BEAST_EXPECT(m.method() == verb::get);
            BOOST_BEAST_EXPECT(m.target() == "/");
            BOOST_BEAST_EXPECT(m.version == 11);
            BOOST_BEAST_EXPECT(m["User-Agent"] == "test");
            BOOST_BEAST_EXPECT(m.body == "*");
        }
        {
            // test partial parsing of final chunk
            // parse through the chunk body
            error_code ec;
            flat_buffer b;
            parser_type<true> p;
            p.eager(true);
            ostream(b) <<
                "PUT / HTTP/1.1\r\n"
                "Transfer-Encoding: chunked\r\n"
                "\r\n"
                "1\r\n"
                "*";
            auto used = p.put(b.data(), ec);
            b.consume(used);
            BOOST_BEAST_EXPECT(! ec);
            BOOST_BEAST_EXPECT(! p.is_done());
            BOOST_BEAST_EXPECT(p.get().body == "*");
            ostream(b) <<
                "\r\n"
                "0;d;e=3;f=\"4\"\r\n"
                "Expires: never\r\n"
                "MD5-Fingerprint: -\r\n";
            // incomplete parse, missing the final crlf
            used = p.put(b.data(), ec);
            b.consume(used);
            BOOST_BEAST_EXPECT(ec == error::need_more);
            ec.assign(0, ec.category());
            BOOST_BEAST_EXPECT(! p.is_done());
            ostream(b) <<
                "\r\n"; // final crlf to end message
            used = p.put(b.data(), ec);
            b.consume(used);
            BOOST_BEAST_EXPECTS(! ec, ec.message());
            BOOST_BEAST_EXPECT(p.is_done());
        }
        // skip body
        {
            error_code ec;
            response_parser<string_body> p;
            p.skip(true);
            p.put(buf(
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 5\r\n"
                "\r\n"
                "*****")
                , ec);
            BOOST_BEAST_EXPECTS(! ec, ec.message());
            BOOST_BEAST_EXPECT(p.is_done());
            BOOST_BEAST_EXPECT(p.is_header_done());
            BOOST_BEAST_EXPECT(p.content_length() &&
                *p.content_length() == 5);
        }
    }

    //--------------------------------------------------------------------------

    template<class DynamicBuffer>
    void
    testNeedMore()
    {
        error_code ec;
        std::size_t used;
        {
            DynamicBuffer b;
            parser_type<true> p;
            ostream(b) <<
                "GET / HTTP/1.1\r\n";
            used = p.put(b.data(), ec);
            BOOST_BEAST_EXPECTS(ec == error::need_more, ec.message());
            b.consume(used);
            ec.assign(0, ec.category());
            ostream(b) <<
                "User-Agent: test\r\n"
                "\r\n";
            used = p.put(b.data(), ec);
            BOOST_BEAST_EXPECTS(! ec, ec.message());
            b.consume(used);
            BOOST_BEAST_EXPECT(p.is_done());
            BOOST_BEAST_EXPECT(p.is_header_done());
        }
    }

    void
    testGotSome()
    {
        error_code ec;
        parser_type<true> p;
        auto used = p.put(buf(""), ec);
        BOOST_BEAST_EXPECT(ec == error::need_more);
        BOOST_BEAST_EXPECT(! p.got_some());
        BOOST_BEAST_EXPECT(used == 0);
        ec.assign(0, ec.category());
        used = p.put(buf("G"), ec);
        BOOST_BEAST_EXPECT(ec == error::need_more);
        BOOST_BEAST_EXPECT(p.got_some());
        BOOST_BEAST_EXPECT(used == 0);
    }

    void
    run() override
    {
        testParse();
        testNeedMore<flat_buffer>();
        testNeedMore<multi_buffer>();
        testGotSome();
    }
};

BOOST_BEAST_DEFINE_TESTSUITE(parser,http,beast);

} // http
} // beast

