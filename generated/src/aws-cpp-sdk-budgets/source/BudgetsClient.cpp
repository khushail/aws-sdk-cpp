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

#include <aws/budgets/BudgetsClient.h>
#include <aws/budgets/BudgetsErrorMarshaller.h>
#include <aws/budgets/BudgetsEndpointProvider.h>
#include <aws/budgets/model/CreateBudgetRequest.h>
#include <aws/budgets/model/CreateBudgetActionRequest.h>
#include <aws/budgets/model/CreateNotificationRequest.h>
#include <aws/budgets/model/CreateSubscriberRequest.h>
#include <aws/budgets/model/DeleteBudgetRequest.h>
#include <aws/budgets/model/DeleteBudgetActionRequest.h>
#include <aws/budgets/model/DeleteNotificationRequest.h>
#include <aws/budgets/model/DeleteSubscriberRequest.h>
#include <aws/budgets/model/DescribeBudgetRequest.h>
#include <aws/budgets/model/DescribeBudgetActionRequest.h>
#include <aws/budgets/model/DescribeBudgetActionHistoriesRequest.h>
#include <aws/budgets/model/DescribeBudgetActionsForAccountRequest.h>
#include <aws/budgets/model/DescribeBudgetActionsForBudgetRequest.h>
#include <aws/budgets/model/DescribeBudgetNotificationsForAccountRequest.h>
#include <aws/budgets/model/DescribeBudgetPerformanceHistoryRequest.h>
#include <aws/budgets/model/DescribeBudgetsRequest.h>
#include <aws/budgets/model/DescribeNotificationsForBudgetRequest.h>
#include <aws/budgets/model/DescribeSubscribersForNotificationRequest.h>
#include <aws/budgets/model/ExecuteBudgetActionRequest.h>
#include <aws/budgets/model/UpdateBudgetRequest.h>
#include <aws/budgets/model/UpdateBudgetActionRequest.h>
#include <aws/budgets/model/UpdateNotificationRequest.h>
#include <aws/budgets/model/UpdateSubscriberRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::Budgets;
using namespace Aws::Budgets::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* BudgetsClient::SERVICE_NAME = "budgets";
const char* BudgetsClient::ALLOCATION_TAG = "BudgetsClient";

BudgetsClient::BudgetsClient(const Budgets::BudgetsClientConfiguration& clientConfiguration,
                             std::shared_ptr<BudgetsEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<BudgetsErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

BudgetsClient::BudgetsClient(const AWSCredentials& credentials,
                             std::shared_ptr<BudgetsEndpointProviderBase> endpointProvider,
                             const Budgets::BudgetsClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<BudgetsErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

BudgetsClient::BudgetsClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                             std::shared_ptr<BudgetsEndpointProviderBase> endpointProvider,
                             const Budgets::BudgetsClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<BudgetsErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  BudgetsClient::BudgetsClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<BudgetsErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<BudgetsEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

BudgetsClient::BudgetsClient(const AWSCredentials& credentials,
                             const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<BudgetsErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<BudgetsEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

BudgetsClient::BudgetsClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                             const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<BudgetsErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<BudgetsEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
BudgetsClient::~BudgetsClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<BudgetsEndpointProviderBase>& BudgetsClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void BudgetsClient::init(const Budgets::BudgetsClientConfiguration& config)
{
  AWSClient::SetServiceClientName("Budgets");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void BudgetsClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

CreateBudgetOutcome BudgetsClient::CreateBudget(const CreateBudgetRequest& request) const
{
  AWS_OPERATION_GUARD(CreateBudget);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateBudgetOutcome>(
    [&]()-> CreateBudgetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateBudgetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateBudgetActionOutcome BudgetsClient::CreateBudgetAction(const CreateBudgetActionRequest& request) const
{
  AWS_OPERATION_GUARD(CreateBudgetAction);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateBudgetActionOutcome>(
    [&]()-> CreateBudgetActionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateBudgetActionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateNotificationOutcome BudgetsClient::CreateNotification(const CreateNotificationRequest& request) const
{
  AWS_OPERATION_GUARD(CreateNotification);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateNotificationOutcome>(
    [&]()-> CreateNotificationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateNotificationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateSubscriberOutcome BudgetsClient::CreateSubscriber(const CreateSubscriberRequest& request) const
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
      return CreateSubscriberOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteBudgetOutcome BudgetsClient::DeleteBudget(const DeleteBudgetRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteBudget);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteBudgetOutcome>(
    [&]()-> DeleteBudgetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteBudgetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteBudgetActionOutcome BudgetsClient::DeleteBudgetAction(const DeleteBudgetActionRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteBudgetAction);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteBudgetActionOutcome>(
    [&]()-> DeleteBudgetActionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteBudgetActionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteNotificationOutcome BudgetsClient::DeleteNotification(const DeleteNotificationRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteNotification);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteNotificationOutcome>(
    [&]()-> DeleteNotificationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteNotificationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteSubscriberOutcome BudgetsClient::DeleteSubscriber(const DeleteSubscriberRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteSubscriber);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteSubscriberOutcome>(
    [&]()-> DeleteSubscriberOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteSubscriberOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeBudgetOutcome BudgetsClient::DescribeBudget(const DescribeBudgetRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeBudget);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeBudgetOutcome>(
    [&]()-> DescribeBudgetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeBudgetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeBudgetActionOutcome BudgetsClient::DescribeBudgetAction(const DescribeBudgetActionRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeBudgetAction);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeBudgetActionOutcome>(
    [&]()-> DescribeBudgetActionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeBudgetActionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeBudgetActionHistoriesOutcome BudgetsClient::DescribeBudgetActionHistories(const DescribeBudgetActionHistoriesRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeBudgetActionHistories);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeBudgetActionHistories, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeBudgetActionHistoriesOutcome>(
    [&]()-> DescribeBudgetActionHistoriesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeBudgetActionHistories, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeBudgetActionHistoriesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeBudgetActionsForAccountOutcome BudgetsClient::DescribeBudgetActionsForAccount(const DescribeBudgetActionsForAccountRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeBudgetActionsForAccount);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeBudgetActionsForAccount, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeBudgetActionsForAccountOutcome>(
    [&]()-> DescribeBudgetActionsForAccountOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeBudgetActionsForAccount, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeBudgetActionsForAccountOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeBudgetActionsForBudgetOutcome BudgetsClient::DescribeBudgetActionsForBudget(const DescribeBudgetActionsForBudgetRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeBudgetActionsForBudget);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeBudgetActionsForBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeBudgetActionsForBudgetOutcome>(
    [&]()-> DescribeBudgetActionsForBudgetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeBudgetActionsForBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeBudgetActionsForBudgetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeBudgetNotificationsForAccountOutcome BudgetsClient::DescribeBudgetNotificationsForAccount(const DescribeBudgetNotificationsForAccountRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeBudgetNotificationsForAccount);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeBudgetNotificationsForAccount, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeBudgetNotificationsForAccountOutcome>(
    [&]()-> DescribeBudgetNotificationsForAccountOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeBudgetNotificationsForAccount, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeBudgetNotificationsForAccountOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeBudgetPerformanceHistoryOutcome BudgetsClient::DescribeBudgetPerformanceHistory(const DescribeBudgetPerformanceHistoryRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeBudgetPerformanceHistory);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeBudgetPerformanceHistory, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeBudgetPerformanceHistoryOutcome>(
    [&]()-> DescribeBudgetPerformanceHistoryOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeBudgetPerformanceHistory, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeBudgetPerformanceHistoryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeBudgetsOutcome BudgetsClient::DescribeBudgets(const DescribeBudgetsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeBudgets);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeBudgets, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeBudgetsOutcome>(
    [&]()-> DescribeBudgetsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeBudgets, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeBudgetsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeNotificationsForBudgetOutcome BudgetsClient::DescribeNotificationsForBudget(const DescribeNotificationsForBudgetRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeNotificationsForBudget);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeNotificationsForBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeNotificationsForBudgetOutcome>(
    [&]()-> DescribeNotificationsForBudgetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeNotificationsForBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeNotificationsForBudgetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeSubscribersForNotificationOutcome BudgetsClient::DescribeSubscribersForNotification(const DescribeSubscribersForNotificationRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeSubscribersForNotification);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeSubscribersForNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeSubscribersForNotificationOutcome>(
    [&]()-> DescribeSubscribersForNotificationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeSubscribersForNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeSubscribersForNotificationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ExecuteBudgetActionOutcome BudgetsClient::ExecuteBudgetAction(const ExecuteBudgetActionRequest& request) const
{
  AWS_OPERATION_GUARD(ExecuteBudgetAction);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ExecuteBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ExecuteBudgetActionOutcome>(
    [&]()-> ExecuteBudgetActionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ExecuteBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ExecuteBudgetActionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateBudgetOutcome BudgetsClient::UpdateBudget(const UpdateBudgetRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateBudget);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateBudgetOutcome>(
    [&]()-> UpdateBudgetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateBudget, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateBudgetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateBudgetActionOutcome BudgetsClient::UpdateBudgetAction(const UpdateBudgetActionRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateBudgetAction);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateBudgetActionOutcome>(
    [&]()-> UpdateBudgetActionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateBudgetAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateBudgetActionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateNotificationOutcome BudgetsClient::UpdateNotification(const UpdateNotificationRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateNotification);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateNotificationOutcome>(
    [&]()-> UpdateNotificationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateNotification, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateNotificationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateSubscriberOutcome BudgetsClient::UpdateSubscriber(const UpdateSubscriberRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateSubscriber);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateSubscriberOutcome>(
    [&]()-> UpdateSubscriberOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateSubscriber, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateSubscriberOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

