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

#include <aws/swf/SWFClient.h>
#include <aws/swf/SWFErrorMarshaller.h>
#include <aws/swf/SWFEndpointProvider.h>
#include <aws/swf/model/CountClosedWorkflowExecutionsRequest.h>
#include <aws/swf/model/CountOpenWorkflowExecutionsRequest.h>
#include <aws/swf/model/CountPendingActivityTasksRequest.h>
#include <aws/swf/model/CountPendingDecisionTasksRequest.h>
#include <aws/swf/model/DeprecateActivityTypeRequest.h>
#include <aws/swf/model/DeprecateDomainRequest.h>
#include <aws/swf/model/DeprecateWorkflowTypeRequest.h>
#include <aws/swf/model/DescribeActivityTypeRequest.h>
#include <aws/swf/model/DescribeDomainRequest.h>
#include <aws/swf/model/DescribeWorkflowExecutionRequest.h>
#include <aws/swf/model/DescribeWorkflowTypeRequest.h>
#include <aws/swf/model/GetWorkflowExecutionHistoryRequest.h>
#include <aws/swf/model/ListActivityTypesRequest.h>
#include <aws/swf/model/ListClosedWorkflowExecutionsRequest.h>
#include <aws/swf/model/ListDomainsRequest.h>
#include <aws/swf/model/ListOpenWorkflowExecutionsRequest.h>
#include <aws/swf/model/ListTagsForResourceRequest.h>
#include <aws/swf/model/ListWorkflowTypesRequest.h>
#include <aws/swf/model/PollForActivityTaskRequest.h>
#include <aws/swf/model/PollForDecisionTaskRequest.h>
#include <aws/swf/model/RecordActivityTaskHeartbeatRequest.h>
#include <aws/swf/model/RegisterActivityTypeRequest.h>
#include <aws/swf/model/RegisterDomainRequest.h>
#include <aws/swf/model/RegisterWorkflowTypeRequest.h>
#include <aws/swf/model/RequestCancelWorkflowExecutionRequest.h>
#include <aws/swf/model/RespondActivityTaskCanceledRequest.h>
#include <aws/swf/model/RespondActivityTaskCompletedRequest.h>
#include <aws/swf/model/RespondActivityTaskFailedRequest.h>
#include <aws/swf/model/RespondDecisionTaskCompletedRequest.h>
#include <aws/swf/model/SignalWorkflowExecutionRequest.h>
#include <aws/swf/model/StartWorkflowExecutionRequest.h>
#include <aws/swf/model/TagResourceRequest.h>
#include <aws/swf/model/TerminateWorkflowExecutionRequest.h>
#include <aws/swf/model/UndeprecateActivityTypeRequest.h>
#include <aws/swf/model/UndeprecateDomainRequest.h>
#include <aws/swf/model/UndeprecateWorkflowTypeRequest.h>
#include <aws/swf/model/UntagResourceRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::SWF;
using namespace Aws::SWF::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* SWFClient::SERVICE_NAME = "swf";
const char* SWFClient::ALLOCATION_TAG = "SWFClient";

SWFClient::SWFClient(const SWF::SWFClientConfiguration& clientConfiguration,
                     std::shared_ptr<SWFEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SWFErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

SWFClient::SWFClient(const AWSCredentials& credentials,
                     std::shared_ptr<SWFEndpointProviderBase> endpointProvider,
                     const SWF::SWFClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SWFErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

SWFClient::SWFClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                     std::shared_ptr<SWFEndpointProviderBase> endpointProvider,
                     const SWF::SWFClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SWFErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  SWFClient::SWFClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SWFErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<SWFEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

SWFClient::SWFClient(const AWSCredentials& credentials,
                     const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SWFErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<SWFEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

SWFClient::SWFClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                     const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<SWFErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<SWFEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
SWFClient::~SWFClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<SWFEndpointProviderBase>& SWFClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void SWFClient::init(const SWF::SWFClientConfiguration& config)
{
  AWSClient::SetServiceClientName("SWF");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void SWFClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

CountClosedWorkflowExecutionsOutcome SWFClient::CountClosedWorkflowExecutions(const CountClosedWorkflowExecutionsRequest& request) const
{
  AWS_OPERATION_GUARD(CountClosedWorkflowExecutions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CountClosedWorkflowExecutions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CountClosedWorkflowExecutionsOutcome>(
    [&]()-> CountClosedWorkflowExecutionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CountClosedWorkflowExecutions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CountClosedWorkflowExecutionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CountOpenWorkflowExecutionsOutcome SWFClient::CountOpenWorkflowExecutions(const CountOpenWorkflowExecutionsRequest& request) const
{
  AWS_OPERATION_GUARD(CountOpenWorkflowExecutions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CountOpenWorkflowExecutions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CountOpenWorkflowExecutionsOutcome>(
    [&]()-> CountOpenWorkflowExecutionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CountOpenWorkflowExecutions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CountOpenWorkflowExecutionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CountPendingActivityTasksOutcome SWFClient::CountPendingActivityTasks(const CountPendingActivityTasksRequest& request) const
{
  AWS_OPERATION_GUARD(CountPendingActivityTasks);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CountPendingActivityTasks, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CountPendingActivityTasksOutcome>(
    [&]()-> CountPendingActivityTasksOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CountPendingActivityTasks, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CountPendingActivityTasksOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CountPendingDecisionTasksOutcome SWFClient::CountPendingDecisionTasks(const CountPendingDecisionTasksRequest& request) const
{
  AWS_OPERATION_GUARD(CountPendingDecisionTasks);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CountPendingDecisionTasks, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CountPendingDecisionTasksOutcome>(
    [&]()-> CountPendingDecisionTasksOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CountPendingDecisionTasks, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CountPendingDecisionTasksOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeprecateActivityTypeOutcome SWFClient::DeprecateActivityType(const DeprecateActivityTypeRequest& request) const
{
  AWS_OPERATION_GUARD(DeprecateActivityType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeprecateActivityType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeprecateActivityTypeOutcome>(
    [&]()-> DeprecateActivityTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeprecateActivityType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeprecateActivityTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeprecateDomainOutcome SWFClient::DeprecateDomain(const DeprecateDomainRequest& request) const
{
  AWS_OPERATION_GUARD(DeprecateDomain);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeprecateDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeprecateDomainOutcome>(
    [&]()-> DeprecateDomainOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeprecateDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeprecateDomainOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeprecateWorkflowTypeOutcome SWFClient::DeprecateWorkflowType(const DeprecateWorkflowTypeRequest& request) const
{
  AWS_OPERATION_GUARD(DeprecateWorkflowType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeprecateWorkflowType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeprecateWorkflowTypeOutcome>(
    [&]()-> DeprecateWorkflowTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeprecateWorkflowType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeprecateWorkflowTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeActivityTypeOutcome SWFClient::DescribeActivityType(const DescribeActivityTypeRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeActivityType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeActivityType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeActivityTypeOutcome>(
    [&]()-> DescribeActivityTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeActivityType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeActivityTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDomainOutcome SWFClient::DescribeDomain(const DescribeDomainRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDomain);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDomainOutcome>(
    [&]()-> DescribeDomainOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeDomainOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeWorkflowExecutionOutcome SWFClient::DescribeWorkflowExecution(const DescribeWorkflowExecutionRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeWorkflowExecution);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeWorkflowExecutionOutcome>(
    [&]()-> DescribeWorkflowExecutionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeWorkflowExecutionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeWorkflowTypeOutcome SWFClient::DescribeWorkflowType(const DescribeWorkflowTypeRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeWorkflowType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeWorkflowType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeWorkflowTypeOutcome>(
    [&]()-> DescribeWorkflowTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeWorkflowType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeWorkflowTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetWorkflowExecutionHistoryOutcome SWFClient::GetWorkflowExecutionHistory(const GetWorkflowExecutionHistoryRequest& request) const
{
  AWS_OPERATION_GUARD(GetWorkflowExecutionHistory);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetWorkflowExecutionHistory, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetWorkflowExecutionHistoryOutcome>(
    [&]()-> GetWorkflowExecutionHistoryOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetWorkflowExecutionHistory, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetWorkflowExecutionHistoryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListActivityTypesOutcome SWFClient::ListActivityTypes(const ListActivityTypesRequest& request) const
{
  AWS_OPERATION_GUARD(ListActivityTypes);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListActivityTypes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListActivityTypesOutcome>(
    [&]()-> ListActivityTypesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListActivityTypes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListActivityTypesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListClosedWorkflowExecutionsOutcome SWFClient::ListClosedWorkflowExecutions(const ListClosedWorkflowExecutionsRequest& request) const
{
  AWS_OPERATION_GUARD(ListClosedWorkflowExecutions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListClosedWorkflowExecutions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListClosedWorkflowExecutionsOutcome>(
    [&]()-> ListClosedWorkflowExecutionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListClosedWorkflowExecutions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListClosedWorkflowExecutionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListDomainsOutcome SWFClient::ListDomains(const ListDomainsRequest& request) const
{
  AWS_OPERATION_GUARD(ListDomains);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListDomains, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListDomainsOutcome>(
    [&]()-> ListDomainsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListDomains, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListDomainsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListOpenWorkflowExecutionsOutcome SWFClient::ListOpenWorkflowExecutions(const ListOpenWorkflowExecutionsRequest& request) const
{
  AWS_OPERATION_GUARD(ListOpenWorkflowExecutions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListOpenWorkflowExecutions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListOpenWorkflowExecutionsOutcome>(
    [&]()-> ListOpenWorkflowExecutionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListOpenWorkflowExecutions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListOpenWorkflowExecutionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListTagsForResourceOutcome SWFClient::ListTagsForResource(const ListTagsForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(ListTagsForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListTagsForResourceOutcome>(
    [&]()-> ListTagsForResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListTagsForResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListWorkflowTypesOutcome SWFClient::ListWorkflowTypes(const ListWorkflowTypesRequest& request) const
{
  AWS_OPERATION_GUARD(ListWorkflowTypes);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListWorkflowTypes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListWorkflowTypesOutcome>(
    [&]()-> ListWorkflowTypesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListWorkflowTypes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListWorkflowTypesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

PollForActivityTaskOutcome SWFClient::PollForActivityTask(const PollForActivityTaskRequest& request) const
{
  AWS_OPERATION_GUARD(PollForActivityTask);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, PollForActivityTask, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<PollForActivityTaskOutcome>(
    [&]()-> PollForActivityTaskOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, PollForActivityTask, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return PollForActivityTaskOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

PollForDecisionTaskOutcome SWFClient::PollForDecisionTask(const PollForDecisionTaskRequest& request) const
{
  AWS_OPERATION_GUARD(PollForDecisionTask);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, PollForDecisionTask, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<PollForDecisionTaskOutcome>(
    [&]()-> PollForDecisionTaskOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, PollForDecisionTask, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return PollForDecisionTaskOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RecordActivityTaskHeartbeatOutcome SWFClient::RecordActivityTaskHeartbeat(const RecordActivityTaskHeartbeatRequest& request) const
{
  AWS_OPERATION_GUARD(RecordActivityTaskHeartbeat);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RecordActivityTaskHeartbeat, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RecordActivityTaskHeartbeatOutcome>(
    [&]()-> RecordActivityTaskHeartbeatOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RecordActivityTaskHeartbeat, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RecordActivityTaskHeartbeatOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RegisterActivityTypeOutcome SWFClient::RegisterActivityType(const RegisterActivityTypeRequest& request) const
{
  AWS_OPERATION_GUARD(RegisterActivityType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RegisterActivityType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RegisterActivityTypeOutcome>(
    [&]()-> RegisterActivityTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RegisterActivityType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RegisterActivityTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RegisterDomainOutcome SWFClient::RegisterDomain(const RegisterDomainRequest& request) const
{
  AWS_OPERATION_GUARD(RegisterDomain);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RegisterDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RegisterDomainOutcome>(
    [&]()-> RegisterDomainOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RegisterDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RegisterDomainOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RegisterWorkflowTypeOutcome SWFClient::RegisterWorkflowType(const RegisterWorkflowTypeRequest& request) const
{
  AWS_OPERATION_GUARD(RegisterWorkflowType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RegisterWorkflowType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RegisterWorkflowTypeOutcome>(
    [&]()-> RegisterWorkflowTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RegisterWorkflowType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RegisterWorkflowTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RequestCancelWorkflowExecutionOutcome SWFClient::RequestCancelWorkflowExecution(const RequestCancelWorkflowExecutionRequest& request) const
{
  AWS_OPERATION_GUARD(RequestCancelWorkflowExecution);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RequestCancelWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RequestCancelWorkflowExecutionOutcome>(
    [&]()-> RequestCancelWorkflowExecutionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RequestCancelWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RequestCancelWorkflowExecutionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RespondActivityTaskCanceledOutcome SWFClient::RespondActivityTaskCanceled(const RespondActivityTaskCanceledRequest& request) const
{
  AWS_OPERATION_GUARD(RespondActivityTaskCanceled);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RespondActivityTaskCanceled, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RespondActivityTaskCanceledOutcome>(
    [&]()-> RespondActivityTaskCanceledOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RespondActivityTaskCanceled, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RespondActivityTaskCanceledOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RespondActivityTaskCompletedOutcome SWFClient::RespondActivityTaskCompleted(const RespondActivityTaskCompletedRequest& request) const
{
  AWS_OPERATION_GUARD(RespondActivityTaskCompleted);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RespondActivityTaskCompleted, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RespondActivityTaskCompletedOutcome>(
    [&]()-> RespondActivityTaskCompletedOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RespondActivityTaskCompleted, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RespondActivityTaskCompletedOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RespondActivityTaskFailedOutcome SWFClient::RespondActivityTaskFailed(const RespondActivityTaskFailedRequest& request) const
{
  AWS_OPERATION_GUARD(RespondActivityTaskFailed);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RespondActivityTaskFailed, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RespondActivityTaskFailedOutcome>(
    [&]()-> RespondActivityTaskFailedOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RespondActivityTaskFailed, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RespondActivityTaskFailedOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RespondDecisionTaskCompletedOutcome SWFClient::RespondDecisionTaskCompleted(const RespondDecisionTaskCompletedRequest& request) const
{
  AWS_OPERATION_GUARD(RespondDecisionTaskCompleted);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RespondDecisionTaskCompleted, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RespondDecisionTaskCompletedOutcome>(
    [&]()-> RespondDecisionTaskCompletedOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RespondDecisionTaskCompleted, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RespondDecisionTaskCompletedOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

SignalWorkflowExecutionOutcome SWFClient::SignalWorkflowExecution(const SignalWorkflowExecutionRequest& request) const
{
  AWS_OPERATION_GUARD(SignalWorkflowExecution);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, SignalWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<SignalWorkflowExecutionOutcome>(
    [&]()-> SignalWorkflowExecutionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, SignalWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return SignalWorkflowExecutionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StartWorkflowExecutionOutcome SWFClient::StartWorkflowExecution(const StartWorkflowExecutionRequest& request) const
{
  AWS_OPERATION_GUARD(StartWorkflowExecution);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartWorkflowExecutionOutcome>(
    [&]()-> StartWorkflowExecutionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return StartWorkflowExecutionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

TagResourceOutcome SWFClient::TagResource(const TagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(TagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, TagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<TagResourceOutcome>(
    [&]()-> TagResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, TagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return TagResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

TerminateWorkflowExecutionOutcome SWFClient::TerminateWorkflowExecution(const TerminateWorkflowExecutionRequest& request) const
{
  AWS_OPERATION_GUARD(TerminateWorkflowExecution);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, TerminateWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<TerminateWorkflowExecutionOutcome>(
    [&]()-> TerminateWorkflowExecutionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, TerminateWorkflowExecution, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return TerminateWorkflowExecutionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UndeprecateActivityTypeOutcome SWFClient::UndeprecateActivityType(const UndeprecateActivityTypeRequest& request) const
{
  AWS_OPERATION_GUARD(UndeprecateActivityType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UndeprecateActivityType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UndeprecateActivityTypeOutcome>(
    [&]()-> UndeprecateActivityTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UndeprecateActivityType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UndeprecateActivityTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UndeprecateDomainOutcome SWFClient::UndeprecateDomain(const UndeprecateDomainRequest& request) const
{
  AWS_OPERATION_GUARD(UndeprecateDomain);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UndeprecateDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UndeprecateDomainOutcome>(
    [&]()-> UndeprecateDomainOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UndeprecateDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UndeprecateDomainOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UndeprecateWorkflowTypeOutcome SWFClient::UndeprecateWorkflowType(const UndeprecateWorkflowTypeRequest& request) const
{
  AWS_OPERATION_GUARD(UndeprecateWorkflowType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UndeprecateWorkflowType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UndeprecateWorkflowTypeOutcome>(
    [&]()-> UndeprecateWorkflowTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UndeprecateWorkflowType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UndeprecateWorkflowTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UntagResourceOutcome SWFClient::UntagResource(const UntagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(UntagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UntagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UntagResourceOutcome>(
    [&]()-> UntagResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UntagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UntagResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

