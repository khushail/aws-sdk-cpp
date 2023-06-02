/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <smithy/Smithy_EXPORTS.h>
#include <smithy/tracing/TracerProvider.h>
#include <smithy/tracing/MeterProvider.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <utility>
#include <mutex>

namespace smithy {
    namespace components {
        namespace tracing {
            class SMITHY_API TelemetryProvider {
            public:
                TelemetryProvider(Aws::UniquePtr<TracerProvider> tracerProvider,
                    Aws::UniquePtr<MeterProvider> meterProvider,
                    std::function<void()> init) :
                    tracerProvider(std::move(tracerProvider)),
                    meterProvider(std::move(meterProvider)),
                    init(std::move(init)) {}

                std::shared_ptr<Tracer>
                getTracer(Aws::String scope, const Aws::Map<Aws::String, Aws::String> &attributes) {
                    return tracerProvider->GetTracer(std::move(scope), attributes);
                }

                std::shared_ptr<Meter>
                getMeter(Aws::String scope, const Aws::Map<Aws::String, Aws::String> &attributes) {
                    return meterProvider->GetMeter(std::move(scope), attributes);
                }

                void RunInit() {
                    std::call_once(initialized, init);
                }

            private:
                std::once_flag initialized;
                const Aws::UniquePtr<TracerProvider> tracerProvider;
                const Aws::UniquePtr<MeterProvider> meterProvider;
                const std::function<void()> init;
            };
        }
    }
}