/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <smithy/tracing/TraceSpan.h>
#include <smithy/Smithy_EXPORTS.h>
#include <memory>

namespace smithy {
    namespace components {
        namespace tracing {
            enum class SMITHY_API SpanKind {
                INTERNAL,
                CLIENT,
                SERVER,
            };

            class SMITHY_API Tracer {
            public:
                virtual ~Tracer() = default;

                virtual std::shared_ptr<TraceSpan> CreateSpan(Aws::String name,
                    const Aws::Map<Aws::String, Aws::String> &attributes,
                    SpanKind spanKind) = 0;
            };
        }
    }
}