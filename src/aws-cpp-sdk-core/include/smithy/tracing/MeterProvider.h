/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/utils/memory/stl/AWSMap.h>
#include <smithy/Smithy_EXPORTS.h>
#include <smithy/tracing/Meter.h>

namespace smithy {
    namespace components {
        namespace tracing {
            class SMITHY_API MeterProvider {
            public:
                virtual ~MeterProvider() = default;

                virtual std::shared_ptr<Meter> GetMeter(Aws::String scope,
                    Aws::Map<Aws::String, Aws::String> attributes) = 0;
            };
        }
    }
}