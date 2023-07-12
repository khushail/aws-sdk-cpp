/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/utils/memory/stl/AWSMap.h>
#include <smithy/Smithy_EXPORTS.h>
#include <memory>
#include <utility>

namespace smithy {
    namespace components {
        namespace tracing {
            /**
             * Experimental - This definition is a work in progress API
             * changes are expected.
             *
             * Status of the span.
             */
            enum class SMITHY_API TraceSpanStatus {
                UNSET,
                OK,
                ERROR,
            };

            /**
             * Experimental - This definition is a work in progress API
             * changes are expected.
             *
             * The basic unit of a "Trace". Represents a time period during which events
             * or metrics can take place such as counts, timers, statistics, and messages.
             * Additionally child "Traces" can exist in a parent trace that will have its
             * own unique events. Keeps track of where and when an event happened.
             */
            class SMITHY_API TraceSpan {
            public:
                TraceSpan(Aws::String name) : m_name(std::move(name)) {}

                virtual ~TraceSpan() = default;

                virtual void emitEvent(Aws::String name, const Aws::Map<Aws::String, Aws::String> &attributes) = 0;

                virtual void setAttribute(Aws::String key, Aws::String value) = 0;

                virtual void setStatus(TraceSpanStatus status) = 0;

                virtual void end() = 0;

            private:
                Aws::String m_name;
            };
        }
    }
}