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

#include <aws/license-manager/LicenseManagerClient.h>
#include <aws/license-manager/LicenseManagerErrorMarshaller.h>
#include <aws/license-manager/LicenseManagerEndpointProvider.h>
#include <aws/license-manager/model/AcceptGrantRequest.h>
#include <aws/license-manager/model/CheckInLicenseRequest.h>
#include <aws/license-manager/model/CheckoutBorrowLicenseRequest.h>
#include <aws/license-manager/model/CheckoutLicenseRequest.h>
#include <aws/license-manager/model/CreateGrantRequest.h>
#include <aws/license-manager/model/CreateGrantVersionRequest.h>
#include <aws/license-manager/model/CreateLicenseRequest.h>
#include <aws/license-manager/model/CreateLicenseConfigurationRequest.h>
#include <aws/license-manager/model/CreateLicenseConversionTaskForResourceRequest.h>
#include <aws/license-manager/model/CreateLicenseManagerReportGeneratorRequest.h>
#include <aws/license-manager/model/CreateLicenseVersionRequest.h>
#include <aws/license-manager/model/CreateTokenRequest.h>
#include <aws/license-manager/model/DeleteGrantRequest.h>
#include <aws/license-manager/model/DeleteLicenseRequest.h>
#include <aws/license-manager/model/DeleteLicenseConfigurationRequest.h>
#include <aws/license-manager/model/DeleteLicenseManagerReportGeneratorRequest.h>
#include <aws/license-manager/model/DeleteTokenRequest.h>
#include <aws/license-manager/model/ExtendLicenseConsumptionRequest.h>
#include <aws/license-manager/model/GetAccessTokenRequest.h>
#include <aws/license-manager/model/GetGrantRequest.h>
#include <aws/license-manager/model/GetLicenseRequest.h>
#include <aws/license-manager/model/GetLicenseConfigurationRequest.h>
#include <aws/license-manager/model/GetLicenseConversionTaskRequest.h>
#include <aws/license-manager/model/GetLicenseManagerReportGeneratorRequest.h>
#include <aws/license-manager/model/GetLicenseUsageRequest.h>
#include <aws/license-manager/model/GetServiceSettingsRequest.h>
#include <aws/license-manager/model/ListAssociationsForLicenseConfigurationRequest.h>
#include <aws/license-manager/model/ListDistributedGrantsRequest.h>
#include <aws/license-manager/model/ListFailuresForLicenseConfigurationOperationsRequest.h>
#include <aws/license-manager/model/ListLicenseConfigurationsRequest.h>
#include <aws/license-manager/model/ListLicenseConversionTasksRequest.h>
#include <aws/license-manager/model/ListLicenseManagerReportGeneratorsRequest.h>
#include <aws/license-manager/model/ListLicenseSpecificationsForResourceRequest.h>
#include <aws/license-manager/model/ListLicenseVersionsRequest.h>
#include <aws/license-manager/model/ListLicensesRequest.h>
#include <aws/license-manager/model/ListReceivedGrantsRequest.h>
#include <aws/license-manager/model/ListReceivedGrantsForOrganizationRequest.h>
#include <aws/license-manager/model/ListReceivedLicensesRequest.h>
#include <aws/license-manager/model/ListReceivedLicensesForOrganizationRequest.h>
#include <aws/license-manager/model/ListResourceInventoryRequest.h>
#include <aws/license-manager/model/ListTagsForResourceRequest.h>
#include <aws/license-manager/model/ListTokensRequest.h>
#include <aws/license-manager/model/ListUsageForLicenseConfigurationRequest.h>
#include <aws/license-manager/model/RejectGrantRequest.h>
#include <aws/license-manager/model/TagResourceRequest.h>
#include <aws/license-manager/model/UntagResourceRequest.h>
#include <aws/license-manager/model/UpdateLicenseConfigurationRequest.h>
#include <aws/license-manager/model/UpdateLicenseManagerReportGeneratorRequest.h>
#include <aws/license-manager/model/UpdateLicenseSpecificationsForResourceRequest.h>
#include <aws/license-manager/model/UpdateServiceSettingsRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::LicenseManager;
using namespace Aws::LicenseManager::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* LicenseManagerClient::SERVICE_NAME = "license-manager";
const char* LicenseManagerClient::ALLOCATION_TAG = "LicenseManagerClient";

LicenseManagerClient::LicenseManagerClient(const LicenseManager::LicenseManagerClientConfiguration& clientConfiguration,
                                           std::shared_ptr<LicenseManagerEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<LicenseManagerErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

LicenseManagerClient::LicenseManagerClient(const AWSCredentials& credentials,
                                           std::shared_ptr<LicenseManagerEndpointProviderBase> endpointProvider,
                                           const LicenseManager::LicenseManagerClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<LicenseManagerErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

LicenseManagerClient::LicenseManagerClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                           std::shared_ptr<LicenseManagerEndpointProviderBase> endpointProvider,
                                           const LicenseManager::LicenseManagerClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<LicenseManagerErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  LicenseManagerClient::LicenseManagerClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<LicenseManagerErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<LicenseManagerEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

LicenseManagerClient::LicenseManagerClient(const AWSCredentials& credentials,
                                           const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<LicenseManagerErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<LicenseManagerEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

LicenseManagerClient::LicenseManagerClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                           const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<LicenseManagerErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<LicenseManagerEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
LicenseManagerClient::~LicenseManagerClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<LicenseManagerEndpointProviderBase>& LicenseManagerClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void LicenseManagerClient::init(const LicenseManager::LicenseManagerClientConfiguration& config)
{
  AWSClient::SetServiceClientName("License Manager");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void LicenseManagerClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

AcceptGrantOutcome LicenseManagerClient::AcceptGrant(const AcceptGrantRequest& request) const
{
  AWS_OPERATION_GUARD(AcceptGrant);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AcceptGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AcceptGrantOutcome>(
    [&]()-> AcceptGrantOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AcceptGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AcceptGrantOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CheckInLicenseOutcome LicenseManagerClient::CheckInLicense(const CheckInLicenseRequest& request) const
{
  AWS_OPERATION_GUARD(CheckInLicense);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CheckInLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CheckInLicenseOutcome>(
    [&]()-> CheckInLicenseOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CheckInLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CheckInLicenseOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CheckoutBorrowLicenseOutcome LicenseManagerClient::CheckoutBorrowLicense(const CheckoutBorrowLicenseRequest& request) const
{
  AWS_OPERATION_GUARD(CheckoutBorrowLicense);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CheckoutBorrowLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CheckoutBorrowLicenseOutcome>(
    [&]()-> CheckoutBorrowLicenseOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CheckoutBorrowLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CheckoutBorrowLicenseOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CheckoutLicenseOutcome LicenseManagerClient::CheckoutLicense(const CheckoutLicenseRequest& request) const
{
  AWS_OPERATION_GUARD(CheckoutLicense);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CheckoutLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CheckoutLicenseOutcome>(
    [&]()-> CheckoutLicenseOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CheckoutLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CheckoutLicenseOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateGrantOutcome LicenseManagerClient::CreateGrant(const CreateGrantRequest& request) const
{
  AWS_OPERATION_GUARD(CreateGrant);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateGrantOutcome>(
    [&]()-> CreateGrantOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateGrantOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateGrantVersionOutcome LicenseManagerClient::CreateGrantVersion(const CreateGrantVersionRequest& request) const
{
  AWS_OPERATION_GUARD(CreateGrantVersion);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateGrantVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateGrantVersionOutcome>(
    [&]()-> CreateGrantVersionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateGrantVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateGrantVersionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateLicenseOutcome LicenseManagerClient::CreateLicense(const CreateLicenseRequest& request) const
{
  AWS_OPERATION_GUARD(CreateLicense);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateLicenseOutcome>(
    [&]()-> CreateLicenseOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateLicenseOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateLicenseConfigurationOutcome LicenseManagerClient::CreateLicenseConfiguration(const CreateLicenseConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(CreateLicenseConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateLicenseConfigurationOutcome>(
    [&]()-> CreateLicenseConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateLicenseConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateLicenseConversionTaskForResourceOutcome LicenseManagerClient::CreateLicenseConversionTaskForResource(const CreateLicenseConversionTaskForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(CreateLicenseConversionTaskForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateLicenseConversionTaskForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateLicenseConversionTaskForResourceOutcome>(
    [&]()-> CreateLicenseConversionTaskForResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateLicenseConversionTaskForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateLicenseConversionTaskForResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateLicenseManagerReportGeneratorOutcome LicenseManagerClient::CreateLicenseManagerReportGenerator(const CreateLicenseManagerReportGeneratorRequest& request) const
{
  AWS_OPERATION_GUARD(CreateLicenseManagerReportGenerator);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateLicenseManagerReportGenerator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateLicenseManagerReportGeneratorOutcome>(
    [&]()-> CreateLicenseManagerReportGeneratorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateLicenseManagerReportGenerator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateLicenseManagerReportGeneratorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateLicenseVersionOutcome LicenseManagerClient::CreateLicenseVersion(const CreateLicenseVersionRequest& request) const
{
  AWS_OPERATION_GUARD(CreateLicenseVersion);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateLicenseVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateLicenseVersionOutcome>(
    [&]()-> CreateLicenseVersionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateLicenseVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateLicenseVersionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateTokenOutcome LicenseManagerClient::CreateToken(const CreateTokenRequest& request) const
{
  AWS_OPERATION_GUARD(CreateToken);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateToken, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateTokenOutcome>(
    [&]()-> CreateTokenOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateToken, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateTokenOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteGrantOutcome LicenseManagerClient::DeleteGrant(const DeleteGrantRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteGrant);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteGrantOutcome>(
    [&]()-> DeleteGrantOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteGrantOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteLicenseOutcome LicenseManagerClient::DeleteLicense(const DeleteLicenseRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteLicense);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteLicenseOutcome>(
    [&]()-> DeleteLicenseOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteLicenseOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteLicenseConfigurationOutcome LicenseManagerClient::DeleteLicenseConfiguration(const DeleteLicenseConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteLicenseConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteLicenseConfigurationOutcome>(
    [&]()-> DeleteLicenseConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteLicenseConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteLicenseManagerReportGeneratorOutcome LicenseManagerClient::DeleteLicenseManagerReportGenerator(const DeleteLicenseManagerReportGeneratorRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteLicenseManagerReportGenerator);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteLicenseManagerReportGenerator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteLicenseManagerReportGeneratorOutcome>(
    [&]()-> DeleteLicenseManagerReportGeneratorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteLicenseManagerReportGenerator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteLicenseManagerReportGeneratorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteTokenOutcome LicenseManagerClient::DeleteToken(const DeleteTokenRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteToken);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteToken, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteTokenOutcome>(
    [&]()-> DeleteTokenOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteToken, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteTokenOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ExtendLicenseConsumptionOutcome LicenseManagerClient::ExtendLicenseConsumption(const ExtendLicenseConsumptionRequest& request) const
{
  AWS_OPERATION_GUARD(ExtendLicenseConsumption);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ExtendLicenseConsumption, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ExtendLicenseConsumptionOutcome>(
    [&]()-> ExtendLicenseConsumptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ExtendLicenseConsumption, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ExtendLicenseConsumptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetAccessTokenOutcome LicenseManagerClient::GetAccessToken(const GetAccessTokenRequest& request) const
{
  AWS_OPERATION_GUARD(GetAccessToken);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetAccessToken, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetAccessTokenOutcome>(
    [&]()-> GetAccessTokenOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetAccessToken, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetAccessTokenOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetGrantOutcome LicenseManagerClient::GetGrant(const GetGrantRequest& request) const
{
  AWS_OPERATION_GUARD(GetGrant);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetGrantOutcome>(
    [&]()-> GetGrantOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetGrantOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLicenseOutcome LicenseManagerClient::GetLicense(const GetLicenseRequest& request) const
{
  AWS_OPERATION_GUARD(GetLicense);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLicenseOutcome>(
    [&]()-> GetLicenseOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLicense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetLicenseOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLicenseConfigurationOutcome LicenseManagerClient::GetLicenseConfiguration(const GetLicenseConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(GetLicenseConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLicenseConfigurationOutcome>(
    [&]()-> GetLicenseConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetLicenseConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLicenseConversionTaskOutcome LicenseManagerClient::GetLicenseConversionTask(const GetLicenseConversionTaskRequest& request) const
{
  AWS_OPERATION_GUARD(GetLicenseConversionTask);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLicenseConversionTask, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLicenseConversionTaskOutcome>(
    [&]()-> GetLicenseConversionTaskOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLicenseConversionTask, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetLicenseConversionTaskOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLicenseManagerReportGeneratorOutcome LicenseManagerClient::GetLicenseManagerReportGenerator(const GetLicenseManagerReportGeneratorRequest& request) const
{
  AWS_OPERATION_GUARD(GetLicenseManagerReportGenerator);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLicenseManagerReportGenerator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLicenseManagerReportGeneratorOutcome>(
    [&]()-> GetLicenseManagerReportGeneratorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLicenseManagerReportGenerator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetLicenseManagerReportGeneratorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLicenseUsageOutcome LicenseManagerClient::GetLicenseUsage(const GetLicenseUsageRequest& request) const
{
  AWS_OPERATION_GUARD(GetLicenseUsage);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLicenseUsage, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLicenseUsageOutcome>(
    [&]()-> GetLicenseUsageOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLicenseUsage, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetLicenseUsageOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetServiceSettingsOutcome LicenseManagerClient::GetServiceSettings(const GetServiceSettingsRequest& request) const
{
  AWS_OPERATION_GUARD(GetServiceSettings);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetServiceSettings, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetServiceSettingsOutcome>(
    [&]()-> GetServiceSettingsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetServiceSettings, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetServiceSettingsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListAssociationsForLicenseConfigurationOutcome LicenseManagerClient::ListAssociationsForLicenseConfiguration(const ListAssociationsForLicenseConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(ListAssociationsForLicenseConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListAssociationsForLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListAssociationsForLicenseConfigurationOutcome>(
    [&]()-> ListAssociationsForLicenseConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListAssociationsForLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListAssociationsForLicenseConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListDistributedGrantsOutcome LicenseManagerClient::ListDistributedGrants(const ListDistributedGrantsRequest& request) const
{
  AWS_OPERATION_GUARD(ListDistributedGrants);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListDistributedGrants, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListDistributedGrantsOutcome>(
    [&]()-> ListDistributedGrantsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListDistributedGrants, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListDistributedGrantsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListFailuresForLicenseConfigurationOperationsOutcome LicenseManagerClient::ListFailuresForLicenseConfigurationOperations(const ListFailuresForLicenseConfigurationOperationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListFailuresForLicenseConfigurationOperations);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListFailuresForLicenseConfigurationOperations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListFailuresForLicenseConfigurationOperationsOutcome>(
    [&]()-> ListFailuresForLicenseConfigurationOperationsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListFailuresForLicenseConfigurationOperations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListFailuresForLicenseConfigurationOperationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLicenseConfigurationsOutcome LicenseManagerClient::ListLicenseConfigurations(const ListLicenseConfigurationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListLicenseConfigurations);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLicenseConfigurations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLicenseConfigurationsOutcome>(
    [&]()-> ListLicenseConfigurationsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLicenseConfigurations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListLicenseConfigurationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLicenseConversionTasksOutcome LicenseManagerClient::ListLicenseConversionTasks(const ListLicenseConversionTasksRequest& request) const
{
  AWS_OPERATION_GUARD(ListLicenseConversionTasks);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLicenseConversionTasks, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLicenseConversionTasksOutcome>(
    [&]()-> ListLicenseConversionTasksOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLicenseConversionTasks, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListLicenseConversionTasksOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLicenseManagerReportGeneratorsOutcome LicenseManagerClient::ListLicenseManagerReportGenerators(const ListLicenseManagerReportGeneratorsRequest& request) const
{
  AWS_OPERATION_GUARD(ListLicenseManagerReportGenerators);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLicenseManagerReportGenerators, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLicenseManagerReportGeneratorsOutcome>(
    [&]()-> ListLicenseManagerReportGeneratorsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLicenseManagerReportGenerators, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListLicenseManagerReportGeneratorsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLicenseSpecificationsForResourceOutcome LicenseManagerClient::ListLicenseSpecificationsForResource(const ListLicenseSpecificationsForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(ListLicenseSpecificationsForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLicenseSpecificationsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLicenseSpecificationsForResourceOutcome>(
    [&]()-> ListLicenseSpecificationsForResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLicenseSpecificationsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListLicenseSpecificationsForResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLicenseVersionsOutcome LicenseManagerClient::ListLicenseVersions(const ListLicenseVersionsRequest& request) const
{
  AWS_OPERATION_GUARD(ListLicenseVersions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLicenseVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLicenseVersionsOutcome>(
    [&]()-> ListLicenseVersionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLicenseVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListLicenseVersionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLicensesOutcome LicenseManagerClient::ListLicenses(const ListLicensesRequest& request) const
{
  AWS_OPERATION_GUARD(ListLicenses);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLicenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLicensesOutcome>(
    [&]()-> ListLicensesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLicenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListLicensesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListReceivedGrantsOutcome LicenseManagerClient::ListReceivedGrants(const ListReceivedGrantsRequest& request) const
{
  AWS_OPERATION_GUARD(ListReceivedGrants);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListReceivedGrants, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListReceivedGrantsOutcome>(
    [&]()-> ListReceivedGrantsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListReceivedGrants, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListReceivedGrantsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListReceivedGrantsForOrganizationOutcome LicenseManagerClient::ListReceivedGrantsForOrganization(const ListReceivedGrantsForOrganizationRequest& request) const
{
  AWS_OPERATION_GUARD(ListReceivedGrantsForOrganization);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListReceivedGrantsForOrganization, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListReceivedGrantsForOrganizationOutcome>(
    [&]()-> ListReceivedGrantsForOrganizationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListReceivedGrantsForOrganization, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListReceivedGrantsForOrganizationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListReceivedLicensesOutcome LicenseManagerClient::ListReceivedLicenses(const ListReceivedLicensesRequest& request) const
{
  AWS_OPERATION_GUARD(ListReceivedLicenses);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListReceivedLicenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListReceivedLicensesOutcome>(
    [&]()-> ListReceivedLicensesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListReceivedLicenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListReceivedLicensesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListReceivedLicensesForOrganizationOutcome LicenseManagerClient::ListReceivedLicensesForOrganization(const ListReceivedLicensesForOrganizationRequest& request) const
{
  AWS_OPERATION_GUARD(ListReceivedLicensesForOrganization);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListReceivedLicensesForOrganization, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListReceivedLicensesForOrganizationOutcome>(
    [&]()-> ListReceivedLicensesForOrganizationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListReceivedLicensesForOrganization, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListReceivedLicensesForOrganizationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListResourceInventoryOutcome LicenseManagerClient::ListResourceInventory(const ListResourceInventoryRequest& request) const
{
  AWS_OPERATION_GUARD(ListResourceInventory);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListResourceInventory, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListResourceInventoryOutcome>(
    [&]()-> ListResourceInventoryOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListResourceInventory, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListResourceInventoryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListTagsForResourceOutcome LicenseManagerClient::ListTagsForResource(const ListTagsForResourceRequest& request) const
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

ListTokensOutcome LicenseManagerClient::ListTokens(const ListTokensRequest& request) const
{
  AWS_OPERATION_GUARD(ListTokens);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListTokens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListTokensOutcome>(
    [&]()-> ListTokensOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListTokens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListTokensOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListUsageForLicenseConfigurationOutcome LicenseManagerClient::ListUsageForLicenseConfiguration(const ListUsageForLicenseConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(ListUsageForLicenseConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListUsageForLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListUsageForLicenseConfigurationOutcome>(
    [&]()-> ListUsageForLicenseConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListUsageForLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListUsageForLicenseConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RejectGrantOutcome LicenseManagerClient::RejectGrant(const RejectGrantRequest& request) const
{
  AWS_OPERATION_GUARD(RejectGrant);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RejectGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RejectGrantOutcome>(
    [&]()-> RejectGrantOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RejectGrant, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RejectGrantOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

TagResourceOutcome LicenseManagerClient::TagResource(const TagResourceRequest& request) const
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

UntagResourceOutcome LicenseManagerClient::UntagResource(const UntagResourceRequest& request) const
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

UpdateLicenseConfigurationOutcome LicenseManagerClient::UpdateLicenseConfiguration(const UpdateLicenseConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateLicenseConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateLicenseConfigurationOutcome>(
    [&]()-> UpdateLicenseConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateLicenseConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateLicenseConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateLicenseManagerReportGeneratorOutcome LicenseManagerClient::UpdateLicenseManagerReportGenerator(const UpdateLicenseManagerReportGeneratorRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateLicenseManagerReportGenerator);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateLicenseManagerReportGenerator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateLicenseManagerReportGeneratorOutcome>(
    [&]()-> UpdateLicenseManagerReportGeneratorOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateLicenseManagerReportGenerator, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateLicenseManagerReportGeneratorOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateLicenseSpecificationsForResourceOutcome LicenseManagerClient::UpdateLicenseSpecificationsForResource(const UpdateLicenseSpecificationsForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateLicenseSpecificationsForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateLicenseSpecificationsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateLicenseSpecificationsForResourceOutcome>(
    [&]()-> UpdateLicenseSpecificationsForResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateLicenseSpecificationsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateLicenseSpecificationsForResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateServiceSettingsOutcome LicenseManagerClient::UpdateServiceSettings(const UpdateServiceSettingsRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateServiceSettings);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateServiceSettings, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateServiceSettingsOutcome>(
    [&]()-> UpdateServiceSettingsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateServiceSettings, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateServiceSettingsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

