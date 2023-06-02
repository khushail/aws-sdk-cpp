/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <aws/core/utils/memory/stl/AWSMap.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <smithy/Smithy_EXPORTS.h>

namespace smithy {
    namespace components {
        namespace tracing {
            class SMITHY_API GaugeHandle {
            public:
                virtual ~GaugeHandle() = default;

                virtual void Stop() = 0;
            };

            class SMITHY_API AsyncMeasurement {
            public:
                virtual ~AsyncMeasurement() = default;

                virtual void Record(double value, const Aws::Map<Aws::String, Aws::String> &attributes) = 0;
            };
        }
    }
}