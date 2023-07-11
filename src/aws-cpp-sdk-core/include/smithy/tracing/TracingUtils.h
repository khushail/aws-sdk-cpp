/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#pragma once

#include <aws/core/monitoring/HttpClientMetrics.h>
#include <smithy/Smithy_EXPORTS.h>
#include <smithy/tracing/Meter.h>
#include <functional>
#include <chrono>
#include <utility>

namespace smithy {
    namespace components {
        namespace tracing {
            static const char SMITHY_METRICS_DNS_DURATION[] = "smithy.client.http.dns_duration";
            static const char SMITHY_METRICS_CONNECT_DURATION[] = "smithy.client.http.connect_duration";
            static const char SMITHY_METRICS_SSL_DURATION[] = "smithy.client.http.ssl_duration";
            static const char SMITHY_METRICS_THROUGHPUT[] = "smithy.client.http.throughput";
            static const char SMITHY_METRICS_UNKNOWN_METRIC[] = "smithy.client.http.unknown_metric";

            class SMITHY_API TracingUtils {
            public:
                TracingUtils() = default;

                template<typename T>
                static T MakeCallWithTiming(std::function<T()> func,
                    Aws::String metricName,
                    std::shared_ptr<Meter> meter,
                    Aws::Map<Aws::String, Aws::String> attributes,
                    Aws::String description = "") {
                    auto before = std::chrono::high_resolution_clock::now();
                    auto returnValue = func();
                    auto after = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
                    auto histogram = meter->CreateHistogram(std::move(metricName), "ms", std::move(description));
                    histogram->record(duration, std::move(attributes));
                    return returnValue;
                }

                static void MakeCallWithTiming(std::function<void(void)> func,
                    Aws::String metricName,
                    std::shared_ptr<Meter> meter,
                    Aws::Map<Aws::String, Aws::String> attributes,
                    Aws::String description = "") {
                    auto before = std::chrono::high_resolution_clock::now();
                    func();
                    auto after = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
                    auto histogram = meter->CreateHistogram(std::move(metricName), "ms", std::move(description));
                    histogram->record(duration, std::move(attributes));
                }

                static void EmitCoreHttpMetrics(const Aws::Monitoring::HttpClientMetricsCollection &metrics,
                    std::shared_ptr<Meter> meter,
                    Aws::Map<Aws::String, Aws::String> attributes,
                    Aws::String description = "") {
                    for (auto const &entry: metrics) {
                        auto smithyMetric = ConvertCoreMetricToSmithy(entry.first);
                        if (smithyMetric.first != SMITHY_METRICS_UNKNOWN_METRIC) {
                            auto histogram = meter->CreateHistogram(std::move(smithyMetric.first),
                                smithyMetric.second,
                                std::move(description));
                            histogram->record(entry.second, std::move(attributes));
                        }
                    }
                }

                static std::pair<Aws::String, Aws::String> ConvertCoreMetricToSmithy(const Aws::String &name) {
                    static Aws::Map<int, std::pair<Aws::String, Aws::String>> metricsTypeToName =
                        {
                            std::pair<int, std::pair<Aws::String, Aws::String>>(
                                static_cast<int>(Aws::Monitoring::HttpClientMetricsType::DnsLatency),
                                std::make_pair(SMITHY_METRICS_DNS_DURATION, "ms")),
                            std::pair<int, std::pair<Aws::String, Aws::String>>(
                                static_cast<int>(Aws::Monitoring::HttpClientMetricsType::ConnectLatency),
                                std::make_pair(SMITHY_METRICS_CONNECT_DURATION, "ms")),
                            std::pair<int, std::pair<Aws::String, Aws::String>>(
                                static_cast<int>(Aws::Monitoring::HttpClientMetricsType::SslLatency),
                                std::make_pair(SMITHY_METRICS_SSL_DURATION, "ms")),
                            std::pair<int, std::pair<Aws::String, Aws::String>>(
                                static_cast<int>(Aws::Monitoring::HttpClientMetricsType::Throughput),
                                std::make_pair(SMITHY_METRICS_THROUGHPUT, "bytes/s")),
                        };

                    auto metricType = Aws::Monitoring::GetHttpClientMetricTypeByName(name);
                    auto it = metricsTypeToName.find(static_cast<int>(metricType));
                    if (it == metricsTypeToName.end()) {
                        return std::make_pair(SMITHY_METRICS_UNKNOWN_METRIC, "unknown");
                    }
                    return it->second;
                }
            };
        }
    }
}