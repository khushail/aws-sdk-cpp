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

#include <aws/privatenetworks/PrivateNetworksClient.h>
#include <aws/privatenetworks/PrivateNetworksErrorMarshaller.h>
#include <aws/privatenetworks/PrivateNetworksEndpointProvider.h>
#include <aws/privatenetworks/model/AcknowledgeOrderReceiptRequest.h>
#include <aws/privatenetworks/model/ActivateDeviceIdentifierRequest.h>
#include <aws/privatenetworks/model/ActivateNetworkSiteRequest.h>
#include <aws/privatenetworks/model/ConfigureAccessPointRequest.h>
#include <aws/privatenetworks/model/CreateNetworkRequest.h>
#include <aws/privatenetworks/model/CreateNetworkSiteRequest.h>
#include <aws/privatenetworks/model/DeactivateDeviceIdentifierRequest.h>
#include <aws/privatenetworks/model/DeleteNetworkRequest.h>
#include <aws/privatenetworks/model/DeleteNetworkSiteRequest.h>
#include <aws/privatenetworks/model/GetDeviceIdentifierRequest.h>
#include <aws/privatenetworks/model/GetNetworkRequest.h>
#include <aws/privatenetworks/model/GetNetworkResourceRequest.h>
#include <aws/privatenetworks/model/GetNetworkSiteRequest.h>
#include <aws/privatenetworks/model/GetOrderRequest.h>
#include <aws/privatenetworks/model/ListDeviceIdentifiersRequest.h>
#include <aws/privatenetworks/model/ListNetworkResourcesRequest.h>
#include <aws/privatenetworks/model/ListNetworkSitesRequest.h>
#include <aws/privatenetworks/model/ListNetworksRequest.h>
#include <aws/privatenetworks/model/ListOrdersRequest.h>
#include <aws/privatenetworks/model/ListTagsForResourceRequest.h>
#include <aws/privatenetworks/model/StartNetworkResourceUpdateRequest.h>
#include <aws/privatenetworks/model/TagResourceRequest.h>
#include <aws/privatenetworks/model/UntagResourceRequest.h>
#include <aws/privatenetworks/model/UpdateNetworkSiteRequest.h>
#include <aws/privatenetworks/model/UpdateNetworkSitePlanRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::PrivateNetworks;
using namespace Aws::PrivateNetworks::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* PrivateNetworksClient::SERVICE_NAME = "private-networks";
const char* PrivateNetworksClient::ALLOCATION_TAG = "PrivateNetworksClient";

PrivateNetworksClient::PrivateNetworksClient(const PrivateNetworks::PrivateNetworksClientConfiguration& clientConfiguration,
                                             std::shared_ptr<PrivateNetworksEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<PrivateNetworksErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

PrivateNetworksClient::PrivateNetworksClient(const AWSCredentials& credentials,
                                             std::shared_ptr<PrivateNetworksEndpointProviderBase> endpointProvider,
                                             const PrivateNetworks::PrivateNetworksClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<PrivateNetworksErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

PrivateNetworksClient::PrivateNetworksClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                             std::shared_ptr<PrivateNetworksEndpointProviderBase> endpointProvider,
                                             const PrivateNetworks::PrivateNetworksClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<PrivateNetworksErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  PrivateNetworksClient::PrivateNetworksClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<PrivateNetworksErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<PrivateNetworksEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

PrivateNetworksClient::PrivateNetworksClient(const AWSCredentials& credentials,
                                             const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<PrivateNetworksErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<PrivateNetworksEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

PrivateNetworksClient::PrivateNetworksClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                             const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<PrivateNetworksErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<PrivateNetworksEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
PrivateNetworksClient::~PrivateNetworksClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<PrivateNetworksEndpointProviderBase>& PrivateNetworksClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void PrivateNetworksClient::init(const PrivateNetworks::PrivateNetworksClientConfiguration& config)
{
  AWSClient::SetServiceClientName("PrivateNetworks");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void PrivateNetworksClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

AcknowledgeOrderReceiptOutcome PrivateNetworksClient::AcknowledgeOrderReceipt(const AcknowledgeOrderReceiptRequest& request) const
{
  AWS_OPERATION_GUARD(AcknowledgeOrderReceipt);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AcknowledgeOrderReceipt, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AcknowledgeOrderReceiptOutcome>(
    [&]()-> AcknowledgeOrderReceiptOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AcknowledgeOrderReceipt, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/orders/acknowledge");
      return AcknowledgeOrderReceiptOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ActivateDeviceIdentifierOutcome PrivateNetworksClient::ActivateDeviceIdentifier(const ActivateDeviceIdentifierRequest& request) const
{
  AWS_OPERATION_GUARD(ActivateDeviceIdentifier);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ActivateDeviceIdentifier, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ActivateDeviceIdentifierOutcome>(
    [&]()-> ActivateDeviceIdentifierOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ActivateDeviceIdentifier, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/device-identifiers/activate");
      return ActivateDeviceIdentifierOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ActivateNetworkSiteOutcome PrivateNetworksClient::ActivateNetworkSite(const ActivateNetworkSiteRequest& request) const
{
  AWS_OPERATION_GUARD(ActivateNetworkSite);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ActivateNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ActivateNetworkSiteOutcome>(
    [&]()-> ActivateNetworkSiteOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ActivateNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-sites/activate");
      return ActivateNetworkSiteOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ConfigureAccessPointOutcome PrivateNetworksClient::ConfigureAccessPoint(const ConfigureAccessPointRequest& request) const
{
  AWS_OPERATION_GUARD(ConfigureAccessPoint);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ConfigureAccessPoint, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ConfigureAccessPointOutcome>(
    [&]()-> ConfigureAccessPointOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ConfigureAccessPoint, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-resources/configure");
      return ConfigureAccessPointOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateNetworkOutcome PrivateNetworksClient::CreateNetwork(const CreateNetworkRequest& request) const
{
  AWS_OPERATION_GUARD(CreateNetwork);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateNetwork, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateNetworkOutcome>(
    [&]()-> CreateNetworkOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateNetwork, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/networks");
      return CreateNetworkOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateNetworkSiteOutcome PrivateNetworksClient::CreateNetworkSite(const CreateNetworkSiteRequest& request) const
{
  AWS_OPERATION_GUARD(CreateNetworkSite);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateNetworkSiteOutcome>(
    [&]()-> CreateNetworkSiteOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-sites");
      return CreateNetworkSiteOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeactivateDeviceIdentifierOutcome PrivateNetworksClient::DeactivateDeviceIdentifier(const DeactivateDeviceIdentifierRequest& request) const
{
  AWS_OPERATION_GUARD(DeactivateDeviceIdentifier);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeactivateDeviceIdentifier, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeactivateDeviceIdentifierOutcome>(
    [&]()-> DeactivateDeviceIdentifierOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeactivateDeviceIdentifier, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/device-identifiers/deactivate");
      return DeactivateDeviceIdentifierOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteNetworkOutcome PrivateNetworksClient::DeleteNetwork(const DeleteNetworkRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteNetwork);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteNetwork, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NetworkArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteNetwork", "Required field: NetworkArn, is not set");
    return DeleteNetworkOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [NetworkArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteNetworkOutcome>(
    [&]()-> DeleteNetworkOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteNetwork, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/networks/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetNetworkArn());
      return DeleteNetworkOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteNetworkSiteOutcome PrivateNetworksClient::DeleteNetworkSite(const DeleteNetworkSiteRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteNetworkSite);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NetworkSiteArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteNetworkSite", "Required field: NetworkSiteArn, is not set");
    return DeleteNetworkSiteOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [NetworkSiteArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteNetworkSiteOutcome>(
    [&]()-> DeleteNetworkSiteOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-sites/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetNetworkSiteArn());
      return DeleteNetworkSiteOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetDeviceIdentifierOutcome PrivateNetworksClient::GetDeviceIdentifier(const GetDeviceIdentifierRequest& request) const
{
  AWS_OPERATION_GUARD(GetDeviceIdentifier);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetDeviceIdentifier, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.DeviceIdentifierArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetDeviceIdentifier", "Required field: DeviceIdentifierArn, is not set");
    return GetDeviceIdentifierOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [DeviceIdentifierArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetDeviceIdentifierOutcome>(
    [&]()-> GetDeviceIdentifierOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetDeviceIdentifier, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/device-identifiers/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetDeviceIdentifierArn());
      return GetDeviceIdentifierOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetNetworkOutcome PrivateNetworksClient::GetNetwork(const GetNetworkRequest& request) const
{
  AWS_OPERATION_GUARD(GetNetwork);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetNetwork, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NetworkArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetNetwork", "Required field: NetworkArn, is not set");
    return GetNetworkOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [NetworkArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetNetworkOutcome>(
    [&]()-> GetNetworkOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetNetwork, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/networks/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetNetworkArn());
      return GetNetworkOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetNetworkResourceOutcome PrivateNetworksClient::GetNetworkResource(const GetNetworkResourceRequest& request) const
{
  AWS_OPERATION_GUARD(GetNetworkResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetNetworkResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NetworkResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetNetworkResource", "Required field: NetworkResourceArn, is not set");
    return GetNetworkResourceOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [NetworkResourceArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetNetworkResourceOutcome>(
    [&]()-> GetNetworkResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetNetworkResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-resources/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetNetworkResourceArn());
      return GetNetworkResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetNetworkSiteOutcome PrivateNetworksClient::GetNetworkSite(const GetNetworkSiteRequest& request) const
{
  AWS_OPERATION_GUARD(GetNetworkSite);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NetworkSiteArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetNetworkSite", "Required field: NetworkSiteArn, is not set");
    return GetNetworkSiteOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [NetworkSiteArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetNetworkSiteOutcome>(
    [&]()-> GetNetworkSiteOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-sites/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetNetworkSiteArn());
      return GetNetworkSiteOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetOrderOutcome PrivateNetworksClient::GetOrder(const GetOrderRequest& request) const
{
  AWS_OPERATION_GUARD(GetOrder);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetOrder, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.OrderArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetOrder", "Required field: OrderArn, is not set");
    return GetOrderOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [OrderArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetOrderOutcome>(
    [&]()-> GetOrderOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetOrder, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/orders/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetOrderArn());
      return GetOrderOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListDeviceIdentifiersOutcome PrivateNetworksClient::ListDeviceIdentifiers(const ListDeviceIdentifiersRequest& request) const
{
  AWS_OPERATION_GUARD(ListDeviceIdentifiers);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListDeviceIdentifiers, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListDeviceIdentifiersOutcome>(
    [&]()-> ListDeviceIdentifiersOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListDeviceIdentifiers, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/device-identifiers/list");
      return ListDeviceIdentifiersOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListNetworkResourcesOutcome PrivateNetworksClient::ListNetworkResources(const ListNetworkResourcesRequest& request) const
{
  AWS_OPERATION_GUARD(ListNetworkResources);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListNetworkResources, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListNetworkResourcesOutcome>(
    [&]()-> ListNetworkResourcesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListNetworkResources, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-resources");
      return ListNetworkResourcesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListNetworkSitesOutcome PrivateNetworksClient::ListNetworkSites(const ListNetworkSitesRequest& request) const
{
  AWS_OPERATION_GUARD(ListNetworkSites);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListNetworkSites, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListNetworkSitesOutcome>(
    [&]()-> ListNetworkSitesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListNetworkSites, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-sites/list");
      return ListNetworkSitesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListNetworksOutcome PrivateNetworksClient::ListNetworks(const ListNetworksRequest& request) const
{
  AWS_OPERATION_GUARD(ListNetworks);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListNetworks, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListNetworksOutcome>(
    [&]()-> ListNetworksOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListNetworks, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/networks/list");
      return ListNetworksOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListOrdersOutcome PrivateNetworksClient::ListOrders(const ListOrdersRequest& request) const
{
  AWS_OPERATION_GUARD(ListOrders);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListOrders, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListOrdersOutcome>(
    [&]()-> ListOrdersOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListOrders, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/orders/list");
      return ListOrdersOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListTagsForResourceOutcome PrivateNetworksClient::ListTagsForResource(const ListTagsForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(ListTagsForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListTagsForResource", "Required field: ResourceArn, is not set");
    return ListTagsForResourceOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
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

PingOutcome PrivateNetworksClient::Ping() const
{
AWS_OPERATION_GUARD(Ping);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + ".Ping",
    {{ "rpc.method", "Ping" }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<PingOutcome>(
    [&]()-> PingOutcome {
        AWS_OPERATION_CHECK_PTR(m_endpointProvider, Ping, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
        const Aws::Vector<Aws::Endpoint::EndpointParameter> staticEndpointParameters;
        auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
            [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(staticEndpointParameters); },
            "smithy.client.resolve_endpoint_duration",
            m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
            {{"rpc.method", "Ping"}, {"rpc.service", this->GetServiceClientName()}});
        AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, Ping, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/ping");
      return PingOutcome(MakeRequest(endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER, "Ping"));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", "Ping"}, {"rpc.service", this->GetServiceClientName()}});
}

StartNetworkResourceUpdateOutcome PrivateNetworksClient::StartNetworkResourceUpdate(const StartNetworkResourceUpdateRequest& request) const
{
  AWS_OPERATION_GUARD(StartNetworkResourceUpdate);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartNetworkResourceUpdate, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartNetworkResourceUpdateOutcome>(
    [&]()-> StartNetworkResourceUpdateOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartNetworkResourceUpdate, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-resources/update");
      return StartNetworkResourceUpdateOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

TagResourceOutcome PrivateNetworksClient::TagResource(const TagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(TagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, TagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("TagResource", "Required field: ResourceArn, is not set");
    return TagResourceOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
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

UntagResourceOutcome PrivateNetworksClient::UntagResource(const UntagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(UntagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UntagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: ResourceArn, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
  }
  if (!request.TagKeysHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: TagKeys, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<PrivateNetworksErrors>(PrivateNetworksErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [TagKeys]", false));
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

UpdateNetworkSiteOutcome PrivateNetworksClient::UpdateNetworkSite(const UpdateNetworkSiteRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateNetworkSite);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateNetworkSiteOutcome>(
    [&]()-> UpdateNetworkSiteOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateNetworkSite, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-sites/site");
      return UpdateNetworkSiteOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateNetworkSitePlanOutcome PrivateNetworksClient::UpdateNetworkSitePlan(const UpdateNetworkSitePlanRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateNetworkSitePlan);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateNetworkSitePlan, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateNetworkSitePlanOutcome>(
    [&]()-> UpdateNetworkSitePlanOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateNetworkSitePlan, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/v1/network-sites/plan");
      return UpdateNetworkSitePlanOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

