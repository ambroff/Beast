//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Test that header file is self-contained.
#include <beast/http/span_body.hpp>

#include <beast/http/message.hpp>
#include <beast/unit_test/suite.hpp>

namespace beast {
namespace http {

struct span_body_test
    : public beast::unit_test::suite
{
    void
    testSpanBody()
    {
        {
            using B = span_body<char const>;
            request<B> req;

            BOOST_BEAST_EXPECT(req.body.size() == 0);
            BOOST_BEAST_EXPECT(B::size(req.body) == 0);

            req.body = B::value_type("xyz", 3);
            BOOST_BEAST_EXPECT(req.body.size() == 3);
            BOOST_BEAST_EXPECT(B::size(req.body) == 3);

            B::reader r{req};
            error_code ec;
            r.init(ec);
            BOOST_BEAST_EXPECTS(! ec, ec.message());
            auto const buf = r.get(ec);
            BOOST_BEAST_EXPECTS(! ec, ec.message());
            if(! BOOST_BEAST_EXPECT(buf != boost::none))
                return;
            BOOST_BEAST_EXPECT(boost::asio::buffer_size(buf->first) == 3);
            BOOST_BEAST_EXPECT(! buf->second);
        }
        {
            char buf[5];
            using B = span_body<char>;
            request<B> req;
            req.body = span<char>{buf, sizeof(buf)};
            B::writer w{req};
            error_code ec;
            w.init(boost::none, ec);
            BOOST_BEAST_EXPECTS(! ec, ec.message());
            w.put(boost::asio::const_buffers_1{
                "123", 3}, ec);
            BOOST_BEAST_EXPECTS(! ec, ec.message());
            BOOST_BEAST_EXPECT(buf[0] == '1');
            BOOST_BEAST_EXPECT(buf[1] == '2');
            BOOST_BEAST_EXPECT(buf[2] == '3');
            w.put(boost::asio::const_buffers_1{
                "456", 3}, ec);
            BOOST_BEAST_EXPECTS(ec == error::buffer_overflow, ec.message());
        }
    }

    void
    run() override
    {
        testSpanBody();
    }
};

BOOST_BEAST_DEFINE_TESTSUITE(span_body,http,beast);

} // http
} // beast
