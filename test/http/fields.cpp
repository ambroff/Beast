//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Test that header file is self-contained.
#include <beast/http/fields.hpp>

#include <beast/http/empty_body.hpp>
#include <beast/http/message.hpp>
#include <beast/http/type_traits.hpp>
#include <beast/test/test_allocator.hpp>
#include <beast/unit_test/suite.hpp>
#include <boost/lexical_cast.hpp>

namespace beast {
namespace http {

BOOST_STATIC_ASSERT(is_fields<fields>::value);

class fields_test : public beast::unit_test::suite
{
public:
    template<class Allocator>
    using fa_t = basic_fields<Allocator>;

    using f_t = fa_t<std::allocator<char>>;

    template<class Allocator>
    static
    void
    fill(std::size_t n, basic_fields<Allocator>& f)
    {
        for(std::size_t i = 1; i<= n; ++i)
            f.insert(boost::lexical_cast<std::string>(i), i);
    }

    template<class U, class V>
    static
    void
    self_assign(U& u, V&& v)
    {
        u = std::forward<V>(v);
    }

    template<class Alloc>
    static
    bool
    empty(basic_fields<Alloc> const& f)
    {
        return f.begin() == f.end();
    }

    template<class Alloc>
    static
    std::size_t
    size(basic_fields<Alloc> const& f)
    {
        return std::distance(f.begin(), f.end());
    }

    void
    testMembers()
    {
        using namespace test;

        // compare equal
        using equal_t = test::test_allocator<char,
            true, true, true, true, true>;

        // compare not equal
        using unequal_t = test::test_allocator<char,
            false, true, true, true, true>;

        // construction
        {
            {
                fields f;
                BOOST_BEAST_EXPECT(f.begin() == f.end());
            }
            {
                unequal_t a1;
                basic_fields<unequal_t> f{a1};
                BOOST_BEAST_EXPECT(f.get_allocator() == a1);
                BOOST_BEAST_EXPECT(f.get_allocator() != unequal_t{});
            }
        }

        // move construction
        {
            {
                basic_fields<equal_t> f1;
                BOOST_BEAST_EXPECT(f1.get_allocator()->nmove == 0);
                f1.insert("1", "1");
                BOOST_BEAST_EXPECT(f1["1"] == "1");
                basic_fields<equal_t> f2{std::move(f1)};
                BOOST_BEAST_EXPECT(f2.get_allocator()->nmove == 1);
                BOOST_BEAST_EXPECT(f2["1"] == "1");
                BOOST_BEAST_EXPECT(f1["1"] == "");
            }
            // allocators equal
            {
                basic_fields<equal_t> f1;
                f1.insert("1", "1");
                equal_t a;
                basic_fields<equal_t> f2{std::move(f1), a};
                BOOST_BEAST_EXPECT(f2["1"] == "1");
                BOOST_BEAST_EXPECT(f1["1"] == "");
            }
            {
                // allocators unequal
                basic_fields<unequal_t> f1;
                f1.insert("1", "1");
                unequal_t a;
                basic_fields<unequal_t> f2{std::move(f1), a};
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
        }

        // copy construction
        {
            {
                basic_fields<equal_t> f1;
                f1.insert("1", "1");
                basic_fields<equal_t> f2{f1};
                BOOST_BEAST_EXPECT(f1.get_allocator() == f2.get_allocator());
                BOOST_BEAST_EXPECT(f1["1"] == "1");
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
            {
                basic_fields<unequal_t> f1;
                f1.insert("1", "1");
                unequal_t a;
                basic_fields<unequal_t> f2(f1, a);
                BOOST_BEAST_EXPECT(f1.get_allocator() != f2.get_allocator());
                BOOST_BEAST_EXPECT(f1["1"] == "1");
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
            {
                basic_fields<equal_t> f1;
                f1.insert("1", "1");
                basic_fields<unequal_t> f2(f1);
                BOOST_BEAST_EXPECT(f1["1"] == "1");
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
            {
                basic_fields<unequal_t> f1;
                f1.insert("1", "1");
                equal_t a;
                basic_fields<equal_t> f2(f1, a);
                BOOST_BEAST_EXPECT(f2.get_allocator() == a);
                BOOST_BEAST_EXPECT(f1["1"] == "1");
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
        }

        // move assignment
        {
            {
                fields f1;
                f1.insert("1", "1");
                fields f2;
                f2 = std::move(f1);
                BOOST_BEAST_EXPECT(f1.begin() == f1.end());
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
            {
                // propagate_on_container_move_assignment : true
                using pocma_t = test::test_allocator<char,
                    true, true, true, true, true>;
                basic_fields<pocma_t> f1;
                f1.insert("1", "1");
                basic_fields<pocma_t> f2;
                f2 = std::move(f1);
                BOOST_BEAST_EXPECT(f1.begin() == f1.end());
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
            {
                // propagate_on_container_move_assignment : false
                using pocma_t = test::test_allocator<char,
                    true, true, false, true, true>;
                basic_fields<pocma_t> f1;
                f1.insert("1", "1");
                basic_fields<pocma_t> f2;
                f2 = std::move(f1);
                BOOST_BEAST_EXPECT(f1.begin() == f1.end());
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
        }

        // copy assignment
        {
            {
                fields f1;
                f1.insert("1", "1");
                fields f2;
                f2 = f1;
                BOOST_BEAST_EXPECT(f1["1"] == "1");
                BOOST_BEAST_EXPECT(f2["1"] == "1");
                basic_fields<equal_t> f3;
                f3 = f2;
                BOOST_BEAST_EXPECT(f3["1"] == "1");
            }
            {
                // propagate_on_container_copy_assignment : true
                using pocca_t = test::test_allocator<char,
                    true, true, true, true, true>;
                basic_fields<pocca_t> f1;
                f1.insert("1", "1");
                basic_fields<pocca_t> f2;
                f2 = f1;
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
            {
                // propagate_on_container_copy_assignment : false
                using pocca_t = test::test_allocator<char,
                    true, false, true, true, true>;
                basic_fields<pocca_t> f1;
                f1.insert("1", "1");
                basic_fields<pocca_t> f2;
                f2 = f1;
                BOOST_BEAST_EXPECT(f2["1"] == "1");
            }
        }

        // swap
        {
            {
                // propagate_on_container_swap : true
                using pocs_t = test::test_allocator<char,
                    false, true, true, true, true>;
                pocs_t a1, a2;
                BOOST_BEAST_EXPECT(a1 != a2);
                basic_fields<pocs_t> f1{a1};
                f1.insert("1", "1");
                basic_fields<pocs_t> f2{a2};
                BOOST_BEAST_EXPECT(f1.get_allocator() == a1);
                BOOST_BEAST_EXPECT(f2.get_allocator() == a2);
                swap(f1, f2);
                BOOST_BEAST_EXPECT(f1.get_allocator() == a2);
                BOOST_BEAST_EXPECT(f2.get_allocator() == a1);
                BOOST_BEAST_EXPECT(f1.begin() == f1.end());
                BOOST_BEAST_EXPECT(f2["1"] == "1");
                swap(f1, f2);
                BOOST_BEAST_EXPECT(f1.get_allocator() == a1);
                BOOST_BEAST_EXPECT(f2.get_allocator() == a2);
                BOOST_BEAST_EXPECT(f1["1"] == "1");
                BOOST_BEAST_EXPECT(f2.begin() == f2.end());
            }
            {
                // propagate_on_container_swap : false
                using pocs_t = test::test_allocator<char,
                    true, true, true, false, true>;
                pocs_t a1, a2;
                BOOST_BEAST_EXPECT(a1 == a2);
                BOOST_BEAST_EXPECT(a1.id() != a2.id());
                basic_fields<pocs_t> f1{a1};
                f1.insert("1", "1");
                basic_fields<pocs_t> f2{a2};
                BOOST_BEAST_EXPECT(f1.get_allocator() == a1);
                BOOST_BEAST_EXPECT(f2.get_allocator() == a2);
                swap(f1, f2);
                BOOST_BEAST_EXPECT(f1.get_allocator().id() == a1.id());
                BOOST_BEAST_EXPECT(f2.get_allocator().id() == a2.id());
                BOOST_BEAST_EXPECT(f1.begin() == f1.end());
                BOOST_BEAST_EXPECT(f2["1"] == "1");
                swap(f1, f2);
                BOOST_BEAST_EXPECT(f1.get_allocator().id() == a1.id());
                BOOST_BEAST_EXPECT(f2.get_allocator().id() == a2.id());
                BOOST_BEAST_EXPECT(f1["1"] == "1");
                BOOST_BEAST_EXPECT(f2.begin() == f2.end());
            }
        }

        // operations
        {
            fields f;
            f.insert(field::user_agent, "x");
            BOOST_BEAST_EXPECT(f.count(field::user_agent));
            BOOST_BEAST_EXPECT(f.count(to_string(field::user_agent)));
            BOOST_BEAST_EXPECT(f.count(field::user_agent) == 1);
            BOOST_BEAST_EXPECT(f.count(to_string(field::user_agent)) == 1);
            f.insert(field::user_agent, "y");
            BOOST_BEAST_EXPECT(f.count(field::user_agent) == 2);
        }
    }

    void testHeaders()
    {
        f_t f1;
        BOOST_BEAST_EXPECT(empty(f1));
        fill(1, f1);
        BOOST_BEAST_EXPECT(size(f1) == 1);
        f_t f2;
        f2 = f1;
        BOOST_BEAST_EXPECT(size(f2) == 1);
        f2.insert("2", "2");
        BOOST_BEAST_EXPECT(std::distance(f2.begin(), f2.end()) == 2);
        f1 = std::move(f2);
        BOOST_BEAST_EXPECT(size(f1) == 2);
        BOOST_BEAST_EXPECT(size(f2) == 0);
        f_t f3(std::move(f1));
        BOOST_BEAST_EXPECT(size(f3) == 2);
        BOOST_BEAST_EXPECT(size(f1) == 0);
        self_assign(f3, std::move(f3));
        BOOST_BEAST_EXPECT(size(f3) == 2);
        BOOST_BEAST_EXPECT(f2.erase("Not-Present") == 0);
    }

    void testRFC2616()
    {
        f_t f;
        f.insert("a", "w");
        f.insert("a", "x");
        f.insert("aa", "y");
        f.insert("f", "z");
        BOOST_BEAST_EXPECT(f.count("a") == 2);
    }

    void testErase()
    {
        f_t f;
        f.insert("a", "w");
        f.insert("a", "x");
        f.insert("aa", "y");
        f.insert("f", "z");
        BOOST_BEAST_EXPECT(size(f) == 4);
        f.erase("a");
        BOOST_BEAST_EXPECT(size(f) == 2);
    }

    void
    testContainer()
    {
        {
            // group fields
            fields f;
            f.insert(field::age,   1);
            f.insert(field::body,  2);
            f.insert(field::close, 3);
            f.insert(field::body,  4);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 0)->name() == field::age);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 1)->name() == field::body);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 2)->name() == field::body);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 3)->name() == field::close);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 0)->name_string() == "Age");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 1)->name_string() == "Body");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 2)->name_string() == "Body");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 3)->name_string() == "Close");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 0)->value() == "1");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 1)->value() == "2");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 2)->value() == "4");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 3)->value() == "3");
            BOOST_BEAST_EXPECT(f.erase(field::body) == 2);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 0)->name_string() == "Age");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 1)->name_string() == "Close");
        }
        {
            // group fields, case insensitive
            fields f;
            f.insert("a",  1);
            f.insert("ab", 2);
            f.insert("b",  3);
            f.insert("AB", 4);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 0)->name() == field::unknown);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 1)->name() == field::unknown);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 2)->name() == field::unknown);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 3)->name() == field::unknown);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 0)->name_string() == "a");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 1)->name_string() == "ab");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 2)->name_string() == "AB");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 3)->name_string() == "b");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 0)->value() == "1");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 1)->value() == "2");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 2)->value() == "4");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 3)->value() == "3");
            BOOST_BEAST_EXPECT(f.erase("Ab") == 2);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 0)->name_string() == "a");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 1)->name_string() == "b");
        }
        {
            // verify insertion orde
            fields f;
            f.insert( "a", 1);
            f.insert("dd", 2);
            f.insert("b",  3);
            f.insert("dD", 4);
            f.insert("c",  5);
            f.insert("Dd", 6);
            f.insert("DD", 7);
            f.insert( "e", 8);
            BOOST_BEAST_EXPECT(f.count("dd") == 4);
            BOOST_BEAST_EXPECT(std::next(f.begin(), 1)->name_string() == "dd");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 2)->name_string() == "dD");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 3)->name_string() == "Dd");
            BOOST_BEAST_EXPECT(std::next(f.begin(), 4)->name_string() == "DD");
            f.set("dd", "-");
            BOOST_BEAST_EXPECT(f.count("dd") == 1);
            BOOST_BEAST_EXPECT(f["dd"] == "-");
        }
    }

    struct sized_body
    {
        using value_type = std::uint64_t;

        static
        std::uint64_t
        size(value_type const& v)
        {
            return v;
        }
    };

    struct unsized_body
    {
        struct value_type {};
    };

    void
    testPreparePayload()
    {
        // GET, empty
        {
            request<empty_body> req;
            req.version = 11;
            req.method(verb::get);

            req.prepare_payload();
            BOOST_BEAST_EXPECT(req.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(req.count(field::transfer_encoding) == 0);

            req.set(field::content_length, "0");
            req.set(field::transfer_encoding, "chunked");
            req.prepare_payload();

            BOOST_BEAST_EXPECT(req.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(req.count(field::transfer_encoding) == 0);

            req.set(field::transfer_encoding, "deflate");
            req.prepare_payload();
            BOOST_BEAST_EXPECT(req.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(req[field::transfer_encoding] == "deflate");

            req.set(field::transfer_encoding, "deflate, chunked");
            req.prepare_payload();
            BOOST_BEAST_EXPECT(req.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(req[field::transfer_encoding] == "deflate");
        }

        // GET, sized
        {
            request<sized_body> req;
            req.version = 11;
            req.method(verb::get);
            req.body = 50;

            req.prepare_payload();
            BOOST_BEAST_EXPECT(req[field::content_length] == "50");
            BOOST_BEAST_EXPECT(req[field::transfer_encoding] == "");

            req.set(field::content_length, "0");
            req.set(field::transfer_encoding, "chunked");
            req.prepare_payload();
            BOOST_BEAST_EXPECT(req[field::content_length] == "50");
            BOOST_BEAST_EXPECT(req.count(field::transfer_encoding) == 0);

            req.set(field::transfer_encoding, "deflate, chunked");
            req.prepare_payload();
            BOOST_BEAST_EXPECT(req[field::content_length] == "50");
            BOOST_BEAST_EXPECT(req[field::transfer_encoding] == "deflate");
        }

        // PUT, empty
        {
            request<empty_body> req;
            req.version = 11;
            req.method(verb::put);

            req.prepare_payload();
            BOOST_BEAST_EXPECT(req[field::content_length] == "0");
            BOOST_BEAST_EXPECT(req.count(field::transfer_encoding) == 0);

            req.set(field::content_length, "50");
            req.set(field::transfer_encoding, "deflate, chunked");
            req.prepare_payload();
            BOOST_BEAST_EXPECT(req[field::content_length] == "0");
            BOOST_BEAST_EXPECT(req[field::transfer_encoding] == "deflate");
        }

        // PUT, sized
        {
            request<sized_body> req;
            req.version = 11;
            req.method(verb::put);
            req.body = 50;

            req.prepare_payload();
            BOOST_BEAST_EXPECT(req[field::content_length] == "50");
            BOOST_BEAST_EXPECT(req.count(field::transfer_encoding) == 0);

            req.set(field::content_length, "25");
            req.set(field::transfer_encoding, "deflate, chunked");
            req.prepare_payload();
            BOOST_BEAST_EXPECT(req[field::content_length] == "50");
            BOOST_BEAST_EXPECT(req[field::transfer_encoding] == "deflate");
        }

        // POST, unsized
        {
            request<unsized_body> req;
            req.version = 11;
            req.method(verb::post);

            req.prepare_payload();
            BOOST_BEAST_EXPECT(req.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(req[field::transfer_encoding] == "chunked");

            req.set(field::transfer_encoding, "deflate");
            req.prepare_payload();
            BOOST_BEAST_EXPECT(req.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(req[field::transfer_encoding] == "deflate, chunked");
        }

        // POST, unsized HTTP/1.0
        {
            request<unsized_body> req;
            req.version = 10;
            req.method(verb::post);

            req.prepare_payload();
            BOOST_BEAST_EXPECT(req.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(req.count(field::transfer_encoding) == 0);

            req.set(field::transfer_encoding, "deflate");
            req.prepare_payload();
            BOOST_BEAST_EXPECT(req.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(req[field::transfer_encoding] == "deflate");
        }

        // OK, empty
        {
            response<empty_body> res;
            res.version = 11;

            res.prepare_payload();
            BOOST_BEAST_EXPECT(res[field::content_length] == "0");
            BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);

            res.erase(field::content_length);
            res.set(field::transfer_encoding, "chunked");
            res.prepare_payload();
            BOOST_BEAST_EXPECT(res[field::content_length] == "0");
            BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);
        }

        // OK, sized
        {
            response<sized_body> res;
            res.version = 11;
            res.body = 50;

            res.prepare_payload();
            BOOST_BEAST_EXPECT(res[field::content_length] == "50");
            BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);

            res.erase(field::content_length);
            res.set(field::transfer_encoding, "chunked");
            res.prepare_payload();
            BOOST_BEAST_EXPECT(res[field::content_length] == "50");
            BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);
        }

        // OK, unsized
        {
            response<unsized_body> res;
            res.version = 11;

            res.prepare_payload();
            BOOST_BEAST_EXPECT(res.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked");
        }
    }

    void
    testKeepAlive()
    {
        response<empty_body> res;
        auto const keep_alive =
            [&](bool v)
            {
                res.keep_alive(v);
                BOOST_BEAST_EXPECT(
                    (res.keep_alive() && v) ||
                    (! res.keep_alive() && ! v));
            };

        BOOST_STATIC_ASSERT(fields::max_static_buffer == 4096);
        std::string const big(4096 + 1, 'a');

        // HTTP/1.0
        res.version = 10;
        res.erase(field::connection);

        keep_alive(false);
        BOOST_BEAST_EXPECT(res.count(field::connection) == 0);

        res.set(field::connection, "close");
        keep_alive(false);
        BOOST_BEAST_EXPECT(res.count(field::connection) == 0);

        res.set(field::connection, "keep-alive");
        keep_alive(false);
        BOOST_BEAST_EXPECT(res.count(field::connection) == 0);

        res.set(field::connection, "keep-alive, close");
        keep_alive(false);
        BOOST_BEAST_EXPECT(res.count(field::connection) == 0);

        res.erase(field::connection);
        keep_alive(true);
        BOOST_BEAST_EXPECT(res[field::connection] == "keep-alive");

        res.set(field::connection, "close");
        keep_alive(true);
        BOOST_BEAST_EXPECT(res[field::connection] == "keep-alive");

        res.set(field::connection, "keep-alive");
        keep_alive(true);
        BOOST_BEAST_EXPECT(res[field::connection] == "keep-alive");

        res.set(field::connection, "keep-alive, close");
        keep_alive(true);
        BOOST_BEAST_EXPECT(res[field::connection] == "keep-alive");

        auto const test10 =
            [&](std::string s)
        {
            res.set(field::connection, s);
            keep_alive(false);
            BOOST_BEAST_EXPECT(res[field::connection] == s);

            res.set(field::connection, s + ", close");
            keep_alive(false);
            BOOST_BEAST_EXPECT(res[field::connection] == s);

            res.set(field::connection, "keep-alive, " + s);
            keep_alive(false);
            BOOST_BEAST_EXPECT(res[field::connection] == s);

            res.set(field::connection, "keep-alive, " + s + ", close");
            keep_alive(false);
            BOOST_BEAST_EXPECT(res[field::connection] == s);

            res.set(field::connection, s);
            keep_alive(true);
            BOOST_BEAST_EXPECT(res[field::connection] == s + ", keep-alive");

            res.set(field::connection, s + ", close");
            keep_alive(true);
            BOOST_BEAST_EXPECT(res[field::connection] == s + ", keep-alive");

            res.set(field::connection, "keep-alive, " + s);
            keep_alive(true);
            BOOST_BEAST_EXPECT(res[field::connection] == "keep-alive, " + s);

            res.set(field::connection, "keep-alive, " + s+ ", close");
            keep_alive(true);
            BOOST_BEAST_EXPECT(res[field::connection] == "keep-alive, " + s);
        };

        test10("foo");
        test10(big);

        // HTTP/1.1
        res.version = 11;

        res.erase(field::connection);
        keep_alive(true);
        BOOST_BEAST_EXPECT(res.count(field::connection) == 0);

        res.set(field::connection, "close");
        keep_alive(true);
        BOOST_BEAST_EXPECT(res.count(field::connection) == 0);

        res.set(field::connection, "keep-alive");
        keep_alive(true);
        BOOST_BEAST_EXPECT(res.count(field::connection) == 0);

        res.set(field::connection, "keep-alive, close");
        keep_alive(true);
        BOOST_BEAST_EXPECT(res.count(field::connection) == 0);

        res.erase(field::connection);
        keep_alive(false);
        BOOST_BEAST_EXPECT(res[field::connection] == "close");

        res.set(field::connection, "close");
        keep_alive(false);
        BOOST_BEAST_EXPECT(res[field::connection] == "close");

        res.set(field::connection, "keep-alive");
        keep_alive(false);
        BOOST_BEAST_EXPECT(res[field::connection] == "close");

        res.set(field::connection, "keep-alive, close");
        keep_alive(false);
        BOOST_BEAST_EXPECT(res[field::connection] == "close");

        auto const test11 =
            [&](std::string s)
        {
            res.set(field::connection, s);
            keep_alive(true);
            BOOST_BEAST_EXPECT(res[field::connection] == s);

            res.set(field::connection, s + ", close");
            keep_alive(true);
            BOOST_BEAST_EXPECT(res[field::connection] == s);

            res.set(field::connection, "keep-alive, " + s);
            keep_alive(true);
            BOOST_BEAST_EXPECT(res[field::connection] == s);

            res.set(field::connection, "keep-alive, " + s + ", close");
            keep_alive(true);
            BOOST_BEAST_EXPECT(res[field::connection] == s);

            res.set(field::connection, s);
            keep_alive(false);
            BOOST_BEAST_EXPECT(res[field::connection] == s + ", close");

            res.set(field::connection, "close, " + s);
            keep_alive(false);
            BOOST_BEAST_EXPECT(res[field::connection] == "close, " + s);

            res.set(field::connection, "keep-alive, " + s);
            keep_alive(false);
            BOOST_BEAST_EXPECT(res[field::connection] == s + ", close");

            res.set(field::connection, "close, " + s + ", keep-alive");
            keep_alive(false);
            BOOST_BEAST_EXPECT(res[field::connection] == "close, " + s);
        };

        test11("foo");
        test11(big);
    }

    void
    testContentLength()
    {
        response<empty_body> res{status::ok, 11};
        BOOST_BEAST_EXPECT(res.count(field::content_length) == 0);
        BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);

        res.content_length(0);
        BOOST_BEAST_EXPECT(res[field::content_length] == "0");
        
        res.content_length(100);
        BOOST_BEAST_EXPECT(res[field::content_length] == "100");
        
        res.content_length(boost::none);
        BOOST_BEAST_EXPECT(res.count(field::content_length) == 0);

        res.set(field::transfer_encoding, "chunked");
        res.content_length(0);
        BOOST_BEAST_EXPECT(res[field::content_length] == "0");
        BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);
        
        res.set(field::transfer_encoding, "chunked");
        res.content_length(100);
        BOOST_BEAST_EXPECT(res[field::content_length] == "100");
        BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);
        
        res.set(field::transfer_encoding, "chunked");
        res.content_length(boost::none);
        BOOST_BEAST_EXPECT(res.count(field::content_length) == 0);
        BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);

        auto const check = [&](std::string s)
        {
            res.set(field::transfer_encoding, s);
            res.content_length(0);
            BOOST_BEAST_EXPECT(res[field::content_length] == "0");
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == s);
        
            res.set(field::transfer_encoding, s);
            res.content_length(100);
            BOOST_BEAST_EXPECT(res[field::content_length] == "100");
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == s);
        
            res.set(field::transfer_encoding, s);
            res.content_length(boost::none);
            BOOST_BEAST_EXPECT(res.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == s);

            res.set(field::transfer_encoding, s + ", chunked");
            res.content_length(0);
            BOOST_BEAST_EXPECT(res[field::content_length] == "0");
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == s);
        
            res.set(field::transfer_encoding, s + ", chunked");
            res.content_length(100);
            BOOST_BEAST_EXPECT(res[field::content_length] == "100");
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == s);
        
            res.set(field::transfer_encoding, s + ", chunked");
            res.content_length(boost::none);
            BOOST_BEAST_EXPECT(res.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == s);

            res.set(field::transfer_encoding, "chunked, " + s);
            res.content_length(0);
            BOOST_BEAST_EXPECT(res[field::content_length] == "0");
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked, " + s);
        
            res.set(field::transfer_encoding, "chunked, " + s);
            res.content_length(100);
            BOOST_BEAST_EXPECT(res[field::content_length] == "100");
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked, " + s);
        
            res.set(field::transfer_encoding, "chunked, " + s);
            res.content_length(boost::none);
            BOOST_BEAST_EXPECT(res.count(field::content_length) == 0);
            BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked, " + s);
        };

        check("foo");

        BOOST_STATIC_ASSERT(fields::max_static_buffer == 4096);
        std::string const big(4096 + 1, 'a');

        check(big);
    }

    void
    testChunked()
    {
        response<empty_body> res{status::ok, 11};
        BOOST_BEAST_EXPECT(res.count(field::content_length) == 0);
        BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);

        auto const chunked =
            [&](bool v)
            {
                res.chunked(v);
                BOOST_BEAST_EXPECT(
                    (res.chunked() && v) ||
                    (! res.chunked() && ! v));
                BOOST_BEAST_EXPECT(res.count(
                    field::content_length) == 0);
            };

        res.erase(field::transfer_encoding);
        res.set(field::content_length, 32);
        chunked(true);
        BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked");

        res.set(field::transfer_encoding, "chunked");
        chunked(true);
        BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked");

        res.erase(field::transfer_encoding);
        res.set(field::content_length, 32);
        chunked(false);
        BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);

        res.set(field::transfer_encoding, "chunked");
        chunked(false);
        BOOST_BEAST_EXPECT(res.count(field::transfer_encoding) == 0);



        res.set(field::transfer_encoding, "foo");
        chunked(true);
        BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "foo, chunked");

        res.set(field::transfer_encoding, "chunked, foo");
        chunked(true);
        BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked, foo, chunked");

        res.set(field::transfer_encoding, "chunked, foo, chunked");
        chunked(true);
        BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked, foo, chunked");

        res.set(field::transfer_encoding, "foo, chunked");
        chunked(false);
        BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "foo");

        res.set(field::transfer_encoding, "chunked, foo");
        chunked(false);
        BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked, foo");

        res.set(field::transfer_encoding, "chunked, foo, chunked");
        chunked(false);
        BOOST_BEAST_EXPECT(res[field::transfer_encoding] == "chunked, foo");
    }

    void
    run() override
    {
        testMembers();
        testHeaders();
        testRFC2616();
        testErase();
        testContainer();
        testPreparePayload();

        testKeepAlive();
        testContentLength();
        testChunked();
    }
};

BOOST_BEAST_DEFINE_TESTSUITE(fields,http,beast);

} // http
} // beast
