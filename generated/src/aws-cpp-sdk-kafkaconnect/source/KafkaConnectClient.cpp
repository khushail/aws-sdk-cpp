/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/core/utils/Outcome.h>
#include <aws/core/auth/AWSAuthSigner.h>
#include <aws/core/client/CoreErrors.h>
#include <aws/core/client/RetryStrategy.h>
#include <aws/core/http/HttpClient.h>
#include <aws/core/http/HttpResponse.h>
#include <aws/core/http/HttpClientFactory.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/utils/threading/Executor.h>
#include <aws/core/utils/DNS.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/logging/ErrorMacros.h>

#include <aws/kafkaconnect/KafkaConnectClient.h>
#include <aws/kafkaconnect/KafkaConnectErrorMarshaller.h>
#include <aws/kafkaconnect/KafkaConnectEndpointProvider.h>
#include <aws/kafkaconnect/model/CreateConnectorRequest.h>
#include <aws/kafkaconnect/model/CreateCustomPluginRequest.h>
#include <aws/kafkaconnect/model/CreateWorkerConfigurationRequest.h>
#include <aws/kafkaconnect/model/DeleteConnectorRequest.h>
#include <aws/kafkaconnect/model/DeleteCustomPluginRequest.h>
#include <aws/kafkaconnect/model/DescribeConnectorRequest.h>
#include <aws/kafkaconnect/model/DescribeCustomPluginRequest.h>
#include <aws/kafkaconnect/model/DescribeWorkerConfigurationRequest.h>
#include <aws/kafkaconnect/model/ListConnectorsRequest.h>
#include <aws/kafkaconnect/model/ListCustomPluginsRequest.h>
#include <aws/kafkaconnect/model/ListWorkerConfigurationsRequest.h>
#include <aws/kafkaconnect/model/UpdateConnectorRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::KafkaConnect;
using namespace Aws::KafkaConnect::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* KafkaConnectClient::SERVICE_NAME = "kafkaconnect";
const char* KafkaConnectClient::ALLOCATION_TAG = "KafkaConnectClient";

KafkaConnectClient::KafkaConnectClient(const KafkaConnect::KafkaConnectClientConfiguration& clientConfiguration,
                                       std::shared_ptr<KafkaConnectEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KafkaConnectErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

KafkaConnectClient::KafkaConnectClient(const AWSCredentials& credentials,
                                       std::shared_ptr<KafkaConnectEndpointProviderBase> endpointProvider,
                                       const KafkaConnect::KafkaConnectClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KafkaConnectErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

KafkaConnectClient::KafkaConnectClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                       std::shared_ptr<KafkaConnectEndpointProviderBase> endpointProvider,
                                       const KafkaConnect::KafkaConnectClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KafkaConnectErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  KafkaConnectClient::KafkaConnectClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KafkaConnectErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<KafkaConnectEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

KafkaConnectClient::KafkaConnectClient(const AWSCredentials& credentials,
                                       const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KafkaConnectErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<KafkaConnectEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

KafkaConnectClient::KafkaConnectClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                       const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KafkaConnectErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<KafkaConnectEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
KafkaConnectClient::~KafkaConnectClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<KafkaConnectEndpointProviderBase>& KafkaConnectClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void KafkaConnectClient::init(const KafkaConnect::KafkaConnectClientConfiguration& config)
{
  AWSClient::SetServiceClientName("KafkaConnect");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void KafkaConnectClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

CreateConnectorOutcome KafkaConnectClient::CreateConnector(const CreateConnectorRequest& request) const
{
  AWS_OPERATION_GUARD(CreateConnector);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateConnector, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".CreateConnector",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateConnectorOutcome>(
    [&]()-> CreateConnectorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateConnector, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/connectors");
      return CreateConnectorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateCustomPluginOutcome KafkaConnectClient::CreateCustomPlugin(const CreateCustomPluginRequest& request) const
{
  AWS_OPERATION_GUARD(CreateCustomPlugin);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateCustomPlugin, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".CreateCustomPlugin",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateCustomPluginOutcome>(
    [&]()-> CreateCustomPluginOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateCustomPlugin, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/custom-plugins");
      return CreateCustomPluginOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateWorkerConfigurationOutcome KafkaConnectClient::CreateWorkerConfiguration(const CreateWorkerConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(CreateWorkerConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateWorkerConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".CreateWorkerConfiguration",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateWorkerConfigurationOutcome>(
    [&]()-> CreateWorkerConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateWorkerConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/worker-configurations");
      return CreateWorkerConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteConnectorOutcome KafkaConnectClient::DeleteConnector(const DeleteConnectorRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteConnector);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteConnector, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ConnectorArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteConnector", "Required field: ConnectorArn, is not set");
    return DeleteConnectorOutcome(Aws::Client::AWSError<KafkaConnectErrors>(KafkaConnectErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ConnectorArn]", false));
  }
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".DeleteConnector",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteConnectorOutcome>(
    [&]()-> DeleteConnectorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteConnector, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/connectors/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetConnectorArn());
      return DeleteConnectorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteCustomPluginOutcome KafkaConnectClient::DeleteCustomPlugin(const DeleteCustomPluginRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteCustomPlugin);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteCustomPlugin, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.CustomPluginArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteCustomPlugin", "Required field: CustomPluginArn, is not set");
    return DeleteCustomPluginOutcome(Aws::Client::AWSError<KafkaConnectErrors>(KafkaConnectErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [CustomPluginArn]", false));
  }
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".DeleteCustomPlugin",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteCustomPluginOutcome>(
    [&]()-> DeleteCustomPluginOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteCustomPlugin, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/custom-plugins/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetCustomPluginArn());
      return DeleteCustomPluginOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeConnectorOutcome KafkaConnectClient::DescribeConnector(const DescribeConnectorRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeConnector);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeConnector, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ConnectorArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeConnector", "Required field: ConnectorArn, is not set");
    return DescribeConnectorOutcome(Aws::Client::AWSError<KafkaConnectErrors>(KafkaConnectErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ConnectorArn]", false));
  }
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".DescribeConnector",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeConnectorOutcome>(
    [&]()-> DescribeConnectorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeConnector, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/connectors/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetConnectorArn());
      return DescribeConnectorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeCustomPluginOutcome KafkaConnectClient::DescribeCustomPlugin(const DescribeCustomPluginRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeCustomPlugin);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeCustomPlugin, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.CustomPluginArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeCustomPlugin", "Required field: CustomPluginArn, is not set");
    return DescribeCustomPluginOutcome(Aws::Client::AWSError<KafkaConnectErrors>(KafkaConnectErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [CustomPluginArn]", false));
  }
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".DescribeCustomPlugin",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeCustomPluginOutcome>(
    [&]()-> DescribeCustomPluginOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeCustomPlugin, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/custom-plugins/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetCustomPluginArn());
      return DescribeCustomPluginOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeWorkerConfigurationOutcome KafkaConnectClient::DescribeWorkerConfiguration(const DescribeWorkerConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeWorkerConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeWorkerConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkerConfigurationArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeWorkerConfiguration", "Required field: WorkerConfigurationArn, is not set");
    return DescribeWorkerConfigurationOutcome(Aws::Client::AWSError<KafkaConnectErrors>(KafkaConnectErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkerConfigurationArn]", false));
  }
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".DescribeWorkerConfiguration",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeWorkerConfigurationOutcome>(
    [&]()-> DescribeWorkerConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeWorkerConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/worker-configurations/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkerConfigurationArn());
      return DescribeWorkerConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListConnectorsOutcome KafkaConnectClient::ListConnectors(const ListConnectorsRequest& request) const
{
  AWS_OPERATION_GUARD(ListConnectors);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListConnectors, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".ListConnectors",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListConnectorsOutcome>(
    [&]()-> ListConnectorsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListConnectors, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/connectors");
      return ListConnectorsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListCustomPluginsOutcome KafkaConnectClient::ListCustomPlugins(const ListCustomPluginsRequest& request) const
{
  AWS_OPERATION_GUARD(ListCustomPlugins);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListCustomPlugins, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".ListCustomPlugins",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListCustomPluginsOutcome>(
    [&]()-> ListCustomPluginsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListCustomPlugins, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/custom-plugins");
      return ListCustomPluginsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListWorkerConfigurationsOutcome KafkaConnectClient::ListWorkerConfigurations(const ListWorkerConfigurationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListWorkerConfigurations);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListWorkerConfigurations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".ListWorkerConfigurations",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListWorkerConfigurationsOutcome>(
    [&]()-> ListWorkerConfigurationsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListWorkerConfigurations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/worker-configurations");
      return ListWorkerConfigurationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateConnectorOutcome KafkaConnectClient::UpdateConnector(const UpdateConnectorRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateConnector);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateConnector, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ConnectorArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateConnector", "Required field: ConnectorArn, is not set");
    return UpdateConnectorOutcome(Aws::Client::AWSError<KafkaConnectErrors>(KafkaConnectErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ConnectorArn]", false));
  }
  if (!request.CurrentVersionHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateConnector", "Required field: CurrentVersion, is not set");
    return UpdateConnectorOutcome(Aws::Client::AWSError<KafkaConnectErrors>(KafkaConnectErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [CurrentVersion]", false));
  }
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".UpdateConnector",
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateConnectorOutcome>(
    [&]()-> UpdateConnectorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateConnector, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/connectors/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetConnectorArn());
      return UpdateConnectorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

