//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Test that header file is self-contained.
#include <beast/core/flat_static_buffer.hpp>

#include "buffer_test.hpp"

#include <beast/core/ostream.hpp>
#include <beast/core/string.hpp>
#include <beast/unit_test/suite.hpp>
#include <string>

namespace beast {

static_assert(
    is_dynamic_buffer<flat_static_buffer_base>::value,
    "DynamicBuffer requirements not met");

class flat_static_buffer_test : public beast::unit_test::suite
{
public:
    void
    testStaticBuffer()
    {
        using namespace test;
        using boost::asio::buffer;
        using boost::asio::buffer_cast;
        using boost::asio::buffer_size;
        char buf[12];
        std::string const s = "Hello, world";
        BOOST_BEAST_EXPECT(s.size() == sizeof(buf));
        for(std::size_t i = 1; i < 4; ++i) {
        for(std::size_t j = 1; j < 4; ++j) {
        for(std::size_t x = 1; x < 4; ++x) {
        for(std::size_t y = 1; y < 4; ++y) {
        for(std::size_t t = 1; t < 4; ++ t) {
        for(std::size_t u = 1; u < 4; ++ u) {
        std::size_t z = sizeof(buf) - (x + y);
        std::size_t v = sizeof(buf) - (t + u);
        {
            std::memset(buf, 0, sizeof(buf));
            flat_static_buffer<sizeof(buf)> ba;
            {
                auto d = ba.prepare(z);
                BOOST_BEAST_EXPECT(buffer_size(d) == z);
            }
            {
                auto d = ba.prepare(0);
                BOOST_BEAST_EXPECT(buffer_size(d) == 0);
            }
            {
                auto d = ba.prepare(y);
                BOOST_BEAST_EXPECT(buffer_size(d) == y);
            }
            {
                auto d = ba.prepare(x);
                BOOST_BEAST_EXPECT(buffer_size(d) == x);
                ba.commit(buffer_copy(d, buffer(s.data(), x)));
            }
            BOOST_BEAST_EXPECT(ba.size() == x);
            BOOST_BEAST_EXPECT(buffer_size(ba.data()) == ba.size());
            {
                auto d = ba.prepare(x);
                BOOST_BEAST_EXPECT(buffer_size(d) == x);
            }
            {
                auto d = ba.prepare(0);
                BOOST_BEAST_EXPECT(buffer_size(d) == 0);
            }
            {
                auto d = ba.prepare(z);
                BOOST_BEAST_EXPECT(buffer_size(d) == z);
            }
            {
                auto d = ba.prepare(y);
                BOOST_BEAST_EXPECT(buffer_size(d) == y);
                ba.commit(buffer_copy(d, buffer(s.data()+x, y)));
            }
            ba.commit(1);
            BOOST_BEAST_EXPECT(ba.size() == x + y);
            BOOST_BEAST_EXPECT(buffer_size(ba.data()) == ba.size());
            {
                auto d = ba.prepare(x);
                BOOST_BEAST_EXPECT(buffer_size(d) == x);
            }
            {
                auto d = ba.prepare(y);
                BOOST_BEAST_EXPECT(buffer_size(d) == y);
            }
            {
                auto d = ba.prepare(0);
                BOOST_BEAST_EXPECT(buffer_size(d) == 0);
            }
            {
                auto d = ba.prepare(z);
                BOOST_BEAST_EXPECT(buffer_size(d) == z);
                ba.commit(buffer_copy(d, buffer(s.data()+x+y, z)));
            }
            ba.commit(2);
            BOOST_BEAST_EXPECT(buffer_size(ba.data()) == buffer_size(ba.mutable_data()));
            BOOST_BEAST_EXPECT(ba.size() == x + y + z);
            BOOST_BEAST_EXPECT(buffer_size(ba.data()) == ba.size());
            BOOST_BEAST_EXPECT(to_string(ba.data()) == s);
            ba.consume(t);
            {
                auto d = ba.prepare(0);
                BOOST_BEAST_EXPECT(buffer_size(d) == 0);
            }
            BOOST_BEAST_EXPECT(to_string(ba.data()) == s.substr(t, std::string::npos));
            ba.consume(u);
            BOOST_BEAST_EXPECT(to_string(ba.data()) == s.substr(t + u, std::string::npos));
            ba.consume(v);
            BOOST_BEAST_EXPECT(to_string(ba.data()) == "");
            ba.consume(1);
            {
                auto d = ba.prepare(0);
                BOOST_BEAST_EXPECT(buffer_size(d) == 0);
            }
            try
            {
                ba.prepare(ba.capacity() - ba.size() + 1);
                fail();
            }
            catch(...)
            {
                pass();
            }
        }
        }}}}}}
    }

    void
    testBuffer()
    {
        using namespace test;
        string_view const s = "Hello, world!";
        
        // flat_static_buffer_base
        {
            char buf[64];
            flat_static_buffer_base b{buf, sizeof(buf)};
            ostream(b) << s;
            BOOST_BEAST_EXPECT(to_string(b.data()) == s);
            b.consume(b.size());
            BOOST_BEAST_EXPECT(to_string(b.data()) == "");
        }

        // flat_static_buffer
        {
            flat_static_buffer<64> b1;
            BOOST_BEAST_EXPECT(b1.size() == 0);
            BOOST_BEAST_EXPECT(b1.max_size() == 64);
            BOOST_BEAST_EXPECT(b1.capacity() == 64);
            ostream(b1) << s;
            BOOST_BEAST_EXPECT(to_string(b1.data()) == s);
            {
                flat_static_buffer<64> b2{b1};
                BOOST_BEAST_EXPECT(to_string(b2.data()) == s);
                b2.consume(7);
                BOOST_BEAST_EXPECT(to_string(b2.data()) == s.substr(7));
            }
            {
                flat_static_buffer<64> b2;
                b2 = b1;
                BOOST_BEAST_EXPECT(to_string(b2.data()) == s);
                b2.consume(7);
                BOOST_BEAST_EXPECT(to_string(b2.data()) == s.substr(7));
            }
        }

        // cause memmove
        {
            flat_static_buffer<10> b;
            write_buffer(b, "12345");
            b.consume(3);
            write_buffer(b, "67890123");
            BOOST_BEAST_EXPECT(to_string(b.data()) == "4567890123");
            try
            {
                b.prepare(1);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }

        // read_size
        {
            flat_static_buffer<10> b;
            BOOST_BEAST_EXPECT(read_size(b, 512) == 10);
            b.prepare(4);
            b.commit(4);
            BOOST_BEAST_EXPECT(read_size(b, 512) == 6);
            b.consume(2);
            BOOST_BEAST_EXPECT(read_size(b, 512) == 8);
            b.prepare(8);
            b.commit(8);
            BOOST_BEAST_EXPECT(read_size(b, 512) == 0);
        }

        // base
        {
            flat_static_buffer<10> b;
            [&](flat_static_buffer_base& base)
            {
                BOOST_BEAST_EXPECT(base.max_size() == b.capacity());
            }
            (b.base());

            [&](flat_static_buffer_base const& base)
            {
                BOOST_BEAST_EXPECT(base.max_size() == b.capacity());
            }
            (b.base());
        }
    }

    void run() override
    {
        testBuffer();
        testStaticBuffer();
    }
};

BOOST_BEAST_DEFINE_TESTSUITE(flat_static_buffer,core,beast);

} // beast
