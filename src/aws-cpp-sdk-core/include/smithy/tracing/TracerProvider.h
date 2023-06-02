/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <smithy/tracing/Tracer.h>
#include <smithy/Smithy_EXPORTS.h>

namespace smithy {
    namespace components {
        namespace tracing {
            class SMITHY_API TracerProvider {
            public:
                virtual ~TracerProvider() = default;

                virtual std::shared_ptr<Tracer> GetTracer(Aws::String scope, const Aws::Map<Aws::String, Aws::String> &attributes) = 0;
            };
        }
    }
}