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

#include <aws/customer-profiles/CustomerProfilesClient.h>
#include <aws/customer-profiles/CustomerProfilesErrorMarshaller.h>
#include <aws/customer-profiles/CustomerProfilesEndpointProvider.h>
#include <aws/customer-profiles/model/AddProfileKeyRequest.h>
#include <aws/customer-profiles/model/CreateCalculatedAttributeDefinitionRequest.h>
#include <aws/customer-profiles/model/CreateDomainRequest.h>
#include <aws/customer-profiles/model/CreateEventStreamRequest.h>
#include <aws/customer-profiles/model/CreateIntegrationWorkflowRequest.h>
#include <aws/customer-profiles/model/CreateProfileRequest.h>
#include <aws/customer-profiles/model/DeleteCalculatedAttributeDefinitionRequest.h>
#include <aws/customer-profiles/model/DeleteDomainRequest.h>
#include <aws/customer-profiles/model/DeleteEventStreamRequest.h>
#include <aws/customer-profiles/model/DeleteIntegrationRequest.h>
#include <aws/customer-profiles/model/DeleteProfileRequest.h>
#include <aws/customer-profiles/model/DeleteProfileKeyRequest.h>
#include <aws/customer-profiles/model/DeleteProfileObjectRequest.h>
#include <aws/customer-profiles/model/DeleteProfileObjectTypeRequest.h>
#include <aws/customer-profiles/model/DeleteWorkflowRequest.h>
#include <aws/customer-profiles/model/GetAutoMergingPreviewRequest.h>
#include <aws/customer-profiles/model/GetCalculatedAttributeDefinitionRequest.h>
#include <aws/customer-profiles/model/GetCalculatedAttributeForProfileRequest.h>
#include <aws/customer-profiles/model/GetDomainRequest.h>
#include <aws/customer-profiles/model/GetEventStreamRequest.h>
#include <aws/customer-profiles/model/GetIdentityResolutionJobRequest.h>
#include <aws/customer-profiles/model/GetIntegrationRequest.h>
#include <aws/customer-profiles/model/GetMatchesRequest.h>
#include <aws/customer-profiles/model/GetProfileObjectTypeRequest.h>
#include <aws/customer-profiles/model/GetProfileObjectTypeTemplateRequest.h>
#include <aws/customer-profiles/model/GetWorkflowRequest.h>
#include <aws/customer-profiles/model/GetWorkflowStepsRequest.h>
#include <aws/customer-profiles/model/ListAccountIntegrationsRequest.h>
#include <aws/customer-profiles/model/ListCalculatedAttributeDefinitionsRequest.h>
#include <aws/customer-profiles/model/ListCalculatedAttributesForProfileRequest.h>
#include <aws/customer-profiles/model/ListDomainsRequest.h>
#include <aws/customer-profiles/model/ListEventStreamsRequest.h>
#include <aws/customer-profiles/model/ListIdentityResolutionJobsRequest.h>
#include <aws/customer-profiles/model/ListIntegrationsRequest.h>
#include <aws/customer-profiles/model/ListProfileObjectTypeTemplatesRequest.h>
#include <aws/customer-profiles/model/ListProfileObjectTypesRequest.h>
#include <aws/customer-profiles/model/ListProfileObjectsRequest.h>
#include <aws/customer-profiles/model/ListTagsForResourceRequest.h>
#include <aws/customer-profiles/model/ListWorkflowsRequest.h>
#include <aws/customer-profiles/model/MergeProfilesRequest.h>
#include <aws/customer-profiles/model/PutIntegrationRequest.h>
#include <aws/customer-profiles/model/PutProfileObjectRequest.h>
#include <aws/customer-profiles/model/PutProfileObjectTypeRequest.h>
#include <aws/customer-profiles/model/SearchProfilesRequest.h>
#include <aws/customer-profiles/model/TagResourceRequest.h>
#include <aws/customer-profiles/model/UntagResourceRequest.h>
#include <aws/customer-profiles/model/UpdateCalculatedAttributeDefinitionRequest.h>
#include <aws/customer-profiles/model/UpdateDomainRequest.h>
#include <aws/customer-profiles/model/UpdateProfileRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::CustomerProfiles;
using namespace Aws::CustomerProfiles::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* CustomerProfilesClient::SERVICE_NAME = "profile";
const char* CustomerProfilesClient::ALLOCATION_TAG = "CustomerProfilesClient";

CustomerProfilesClient::CustomerProfilesClient(const CustomerProfiles::CustomerProfilesClientConfiguration& clientConfiguration,
                                               std::shared_ptr<CustomerProfilesEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<CustomerProfilesErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

CustomerProfilesClient::CustomerProfilesClient(const AWSCredentials& credentials,
                                               std::shared_ptr<CustomerProfilesEndpointProviderBase> endpointProvider,
                                               const CustomerProfiles::CustomerProfilesClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<CustomerProfilesErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

CustomerProfilesClient::CustomerProfilesClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                               std::shared_ptr<CustomerProfilesEndpointProviderBase> endpointProvider,
                                               const CustomerProfiles::CustomerProfilesClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<CustomerProfilesErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  CustomerProfilesClient::CustomerProfilesClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<CustomerProfilesErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<CustomerProfilesEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

CustomerProfilesClient::CustomerProfilesClient(const AWSCredentials& credentials,
                                               const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<CustomerProfilesErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<CustomerProfilesEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

CustomerProfilesClient::CustomerProfilesClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                               const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<CustomerProfilesErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<CustomerProfilesEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
CustomerProfilesClient::~CustomerProfilesClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<CustomerProfilesEndpointProviderBase>& CustomerProfilesClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void CustomerProfilesClient::init(const CustomerProfiles::CustomerProfilesClientConfiguration& config)
{
  AWSClient::SetServiceClientName("Customer Profiles");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void CustomerProfilesClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

AddProfileKeyOutcome CustomerProfilesClient::AddProfileKey(const AddProfileKeyRequest& request) const
{
  AWS_OPERATION_GUARD(AddProfileKey);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AddProfileKey, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("AddProfileKey", "Required field: DomainName, is not set");
    return AddProfileKeyOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AddProfileKeyOutcome>(
    [&]()-> AddProfileKeyOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AddProfileKey, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/keys");
      return AddProfileKeyOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateCalculatedAttributeDefinitionOutcome CustomerProfilesClient::CreateCalculatedAttributeDefinition(const CreateCalculatedAttributeDefinitionRequest& request) const
{
  AWS_OPERATION_GUARD(CreateCalculatedAttributeDefinition);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateCalculatedAttributeDefinition, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateCalculatedAttributeDefinition", "Required field: DomainName, is not set");
    return CreateCalculatedAttributeDefinitionOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.CalculatedAttributeNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateCalculatedAttributeDefinition", "Required field: CalculatedAttributeName, is not set");
    return CreateCalculatedAttributeDefinitionOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [CalculatedAttributeName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateCalculatedAttributeDefinitionOutcome>(
    [&]()-> CreateCalculatedAttributeDefinitionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateCalculatedAttributeDefinition, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/calculated-attributes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetCalculatedAttributeName());
      return CreateCalculatedAttributeDefinitionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDomainOutcome CustomerProfilesClient::CreateDomain(const CreateDomainRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDomain);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateDomain", "Required field: DomainName, is not set");
    return CreateDomainOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDomainOutcome>(
    [&]()-> CreateDomainOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      return CreateDomainOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateEventStreamOutcome CustomerProfilesClient::CreateEventStream(const CreateEventStreamRequest& request) const
{
  AWS_OPERATION_GUARD(CreateEventStream);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateEventStream, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateEventStream", "Required field: DomainName, is not set");
    return CreateEventStreamOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.EventStreamNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateEventStream", "Required field: EventStreamName, is not set");
    return CreateEventStreamOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [EventStreamName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateEventStreamOutcome>(
    [&]()-> CreateEventStreamOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateEventStream, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/event-streams/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetEventStreamName());
      return CreateEventStreamOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateIntegrationWorkflowOutcome CustomerProfilesClient::CreateIntegrationWorkflow(const CreateIntegrationWorkflowRequest& request) const
{
  AWS_OPERATION_GUARD(CreateIntegrationWorkflow);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateIntegrationWorkflow, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateIntegrationWorkflow", "Required field: DomainName, is not set");
    return CreateIntegrationWorkflowOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateIntegrationWorkflowOutcome>(
    [&]()-> CreateIntegrationWorkflowOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateIntegrationWorkflow, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workflows/integrations");
      return CreateIntegrationWorkflowOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateProfileOutcome CustomerProfilesClient::CreateProfile(const CreateProfileRequest& request) const
{
  AWS_OPERATION_GUARD(CreateProfile);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateProfile", "Required field: DomainName, is not set");
    return CreateProfileOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateProfileOutcome>(
    [&]()-> CreateProfileOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles");
      return CreateProfileOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteCalculatedAttributeDefinitionOutcome CustomerProfilesClient::DeleteCalculatedAttributeDefinition(const DeleteCalculatedAttributeDefinitionRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteCalculatedAttributeDefinition);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteCalculatedAttributeDefinition, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteCalculatedAttributeDefinition", "Required field: DomainName, is not set");
    return DeleteCalculatedAttributeDefinitionOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.CalculatedAttributeNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteCalculatedAttributeDefinition", "Required field: CalculatedAttributeName, is not set");
    return DeleteCalculatedAttributeDefinitionOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [CalculatedAttributeName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteCalculatedAttributeDefinitionOutcome>(
    [&]()-> DeleteCalculatedAttributeDefinitionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteCalculatedAttributeDefinition, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/calculated-attributes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetCalculatedAttributeName());
      return DeleteCalculatedAttributeDefinitionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDomainOutcome CustomerProfilesClient::DeleteDomain(const DeleteDomainRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDomain);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteDomain", "Required field: DomainName, is not set");
    return DeleteDomainOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDomainOutcome>(
    [&]()-> DeleteDomainOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      return DeleteDomainOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteEventStreamOutcome CustomerProfilesClient::DeleteEventStream(const DeleteEventStreamRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteEventStream);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteEventStream, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteEventStream", "Required field: DomainName, is not set");
    return DeleteEventStreamOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.EventStreamNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteEventStream", "Required field: EventStreamName, is not set");
    return DeleteEventStreamOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [EventStreamName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteEventStreamOutcome>(
    [&]()-> DeleteEventStreamOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteEventStream, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/event-streams/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetEventStreamName());
      return DeleteEventStreamOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteIntegrationOutcome CustomerProfilesClient::DeleteIntegration(const DeleteIntegrationRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteIntegration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteIntegration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteIntegration", "Required field: DomainName, is not set");
    return DeleteIntegrationOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteIntegrationOutcome>(
    [&]()-> DeleteIntegrationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteIntegration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/integrations/delete");
      return DeleteIntegrationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteProfileOutcome CustomerProfilesClient::DeleteProfile(const DeleteProfileRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteProfile);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfile", "Required field: DomainName, is not set");
    return DeleteProfileOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteProfileOutcome>(
    [&]()-> DeleteProfileOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/delete");
      return DeleteProfileOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteProfileKeyOutcome CustomerProfilesClient::DeleteProfileKey(const DeleteProfileKeyRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteProfileKey);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteProfileKey, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfileKey", "Required field: DomainName, is not set");
    return DeleteProfileKeyOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteProfileKeyOutcome>(
    [&]()-> DeleteProfileKeyOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteProfileKey, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/keys/delete");
      return DeleteProfileKeyOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteProfileObjectOutcome CustomerProfilesClient::DeleteProfileObject(const DeleteProfileObjectRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteProfileObject);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteProfileObject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfileObject", "Required field: DomainName, is not set");
    return DeleteProfileObjectOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteProfileObjectOutcome>(
    [&]()-> DeleteProfileObjectOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteProfileObject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/objects/delete");
      return DeleteProfileObjectOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteProfileObjectTypeOutcome CustomerProfilesClient::DeleteProfileObjectType(const DeleteProfileObjectTypeRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteProfileObjectType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteProfileObjectType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfileObjectType", "Required field: DomainName, is not set");
    return DeleteProfileObjectTypeOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.ObjectTypeNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfileObjectType", "Required field: ObjectTypeName, is not set");
    return DeleteProfileObjectTypeOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ObjectTypeName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteProfileObjectTypeOutcome>(
    [&]()-> DeleteProfileObjectTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteProfileObjectType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/object-types/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetObjectTypeName());
      return DeleteProfileObjectTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteWorkflowOutcome CustomerProfilesClient::DeleteWorkflow(const DeleteWorkflowRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteWorkflow);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteWorkflow, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteWorkflow", "Required field: DomainName, is not set");
    return DeleteWorkflowOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.WorkflowIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteWorkflow", "Required field: WorkflowId, is not set");
    return DeleteWorkflowOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkflowId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteWorkflowOutcome>(
    [&]()-> DeleteWorkflowOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteWorkflow, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workflows/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkflowId());
      return DeleteWorkflowOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetAutoMergingPreviewOutcome CustomerProfilesClient::GetAutoMergingPreview(const GetAutoMergingPreviewRequest& request) const
{
  AWS_OPERATION_GUARD(GetAutoMergingPreview);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetAutoMergingPreview, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetAutoMergingPreview", "Required field: DomainName, is not set");
    return GetAutoMergingPreviewOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetAutoMergingPreviewOutcome>(
    [&]()-> GetAutoMergingPreviewOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetAutoMergingPreview, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/identity-resolution-jobs/auto-merging-preview");
      return GetAutoMergingPreviewOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetCalculatedAttributeDefinitionOutcome CustomerProfilesClient::GetCalculatedAttributeDefinition(const GetCalculatedAttributeDefinitionRequest& request) const
{
  AWS_OPERATION_GUARD(GetCalculatedAttributeDefinition);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetCalculatedAttributeDefinition, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetCalculatedAttributeDefinition", "Required field: DomainName, is not set");
    return GetCalculatedAttributeDefinitionOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.CalculatedAttributeNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetCalculatedAttributeDefinition", "Required field: CalculatedAttributeName, is not set");
    return GetCalculatedAttributeDefinitionOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [CalculatedAttributeName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetCalculatedAttributeDefinitionOutcome>(
    [&]()-> GetCalculatedAttributeDefinitionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetCalculatedAttributeDefinition, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/calculated-attributes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetCalculatedAttributeName());
      return GetCalculatedAttributeDefinitionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetCalculatedAttributeForProfileOutcome CustomerProfilesClient::GetCalculatedAttributeForProfile(const GetCalculatedAttributeForProfileRequest& request) const
{
  AWS_OPERATION_GUARD(GetCalculatedAttributeForProfile);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetCalculatedAttributeForProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetCalculatedAttributeForProfile", "Required field: DomainName, is not set");
    return GetCalculatedAttributeForProfileOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.ProfileIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetCalculatedAttributeForProfile", "Required field: ProfileId, is not set");
    return GetCalculatedAttributeForProfileOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ProfileId]", false));
  }
  if (!request.CalculatedAttributeNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetCalculatedAttributeForProfile", "Required field: CalculatedAttributeName, is not set");
    return GetCalculatedAttributeForProfileOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [CalculatedAttributeName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetCalculatedAttributeForProfileOutcome>(
    [&]()-> GetCalculatedAttributeForProfileOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetCalculatedAttributeForProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profile/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetProfileId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/calculated-attributes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetCalculatedAttributeName());
      return GetCalculatedAttributeForProfileOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetDomainOutcome CustomerProfilesClient::GetDomain(const GetDomainRequest& request) const
{
  AWS_OPERATION_GUARD(GetDomain);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetDomain", "Required field: DomainName, is not set");
    return GetDomainOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetDomainOutcome>(
    [&]()-> GetDomainOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      return GetDomainOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetEventStreamOutcome CustomerProfilesClient::GetEventStream(const GetEventStreamRequest& request) const
{
  AWS_OPERATION_GUARD(GetEventStream);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetEventStream, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetEventStream", "Required field: DomainName, is not set");
    return GetEventStreamOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.EventStreamNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetEventStream", "Required field: EventStreamName, is not set");
    return GetEventStreamOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [EventStreamName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetEventStreamOutcome>(
    [&]()-> GetEventStreamOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetEventStream, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/event-streams/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetEventStreamName());
      return GetEventStreamOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetIdentityResolutionJobOutcome CustomerProfilesClient::GetIdentityResolutionJob(const GetIdentityResolutionJobRequest& request) const
{
  AWS_OPERATION_GUARD(GetIdentityResolutionJob);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetIdentityResolutionJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetIdentityResolutionJob", "Required field: DomainName, is not set");
    return GetIdentityResolutionJobOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.JobIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetIdentityResolutionJob", "Required field: JobId, is not set");
    return GetIdentityResolutionJobOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [JobId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetIdentityResolutionJobOutcome>(
    [&]()-> GetIdentityResolutionJobOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetIdentityResolutionJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/identity-resolution-jobs/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetJobId());
      return GetIdentityResolutionJobOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetIntegrationOutcome CustomerProfilesClient::GetIntegration(const GetIntegrationRequest& request) const
{
  AWS_OPERATION_GUARD(GetIntegration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetIntegration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetIntegration", "Required field: DomainName, is not set");
    return GetIntegrationOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetIntegrationOutcome>(
    [&]()-> GetIntegrationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetIntegration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/integrations");
      return GetIntegrationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetMatchesOutcome CustomerProfilesClient::GetMatches(const GetMatchesRequest& request) const
{
  AWS_OPERATION_GUARD(GetMatches);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetMatches, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetMatches", "Required field: DomainName, is not set");
    return GetMatchesOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetMatchesOutcome>(
    [&]()-> GetMatchesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetMatches, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/matches");
      return GetMatchesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetProfileObjectTypeOutcome CustomerProfilesClient::GetProfileObjectType(const GetProfileObjectTypeRequest& request) const
{
  AWS_OPERATION_GUARD(GetProfileObjectType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetProfileObjectType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetProfileObjectType", "Required field: DomainName, is not set");
    return GetProfileObjectTypeOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.ObjectTypeNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetProfileObjectType", "Required field: ObjectTypeName, is not set");
    return GetProfileObjectTypeOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ObjectTypeName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetProfileObjectTypeOutcome>(
    [&]()-> GetProfileObjectTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetProfileObjectType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/object-types/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetObjectTypeName());
      return GetProfileObjectTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetProfileObjectTypeTemplateOutcome CustomerProfilesClient::GetProfileObjectTypeTemplate(const GetProfileObjectTypeTemplateRequest& request) const
{
  AWS_OPERATION_GUARD(GetProfileObjectTypeTemplate);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetProfileObjectTypeTemplate, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.TemplateIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetProfileObjectTypeTemplate", "Required field: TemplateId, is not set");
    return GetProfileObjectTypeTemplateOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [TemplateId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetProfileObjectTypeTemplateOutcome>(
    [&]()-> GetProfileObjectTypeTemplateOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetProfileObjectTypeTemplate, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/templates/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetTemplateId());
      return GetProfileObjectTypeTemplateOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetWorkflowOutcome CustomerProfilesClient::GetWorkflow(const GetWorkflowRequest& request) const
{
  AWS_OPERATION_GUARD(GetWorkflow);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetWorkflow, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetWorkflow", "Required field: DomainName, is not set");
    return GetWorkflowOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.WorkflowIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetWorkflow", "Required field: WorkflowId, is not set");
    return GetWorkflowOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkflowId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetWorkflowOutcome>(
    [&]()-> GetWorkflowOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetWorkflow, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workflows/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkflowId());
      return GetWorkflowOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetWorkflowStepsOutcome CustomerProfilesClient::GetWorkflowSteps(const GetWorkflowStepsRequest& request) const
{
  AWS_OPERATION_GUARD(GetWorkflowSteps);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetWorkflowSteps, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetWorkflowSteps", "Required field: DomainName, is not set");
    return GetWorkflowStepsOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.WorkflowIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetWorkflowSteps", "Required field: WorkflowId, is not set");
    return GetWorkflowStepsOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkflowId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetWorkflowStepsOutcome>(
    [&]()-> GetWorkflowStepsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetWorkflowSteps, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workflows/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkflowId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/steps");
      return GetWorkflowStepsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListAccountIntegrationsOutcome CustomerProfilesClient::ListAccountIntegrations(const ListAccountIntegrationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListAccountIntegrations);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListAccountIntegrations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListAccountIntegrationsOutcome>(
    [&]()-> ListAccountIntegrationsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListAccountIntegrations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/integrations");
      return ListAccountIntegrationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListCalculatedAttributeDefinitionsOutcome CustomerProfilesClient::ListCalculatedAttributeDefinitions(const ListCalculatedAttributeDefinitionsRequest& request) const
{
  AWS_OPERATION_GUARD(ListCalculatedAttributeDefinitions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListCalculatedAttributeDefinitions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListCalculatedAttributeDefinitions", "Required field: DomainName, is not set");
    return ListCalculatedAttributeDefinitionsOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListCalculatedAttributeDefinitionsOutcome>(
    [&]()-> ListCalculatedAttributeDefinitionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListCalculatedAttributeDefinitions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/calculated-attributes");
      return ListCalculatedAttributeDefinitionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListCalculatedAttributesForProfileOutcome CustomerProfilesClient::ListCalculatedAttributesForProfile(const ListCalculatedAttributesForProfileRequest& request) const
{
  AWS_OPERATION_GUARD(ListCalculatedAttributesForProfile);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListCalculatedAttributesForProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListCalculatedAttributesForProfile", "Required field: DomainName, is not set");
    return ListCalculatedAttributesForProfileOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.ProfileIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListCalculatedAttributesForProfile", "Required field: ProfileId, is not set");
    return ListCalculatedAttributesForProfileOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ProfileId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListCalculatedAttributesForProfileOutcome>(
    [&]()-> ListCalculatedAttributesForProfileOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListCalculatedAttributesForProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profile/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetProfileId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/calculated-attributes");
      return ListCalculatedAttributesForProfileOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListDomainsOutcome CustomerProfilesClient::ListDomains(const ListDomainsRequest& request) const
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
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains");
      return ListDomainsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListEventStreamsOutcome CustomerProfilesClient::ListEventStreams(const ListEventStreamsRequest& request) const
{
  AWS_OPERATION_GUARD(ListEventStreams);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListEventStreams, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListEventStreams", "Required field: DomainName, is not set");
    return ListEventStreamsOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListEventStreamsOutcome>(
    [&]()-> ListEventStreamsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListEventStreams, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/event-streams");
      return ListEventStreamsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListIdentityResolutionJobsOutcome CustomerProfilesClient::ListIdentityResolutionJobs(const ListIdentityResolutionJobsRequest& request) const
{
  AWS_OPERATION_GUARD(ListIdentityResolutionJobs);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListIdentityResolutionJobs, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListIdentityResolutionJobs", "Required field: DomainName, is not set");
    return ListIdentityResolutionJobsOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListIdentityResolutionJobsOutcome>(
    [&]()-> ListIdentityResolutionJobsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListIdentityResolutionJobs, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/identity-resolution-jobs");
      return ListIdentityResolutionJobsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListIntegrationsOutcome CustomerProfilesClient::ListIntegrations(const ListIntegrationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListIntegrations);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListIntegrations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListIntegrations", "Required field: DomainName, is not set");
    return ListIntegrationsOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListIntegrationsOutcome>(
    [&]()-> ListIntegrationsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListIntegrations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/integrations");
      return ListIntegrationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListProfileObjectTypeTemplatesOutcome CustomerProfilesClient::ListProfileObjectTypeTemplates(const ListProfileObjectTypeTemplatesRequest& request) const
{
  AWS_OPERATION_GUARD(ListProfileObjectTypeTemplates);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListProfileObjectTypeTemplates, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListProfileObjectTypeTemplatesOutcome>(
    [&]()-> ListProfileObjectTypeTemplatesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListProfileObjectTypeTemplates, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/templates");
      return ListProfileObjectTypeTemplatesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListProfileObjectTypesOutcome CustomerProfilesClient::ListProfileObjectTypes(const ListProfileObjectTypesRequest& request) const
{
  AWS_OPERATION_GUARD(ListProfileObjectTypes);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListProfileObjectTypes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListProfileObjectTypes", "Required field: DomainName, is not set");
    return ListProfileObjectTypesOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListProfileObjectTypesOutcome>(
    [&]()-> ListProfileObjectTypesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListProfileObjectTypes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/object-types");
      return ListProfileObjectTypesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListProfileObjectsOutcome CustomerProfilesClient::ListProfileObjects(const ListProfileObjectsRequest& request) const
{
  AWS_OPERATION_GUARD(ListProfileObjects);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListProfileObjects, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListProfileObjects", "Required field: DomainName, is not set");
    return ListProfileObjectsOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListProfileObjectsOutcome>(
    [&]()-> ListProfileObjectsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListProfileObjects, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/objects");
      return ListProfileObjectsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListTagsForResourceOutcome CustomerProfilesClient::ListTagsForResource(const ListTagsForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(ListTagsForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListTagsForResource", "Required field: ResourceArn, is not set");
    return ListTagsForResourceOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListTagsForResourceOutcome>(
    [&]()-> ListTagsForResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/tags/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetResourceArn());
      return ListTagsForResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListWorkflowsOutcome CustomerProfilesClient::ListWorkflows(const ListWorkflowsRequest& request) const
{
  AWS_OPERATION_GUARD(ListWorkflows);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListWorkflows, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListWorkflows", "Required field: DomainName, is not set");
    return ListWorkflowsOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListWorkflowsOutcome>(
    [&]()-> ListWorkflowsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListWorkflows, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workflows");
      return ListWorkflowsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

MergeProfilesOutcome CustomerProfilesClient::MergeProfiles(const MergeProfilesRequest& request) const
{
  AWS_OPERATION_GUARD(MergeProfiles);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, MergeProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("MergeProfiles", "Required field: DomainName, is not set");
    return MergeProfilesOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<MergeProfilesOutcome>(
    [&]()-> MergeProfilesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, MergeProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/objects/merge");
      return MergeProfilesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

PutIntegrationOutcome CustomerProfilesClient::PutIntegration(const PutIntegrationRequest& request) const
{
  AWS_OPERATION_GUARD(PutIntegration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, PutIntegration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("PutIntegration", "Required field: DomainName, is not set");
    return PutIntegrationOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<PutIntegrationOutcome>(
    [&]()-> PutIntegrationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, PutIntegration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/integrations");
      return PutIntegrationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

PutProfileObjectOutcome CustomerProfilesClient::PutProfileObject(const PutProfileObjectRequest& request) const
{
  AWS_OPERATION_GUARD(PutProfileObject);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, PutProfileObject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("PutProfileObject", "Required field: DomainName, is not set");
    return PutProfileObjectOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<PutProfileObjectOutcome>(
    [&]()-> PutProfileObjectOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, PutProfileObject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/objects");
      return PutProfileObjectOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

PutProfileObjectTypeOutcome CustomerProfilesClient::PutProfileObjectType(const PutProfileObjectTypeRequest& request) const
{
  AWS_OPERATION_GUARD(PutProfileObjectType);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, PutProfileObjectType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("PutProfileObjectType", "Required field: DomainName, is not set");
    return PutProfileObjectTypeOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.ObjectTypeNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("PutProfileObjectType", "Required field: ObjectTypeName, is not set");
    return PutProfileObjectTypeOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ObjectTypeName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<PutProfileObjectTypeOutcome>(
    [&]()-> PutProfileObjectTypeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, PutProfileObjectType, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/object-types/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetObjectTypeName());
      return PutProfileObjectTypeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

SearchProfilesOutcome CustomerProfilesClient::SearchProfiles(const SearchProfilesRequest& request) const
{
  AWS_OPERATION_GUARD(SearchProfiles);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, SearchProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("SearchProfiles", "Required field: DomainName, is not set");
    return SearchProfilesOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<SearchProfilesOutcome>(
    [&]()-> SearchProfilesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, SearchProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/search");
      return SearchProfilesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

TagResourceOutcome CustomerProfilesClient::TagResource(const TagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(TagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, TagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("TagResource", "Required field: ResourceArn, is not set");
    return TagResourceOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<TagResourceOutcome>(
    [&]()-> TagResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, TagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/tags/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetResourceArn());
      return TagResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UntagResourceOutcome CustomerProfilesClient::UntagResource(const UntagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(UntagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UntagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: ResourceArn, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
  }
  if (!request.TagKeysHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: TagKeys, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [TagKeys]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UntagResourceOutcome>(
    [&]()-> UntagResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UntagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/tags/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetResourceArn());
      return UntagResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateCalculatedAttributeDefinitionOutcome CustomerProfilesClient::UpdateCalculatedAttributeDefinition(const UpdateCalculatedAttributeDefinitionRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateCalculatedAttributeDefinition);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateCalculatedAttributeDefinition, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateCalculatedAttributeDefinition", "Required field: DomainName, is not set");
    return UpdateCalculatedAttributeDefinitionOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  if (!request.CalculatedAttributeNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateCalculatedAttributeDefinition", "Required field: CalculatedAttributeName, is not set");
    return UpdateCalculatedAttributeDefinitionOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [CalculatedAttributeName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateCalculatedAttributeDefinitionOutcome>(
    [&]()-> UpdateCalculatedAttributeDefinitionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateCalculatedAttributeDefinition, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/calculated-attributes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetCalculatedAttributeName());
      return UpdateCalculatedAttributeDefinitionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateDomainOutcome CustomerProfilesClient::UpdateDomain(const UpdateDomainRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateDomain);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateDomain", "Required field: DomainName, is not set");
    return UpdateDomainOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateDomainOutcome>(
    [&]()-> UpdateDomainOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateDomain, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      return UpdateDomainOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateProfileOutcome CustomerProfilesClient::UpdateProfile(const UpdateProfileRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateProfile);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DomainNameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateProfile", "Required field: DomainName, is not set");
    return UpdateProfileOutcome(Aws::Client::AWSError<CustomerProfilesErrors>(CustomerProfilesErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DomainName]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateProfileOutcome>(
    [&]()-> UpdateProfileOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/domains/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDomainName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles");
      return UpdateProfileOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

