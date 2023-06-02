/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <smithy/Smithy_EXPORTS.h>
#include <smithy/tracing/Meter.h>
#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/metrics/meter.h>
#include <opentelemetry/metrics/observer_result.h>
#include <opentelemetry/metrics/async_instruments.h>
#include <utility>

namespace smithy {
    namespace components {
        namespace tracing {
            class OtelMeterAdapter final : public Meter {
            public:
                explicit OtelMeterAdapter(opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> meter) :
                    otelMeter(std::move(meter)) {}

                Aws::UniquePtr<GaugeHandle> CreateGauge(Aws::String name,
                    std::function<void(Aws::UniquePtr<AsyncMeasurement>)> callback,
                    Aws::String units,
                    Aws::String description) override;

                Aws::UniquePtr<UpDownCounter> CreateUpDownCounter(Aws::String name,
                    Aws::String units,
                    Aws::String description) override;

                Aws::UniquePtr<MonotonicCounter> CreateCounter(Aws::String name,
                    Aws::String units,
                    Aws::String description) override;

                Aws::UniquePtr<Histogram> CreateHistogram(Aws::String name,
                    Aws::String units,
                    Aws::String description) override;

            private:
                opentelemetry::nostd::shared_ptr<opentelemetry::metrics::Meter> otelMeter;
            };

            class OtelCounterAdapter final : public MonotonicCounter {
            public:
                explicit OtelCounterAdapter(
                    opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<uint64_t>> counter);

                void add(long value, Aws::Map<Aws::String, Aws::String> attributes) override;

            private:
                opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Counter<uint64_t>> otelCounter;
            };

            class OtelUpDownCounterAdapter final : public UpDownCounter {
            public:
                explicit OtelUpDownCounterAdapter(
                    opentelemetry::nostd::unique_ptr<opentelemetry::metrics::UpDownCounter<int64_t>> counter);

                void add(long value, Aws::Map<Aws::String, Aws::String> attributes) override;

            private:
                opentelemetry::nostd::unique_ptr<opentelemetry::metrics::UpDownCounter<int64_t>> otelUpDownCounter;
            };

            class OtelHistogramAdapter final : public Histogram {
            public:
                explicit OtelHistogramAdapter(
                    opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Histogram<double>> otelHistogram);

                void record(double value,
                    Aws::Map<Aws::String, Aws::String> attributes) override;

            private:
                opentelemetry::nostd::unique_ptr<opentelemetry::metrics::Histogram<double>> otelHistogram;
            };

            struct GaugeHandleState {
                std::function<void(Aws::UniquePtr<AsyncMeasurement>)> callback;
            };

            class OtelGaugeAdapter final : public GaugeHandle {
            public:
                explicit OtelGaugeAdapter(
                    opentelemetry::nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> otelGauge,
                        opentelemetry::metrics::ObservableCallbackPtr callback);

                void Stop() override;

            private:
                opentelemetry::nostd::shared_ptr<opentelemetry::metrics::ObservableInstrument> otelGauge;
                opentelemetry::metrics::ObservableCallbackPtr otelCallback;
            };

            class OtelObserverAdapter final : public AsyncMeasurement {
            public:
                explicit OtelObserverAdapter(const opentelemetry::metrics::ObserverResult &otelResult);

                void Record(double value, const Aws::Map<Aws::String, Aws::String> &attributes) override;

            private:
                const opentelemetry::metrics::ObserverResult &otelResult;
            };
        }
    }
}