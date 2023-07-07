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

#include <aws/securitylake/SecurityLakeClient.h>
#include <aws/securitylake/SecurityLakeErrorMarshaller.h>
#include <aws/securitylake/SecurityLakeEndpointProvider.h>
#include <aws/securitylake/model/CreateAwsLogSourceRequest.h>
#include <aws/securitylake/model/CreateCustomLogSourceRequest.h>
#include <aws/securitylake/model/CreateDataLakeRequest.h>
#include <aws/securitylake/model/CreateDataLakeExceptionSubscriptionRequest.h>
#include <aws/securitylake/model/CreateDataLakeOrganizationConfigurationRequest.h>
#include <aws/securitylake/model/CreateSubscriberRequest.h>
#include <aws/securitylake/model/CreateSubscriberNotificationRequest.h>
#include <aws/securitylake/model/DeleteAwsLogSourceRequest.h>
#include <aws/securitylake/model/DeleteCustomLogSourceRequest.h>
#include <aws/securitylake/model/DeleteDataLakeRequest.h>
#include <aws/securitylake/model/DeleteDataLakeExceptionSubscriptionRequest.h>
#include <aws/securitylake/model/DeleteDataLakeOrganizationConfigurationRequest.h>
#include <aws/securitylake/model/DeleteSubscriberRequest.h>
#include <aws/securitylake/model/DeleteSubscriberNotificationRequest.h>
#include <aws/securitylake/model/DeregisterDataLakeDelegatedAdministratorRequest.h>
#include <aws/securitylake/model/GetDataLakeExceptionSubscriptionRequest.h>
#include <aws/securitylake/model/GetDataLakeOrganizationConfigurationRequest.h>
#include <aws/securitylake/model/GetDataLakeSourcesRequest.h>
#include <aws/securitylake/model/GetSubscriberRequest.h>
#include <aws/securitylake/model/ListDataLakeExceptionsRequest.h>
#include <aws/securitylake/model/ListDataLakesRequest.h>
#include <aws/securitylake/model/ListLogSourcesRequest.h>
#include <aws/securitylake/model/ListSubscribersRequest.h>
#include <aws/securitylake/model/RegisterDataLakeDelegatedAdministratorRequest.h>
#include <aws/securitylake/model/UpdateDataLakeRequest.h>
#include <aws/securitylake/model/UpdateDataLakeExceptionSubscriptionRequest.h>
#include <aws/securitylake/model/UpdateSubscriberRequest.h>
#include <aws/securitylake/model/UpdateSubscriberNotificationRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::SecurityLake;
using namespace Aws::SecurityLake::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* SecurityLakeClient::SERVICE_NAME = "securitylake";
const char* SecurityLakeClient::ALLOCATION_TAG = "SecurityLakeClient";

SecurityLakeClient::SecurityLakeClient(const SecurityLake::SecurityLakeClientConfiguration& clientConfiguration,
                                       std::shared_ptr<SecurityLakeEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SecurityLakeErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

SecurityLakeClient::SecurityLakeClient(const AWSCredentials& credentials,
                                       std::shared_ptr<SecurityLakeEndpointProviderBase> endpointProvider,
                                       const SecurityLake::SecurityLakeClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SecurityLakeErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

SecurityLakeClient::SecurityLakeClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                       std::shared_ptr<SecurityLakeEndpointProviderBase> endpointProvider,
                                       const SecurityLake::SecurityLakeClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SecurityLakeErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  SecurityLakeClient::SecurityLakeClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SecurityLakeErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<SecurityLakeEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

SecurityLakeClient::SecurityLakeClient(const AWSCredentials& credentials,
                                       const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SecurityLakeErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<SecurityLakeEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

SecurityLakeClient::SecurityLakeClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                       const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SecurityLakeErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<SecurityLakeEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
SecurityLakeClient::~SecurityLakeClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<SecurityLakeEndpointProviderBase>& SecurityLakeClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void SecurityLakeClient::init(const SecurityLake::SecurityLakeClientConfiguration& config)
{
  AWSClient::SetServiceClientName("SecurityLake");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void SecurityLakeClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

CreateAwsLogSourceOutcome SecurityLakeClient::CreateAwsLogSource(const CreateAwsLogSourceRequest& request) const
{
  AWS_OPERATION_GUARD(CreateAwsLogSource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateAwsLogSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateAwsLogSourceOutcome>(
    [&]()-> CreateAwsLogSourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateAwsLogSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/logsources/aws");
      return CreateAwsLogSourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateCustomLogSourceOutcome SecurityLakeClient::CreateCustomLogSource(const CreateCustomLogSourceRequest& request) const
{
  AWS_OPERATION_GUARD(CreateCustomLogSource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateCustomLogSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateCustomLogSourceOutcome>(
    [&]()-> CreateCustomLogSourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateCustomLogSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/logsources/custom");
      return CreateCustomLogSourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDataLakeOutcome SecurityLakeClient::CreateDataLake(const CreateDataLakeRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDataLake);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDataLake, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDataLakeOutcome>(
    [&]()-> CreateDataLakeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDataLake, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake");
      return CreateDataLakeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDataLakeExceptionSubscriptionOutcome SecurityLakeClient::CreateDataLakeExceptionSubscription(const CreateDataLakeExceptionSubscriptionRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDataLakeExceptionSubscription);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDataLakeExceptionSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDataLakeExceptionSubscriptionOutcome>(
    [&]()-> CreateDataLakeExceptionSubscriptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDataLakeExceptionSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/exceptions/subscription");
      return CreateDataLakeExceptionSubscriptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDataLakeOrganizationConfigurationOutcome SecurityLakeClient::CreateDataLakeOrganizationConfiguration(const CreateDataLakeOrganizationConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDataLakeOrganizationConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDataLakeOrganizationConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDataLakeOrganizationConfigurationOutcome>(
    [&]()-> CreateDataLakeOrganizationConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDataLakeOrganizationConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/organization/configuration");
      return CreateDataLakeOrganizationConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateSubscriberOutcome SecurityLakeClient::CreateSubscriber(const CreateSubscriberRequest& request) const
{
  AWS_OPERATION_GUARD(CreateSubscriber);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateSubscriberOutcome>(
    [&]()-> CreateSubscriberOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/subscribers");
      return CreateSubscriberOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateSubscriberNotificationOutcome SecurityLakeClient::CreateSubscriberNotification(const CreateSubscriberNotificationRequest& request) const
{
  AWS_OPERATION_GUARD(CreateSubscriberNotification);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateSubscriberNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SubscriberIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateSubscriberNotification", "Required field: SubscriberId, is not set");
    return CreateSubscriberNotificationOutcome(Aws::Client::AWSError<SecurityLakeErrors>(SecurityLakeErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [SubscriberId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateSubscriberNotificationOutcome>(
    [&]()-> CreateSubscriberNotificationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateSubscriberNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/subscribers/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetSubscriberId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/notification");
      return CreateSubscriberNotificationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteAwsLogSourceOutcome SecurityLakeClient::DeleteAwsLogSource(const DeleteAwsLogSourceRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteAwsLogSource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteAwsLogSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteAwsLogSourceOutcome>(
    [&]()-> DeleteAwsLogSourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteAwsLogSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/logsources/aws/delete");
      return DeleteAwsLogSourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteCustomLogSourceOutcome SecurityLakeClient::DeleteCustomLogSource(const DeleteCustomLogSourceRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteCustomLogSource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteCustomLogSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SourceNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteCustomLogSource", "Required field: SourceName, is not set");
    return DeleteCustomLogSourceOutcome(Aws::Client::AWSError<SecurityLakeErrors>(SecurityLakeErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [SourceName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteCustomLogSourceOutcome>(
    [&]()-> DeleteCustomLogSourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteCustomLogSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/logsources/custom/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetSourceName());
      return DeleteCustomLogSourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDataLakeOutcome SecurityLakeClient::DeleteDataLake(const DeleteDataLakeRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDataLake);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDataLake, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDataLakeOutcome>(
    [&]()-> DeleteDataLakeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDataLake, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/delete");
      return DeleteDataLakeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDataLakeExceptionSubscriptionOutcome SecurityLakeClient::DeleteDataLakeExceptionSubscription(const DeleteDataLakeExceptionSubscriptionRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDataLakeExceptionSubscription);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDataLakeExceptionSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDataLakeExceptionSubscriptionOutcome>(
    [&]()-> DeleteDataLakeExceptionSubscriptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDataLakeExceptionSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/exceptions/subscription");
      return DeleteDataLakeExceptionSubscriptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDataLakeOrganizationConfigurationOutcome SecurityLakeClient::DeleteDataLakeOrganizationConfiguration(const DeleteDataLakeOrganizationConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDataLakeOrganizationConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDataLakeOrganizationConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDataLakeOrganizationConfigurationOutcome>(
    [&]()-> DeleteDataLakeOrganizationConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDataLakeOrganizationConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/organization/configuration/delete");
      return DeleteDataLakeOrganizationConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteSubscriberOutcome SecurityLakeClient::DeleteSubscriber(const DeleteSubscriberRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteSubscriber);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SubscriberIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteSubscriber", "Required field: SubscriberId, is not set");
    return DeleteSubscriberOutcome(Aws::Client::AWSError<SecurityLakeErrors>(SecurityLakeErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [SubscriberId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteSubscriberOutcome>(
    [&]()-> DeleteSubscriberOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/subscribers/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetSubscriberId());
      return DeleteSubscriberOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteSubscriberNotificationOutcome SecurityLakeClient::DeleteSubscriberNotification(const DeleteSubscriberNotificationRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteSubscriberNotification);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteSubscriberNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SubscriberIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteSubscriberNotification", "Required field: SubscriberId, is not set");
    return DeleteSubscriberNotificationOutcome(Aws::Client::AWSError<SecurityLakeErrors>(SecurityLakeErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [SubscriberId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteSubscriberNotificationOutcome>(
    [&]()-> DeleteSubscriberNotificationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteSubscriberNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/subscribers/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetSubscriberId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/notification");
      return DeleteSubscriberNotificationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeregisterDataLakeDelegatedAdministratorOutcome SecurityLakeClient::DeregisterDataLakeDelegatedAdministrator(const DeregisterDataLakeDelegatedAdministratorRequest& request) const
{
  AWS_OPERATION_GUARD(DeregisterDataLakeDelegatedAdministrator);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeregisterDataLakeDelegatedAdministrator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeregisterDataLakeDelegatedAdministratorOutcome>(
    [&]()-> DeregisterDataLakeDelegatedAdministratorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeregisterDataLakeDelegatedAdministrator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/delegate");
      return DeregisterDataLakeDelegatedAdministratorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetDataLakeExceptionSubscriptionOutcome SecurityLakeClient::GetDataLakeExceptionSubscription(const GetDataLakeExceptionSubscriptionRequest& request) const
{
  AWS_OPERATION_GUARD(GetDataLakeExceptionSubscription);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetDataLakeExceptionSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetDataLakeExceptionSubscriptionOutcome>(
    [&]()-> GetDataLakeExceptionSubscriptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetDataLakeExceptionSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/exceptions/subscription");
      return GetDataLakeExceptionSubscriptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetDataLakeOrganizationConfigurationOutcome SecurityLakeClient::GetDataLakeOrganizationConfiguration(const GetDataLakeOrganizationConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(GetDataLakeOrganizationConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetDataLakeOrganizationConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetDataLakeOrganizationConfigurationOutcome>(
    [&]()-> GetDataLakeOrganizationConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetDataLakeOrganizationConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/organization/configuration");
      return GetDataLakeOrganizationConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetDataLakeSourcesOutcome SecurityLakeClient::GetDataLakeSources(const GetDataLakeSourcesRequest& request) const
{
  AWS_OPERATION_GUARD(GetDataLakeSources);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetDataLakeSources, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetDataLakeSourcesOutcome>(
    [&]()-> GetDataLakeSourcesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetDataLakeSources, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/sources");
      return GetDataLakeSourcesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetSubscriberOutcome SecurityLakeClient::GetSubscriber(const GetSubscriberRequest& request) const
{
  AWS_OPERATION_GUARD(GetSubscriber);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SubscriberIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetSubscriber", "Required field: SubscriberId, is not set");
    return GetSubscriberOutcome(Aws::Client::AWSError<SecurityLakeErrors>(SecurityLakeErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [SubscriberId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetSubscriberOutcome>(
    [&]()-> GetSubscriberOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/subscribers/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetSubscriberId());
      return GetSubscriberOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListDataLakeExceptionsOutcome SecurityLakeClient::ListDataLakeExceptions(const ListDataLakeExceptionsRequest& request) const
{
  AWS_OPERATION_GUARD(ListDataLakeExceptions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListDataLakeExceptions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListDataLakeExceptionsOutcome>(
    [&]()-> ListDataLakeExceptionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListDataLakeExceptions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/exceptions");
      return ListDataLakeExceptionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListDataLakesOutcome SecurityLakeClient::ListDataLakes(const ListDataLakesRequest& request) const
{
  AWS_OPERATION_GUARD(ListDataLakes);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListDataLakes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListDataLakesOutcome>(
    [&]()-> ListDataLakesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListDataLakes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalakes");
      return ListDataLakesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLogSourcesOutcome SecurityLakeClient::ListLogSources(const ListLogSourcesRequest& request) const
{
  AWS_OPERATION_GUARD(ListLogSources);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLogSources, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLogSourcesOutcome>(
    [&]()-> ListLogSourcesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLogSources, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/logsources/list");
      return ListLogSourcesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListSubscribersOutcome SecurityLakeClient::ListSubscribers(const ListSubscribersRequest& request) const
{
  AWS_OPERATION_GUARD(ListSubscribers);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListSubscribers, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListSubscribersOutcome>(
    [&]()-> ListSubscribersOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListSubscribers, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/subscribers");
      return ListSubscribersOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RegisterDataLakeDelegatedAdministratorOutcome SecurityLakeClient::RegisterDataLakeDelegatedAdministrator(const RegisterDataLakeDelegatedAdministratorRequest& request) const
{
  AWS_OPERATION_GUARD(RegisterDataLakeDelegatedAdministrator);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RegisterDataLakeDelegatedAdministrator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RegisterDataLakeDelegatedAdministratorOutcome>(
    [&]()-> RegisterDataLakeDelegatedAdministratorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RegisterDataLakeDelegatedAdministrator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/delegate");
      return RegisterDataLakeDelegatedAdministratorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateDataLakeOutcome SecurityLakeClient::UpdateDataLake(const UpdateDataLakeRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateDataLake);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateDataLake, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateDataLakeOutcome>(
    [&]()-> UpdateDataLakeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateDataLake, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake");
      return UpdateDataLakeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateDataLakeExceptionSubscriptionOutcome SecurityLakeClient::UpdateDataLakeExceptionSubscription(const UpdateDataLakeExceptionSubscriptionRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateDataLakeExceptionSubscription);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateDataLakeExceptionSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateDataLakeExceptionSubscriptionOutcome>(
    [&]()-> UpdateDataLakeExceptionSubscriptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateDataLakeExceptionSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/datalake/exceptions/subscription");
      return UpdateDataLakeExceptionSubscriptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateSubscriberOutcome SecurityLakeClient::UpdateSubscriber(const UpdateSubscriberRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateSubscriber);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SubscriberIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateSubscriber", "Required field: SubscriberId, is not set");
    return UpdateSubscriberOutcome(Aws::Client::AWSError<SecurityLakeErrors>(SecurityLakeErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [SubscriberId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateSubscriberOutcome>(
    [&]()-> UpdateSubscriberOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/subscribers/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetSubscriberId());
      return UpdateSubscriberOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateSubscriberNotificationOutcome SecurityLakeClient::UpdateSubscriberNotification(const UpdateSubscriberNotificationRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateSubscriberNotification);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateSubscriberNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.SubscriberIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateSubscriberNotification", "Required field: SubscriberId, is not set");
    return UpdateSubscriberNotificationOutcome(Aws::Client::AWSError<SecurityLakeErrors>(SecurityLakeErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [SubscriberId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateSubscriberNotificationOutcome>(
    [&]()-> UpdateSubscriberNotificationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateSubscriberNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/subscribers/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetSubscriberId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/notification");
      return UpdateSubscriberNotificationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

