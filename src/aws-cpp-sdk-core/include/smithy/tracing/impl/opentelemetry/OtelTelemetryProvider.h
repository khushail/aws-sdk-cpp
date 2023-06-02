/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <smithy/Smithy_EXPORTS.h>

#include <utility>
#include <smithy/tracing/TelemetryProvider.h>
#include <opentelemetry/sdk/trace/exporter.h>

namespace smithy {
    namespace components {
        namespace tracing {
            class SMITHY_API OtelTelemetryProvider {
            public:
                static Aws::UniquePtr<TelemetryProvider> CreateOtelProvider();
            };
        }
    }
}