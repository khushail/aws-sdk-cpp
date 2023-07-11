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
                    std::function<void()> init,
                    std::function<void()> shutdown) :
                    m_tracerProvider(std::move(tracerProvider)),
                    m_meterProvider(std::move(meterProvider)),
                    m_init(std::move(init)),
                    m_shutdown(std::move(shutdown)) {
                    RunInit();
                }

                virtual ~TelemetryProvider() {
                    RunShutDown();
                }

                std::shared_ptr<Tracer>
                getTracer(Aws::String scope, const Aws::Map<Aws::String, Aws::String> &attributes) {
                    return m_tracerProvider->GetTracer(std::move(scope), attributes);
                }

                std::shared_ptr<Meter>
                getMeter(Aws::String scope, const Aws::Map<Aws::String, Aws::String> &attributes) {
                    return m_meterProvider->GetMeter(std::move(scope), attributes);
                }

                void RunInit() {
                    std::call_once(m_initFlag, m_init);
                }

                void RunShutDown() {
                    std::call_once(m_shutdownFlag, m_shutdown);
                }

            private:
                std::once_flag m_initFlag;
                std::once_flag m_shutdownFlag;
                const Aws::UniquePtr<TracerProvider> m_tracerProvider;
                const Aws::UniquePtr<MeterProvider> m_meterProvider;
                const std::function<void()> m_init;
                const std::function<void()> m_shutdown;
            };
        }
    }
}