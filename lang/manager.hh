/*
 * Copyright (C) 2024-present ScyllaDB
 */

/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#pragma once

#include <seastar/core/sharded.hh>
#include "rust/wasmtime_bindings.hh"
#include "cql3/functions/user_function.hh"

namespace lang {

class manager : public seastar::peering_sharded_service<manager> {
    std::shared_ptr<rust::Box<wasmtime::Engine>> _engine;

public:
    const unsigned lua_max_bytes;
    const unsigned lua_max_contiguous;
    const std::chrono::milliseconds lua_timeout;

public:
    struct lua_config {
        unsigned max_bytes;
        unsigned max_contiguous;
        std::chrono::milliseconds timeout;
    };
    struct config {
        lua_config lua;
    };
    manager(config);
    future<> start();
    future<> stop();
    void remove(const db::functions::function_name& name, const std::vector<data_type>& arg_types) noexcept {
        _instance_cache->remove(name, arg_types);
    }

    using context = std::optional<cql3::functions::user_function::context>;
    future<context> create(sstring language, sstring name, const std::vector<sstring>& arg_names, std::string script);
};

} // lang namespace
