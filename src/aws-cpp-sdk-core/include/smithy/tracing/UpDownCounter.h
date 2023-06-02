/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/utils/memory/stl/AWSMap.h>
#include <smithy/Smithy_EXPORTS.h>

namespace smithy {
    namespace components {
        namespace tracing {
            class SMITHY_API UpDownCounter {
            public:
                virtual ~UpDownCounter() = default;
                virtual void add(long value, Aws::Map<Aws::String, Aws::String> attributes) = 0;
            };
        }
    }
}