/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <smithy/Smithy_EXPORTS.h>
#include <smithy/tracing/MeterProvider.h>


namespace smithy {
    namespace components {
        namespace tracing {
            class NoopGauageHandle : public GaugeHandle {
            public:
                void Stop() override {}
            };

            class NoopUpDownCounter : public UpDownCounter {
                void add(long value, Aws::Map<Aws::String, Aws::String> attributes) override {
                    AWS_UNREFERENCED_PARAM(value);
                    AWS_UNREFERENCED_PARAM(attributes);
                }
            };

            class NoopMonotonicCounter : public MonotonicCounter {
                void add(long value, Aws::Map<Aws::String, Aws::String> attributes) override {
                    AWS_UNREFERENCED_PARAM(value);
                    AWS_UNREFERENCED_PARAM(attributes);
                }
            };

            class NoopHistogram : public Histogram {
            public:
                void record(double value, Aws::Map<Aws::String, Aws::String> attributes) override {
                    AWS_UNREFERENCED_PARAM(value);
                    AWS_UNREFERENCED_PARAM(attributes);
                }
            };

            class NoopMeter : public Meter {
            public:
                Aws::UniquePtr<GaugeHandle> CreateGauge(Aws::String name,
                    std::function<void(Aws::UniquePtr<AsyncMeasurement>)> callback,
                    Aws::String units,
                    Aws::String description) override {
                    AWS_UNREFERENCED_PARAM(name);
                    AWS_UNREFERENCED_PARAM(callback);
                    AWS_UNREFERENCED_PARAM(units);
                    AWS_UNREFERENCED_PARAM(description);
                    return Aws::MakeUnique<NoopGauageHandle>("NO_OP");
                }

                Aws::UniquePtr<UpDownCounter> CreateUpDownCounter(Aws::String name,
                    Aws::String units,
                    Aws::String description) override {
                    AWS_UNREFERENCED_PARAM(name);
                    AWS_UNREFERENCED_PARAM(units);
                    AWS_UNREFERENCED_PARAM(description);
                    return Aws::MakeUnique<NoopUpDownCounter>("NO_OP");
                }

                Aws::UniquePtr<MonotonicCounter> CreateCounter(Aws::String name,
                    Aws::String units,
                    Aws::String description) override {
                    AWS_UNREFERENCED_PARAM(name);
                    AWS_UNREFERENCED_PARAM(units);
                    AWS_UNREFERENCED_PARAM(description);
                    return Aws::MakeUnique<NoopMonotonicCounter>("NO_OP");
                }

                Aws::UniquePtr<Histogram> CreateHistogram(Aws::String name,
                    Aws::String units,
                    Aws::String description) override {
                    AWS_UNREFERENCED_PARAM(name);
                    AWS_UNREFERENCED_PARAM(units);
                    AWS_UNREFERENCED_PARAM(description);
                    return Aws::MakeUnique<NoopHistogram>("NO_OP");
                }
            };

            class NoopMeterProvider : public MeterProvider {
            public:
                std::shared_ptr<Meter>
                GetMeter(Aws::String scope, Aws::Map<Aws::String, Aws::String> attributes) override {
                    AWS_UNREFERENCED_PARAM(scope);
                    AWS_UNREFERENCED_PARAM(attributes);
                    return Aws::MakeShared<NoopMeter>("NO_OP");
                }
            };
        }
    }
}