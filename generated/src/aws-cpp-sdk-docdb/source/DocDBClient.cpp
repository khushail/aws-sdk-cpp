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
#include <aws/core/utils/xml/XmlSerializer.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/core/utils/threading/Executor.h>
#include <aws/core/utils/DNS.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/logging/ErrorMacros.h>

#include <aws/docdb/DocDBClient.h>
#include <aws/docdb/DocDBErrorMarshaller.h>
#include <aws/docdb/DocDBEndpointProvider.h>
#include <aws/docdb/model/AddSourceIdentifierToSubscriptionRequest.h>
#include <aws/docdb/model/AddTagsToResourceRequest.h>
#include <aws/docdb/model/ApplyPendingMaintenanceActionRequest.h>
#include <aws/docdb/model/CopyDBClusterParameterGroupRequest.h>
#include <aws/docdb/model/CopyDBClusterSnapshotRequest.h>
#include <aws/docdb/model/CreateDBClusterRequest.h>
#include <aws/docdb/model/CreateDBClusterParameterGroupRequest.h>
#include <aws/docdb/model/CreateDBClusterSnapshotRequest.h>
#include <aws/docdb/model/CreateDBInstanceRequest.h>
#include <aws/docdb/model/CreateDBSubnetGroupRequest.h>
#include <aws/docdb/model/CreateEventSubscriptionRequest.h>
#include <aws/docdb/model/CreateGlobalClusterRequest.h>
#include <aws/docdb/model/DeleteDBClusterRequest.h>
#include <aws/docdb/model/DeleteDBClusterParameterGroupRequest.h>
#include <aws/docdb/model/DeleteDBClusterSnapshotRequest.h>
#include <aws/docdb/model/DeleteDBInstanceRequest.h>
#include <aws/docdb/model/DeleteDBSubnetGroupRequest.h>
#include <aws/docdb/model/DeleteEventSubscriptionRequest.h>
#include <aws/docdb/model/DeleteGlobalClusterRequest.h>
#include <aws/docdb/model/DescribeCertificatesRequest.h>
#include <aws/docdb/model/DescribeDBClusterParameterGroupsRequest.h>
#include <aws/docdb/model/DescribeDBClusterParametersRequest.h>
#include <aws/docdb/model/DescribeDBClusterSnapshotAttributesRequest.h>
#include <aws/docdb/model/DescribeDBClusterSnapshotsRequest.h>
#include <aws/docdb/model/DescribeDBClustersRequest.h>
#include <aws/docdb/model/DescribeDBEngineVersionsRequest.h>
#include <aws/docdb/model/DescribeDBInstancesRequest.h>
#include <aws/docdb/model/DescribeDBSubnetGroupsRequest.h>
#include <aws/docdb/model/DescribeEngineDefaultClusterParametersRequest.h>
#include <aws/docdb/model/DescribeEventCategoriesRequest.h>
#include <aws/docdb/model/DescribeEventSubscriptionsRequest.h>
#include <aws/docdb/model/DescribeEventsRequest.h>
#include <aws/docdb/model/DescribeGlobalClustersRequest.h>
#include <aws/docdb/model/DescribeOrderableDBInstanceOptionsRequest.h>
#include <aws/docdb/model/DescribePendingMaintenanceActionsRequest.h>
#include <aws/docdb/model/FailoverDBClusterRequest.h>
#include <aws/docdb/model/ListTagsForResourceRequest.h>
#include <aws/docdb/model/ModifyDBClusterRequest.h>
#include <aws/docdb/model/ModifyDBClusterParameterGroupRequest.h>
#include <aws/docdb/model/ModifyDBClusterSnapshotAttributeRequest.h>
#include <aws/docdb/model/ModifyDBInstanceRequest.h>
#include <aws/docdb/model/ModifyDBSubnetGroupRequest.h>
#include <aws/docdb/model/ModifyEventSubscriptionRequest.h>
#include <aws/docdb/model/ModifyGlobalClusterRequest.h>
#include <aws/docdb/model/RebootDBInstanceRequest.h>
#include <aws/docdb/model/RemoveFromGlobalClusterRequest.h>
#include <aws/docdb/model/RemoveSourceIdentifierFromSubscriptionRequest.h>
#include <aws/docdb/model/RemoveTagsFromResourceRequest.h>
#include <aws/docdb/model/ResetDBClusterParameterGroupRequest.h>
#include <aws/docdb/model/RestoreDBClusterFromSnapshotRequest.h>
#include <aws/docdb/model/RestoreDBClusterToPointInTimeRequest.h>
#include <aws/docdb/model/StartDBClusterRequest.h>
#include <aws/docdb/model/StopDBClusterRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::DocDB;
using namespace Aws::DocDB::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Xml;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;


const char* DocDBClient::SERVICE_NAME = "rds";
const char* DocDBClient::ALLOCATION_TAG = "DocDBClient";

DocDBClient::DocDBClient(const DocDB::DocDBClientConfiguration& clientConfiguration,
                         std::shared_ptr<DocDBEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<DocDBErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

DocDBClient::DocDBClient(const AWSCredentials& credentials,
                         std::shared_ptr<DocDBEndpointProviderBase> endpointProvider,
                         const DocDB::DocDBClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<DocDBErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

DocDBClient::DocDBClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                         std::shared_ptr<DocDBEndpointProviderBase> endpointProvider,
                         const DocDB::DocDBClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<DocDBErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  DocDBClient::DocDBClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<DocDBErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<DocDBEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

DocDBClient::DocDBClient(const AWSCredentials& credentials,
                         const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<DocDBErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<DocDBEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

DocDBClient::DocDBClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                         const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<DocDBErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<DocDBEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
DocDBClient::~DocDBClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<DocDBEndpointProviderBase>& DocDBClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void DocDBClient::init(const DocDB::DocDBClientConfiguration& config)
{
  AWSClient::SetServiceClientName("DocDB");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void DocDBClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

Aws::String DocDBClient::ConvertRequestToPresignedUrl(const AmazonSerializableWebServiceRequest& requestToConvert, const char* region) const
{
  if (!m_endpointProvider)
  {
    AWS_LOGSTREAM_ERROR(ALLOCATION_TAG, "Presigned URL generating failed. Endpoint provider is not initialized.");
    return "";
  }
  Aws::Endpoint::EndpointParameters endpointParameters;
  endpointParameters.emplace_back(Aws::Endpoint::EndpointParameter("Region", Aws::String(region)));
  ResolveEndpointOutcome endpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(endpointParameters);
  if (!endpointResolutionOutcome.IsSuccess())
  {
    AWS_LOGSTREAM_ERROR(ALLOCATION_TAG, "Endpoint resolution failed: " << endpointResolutionOutcome.GetError().GetMessage());
    return "";
  }
  Aws::StringStream ss;
  ss << "?" << requestToConvert.SerializePayload();
  endpointResolutionOutcome.GetResult().SetQueryString(ss.str());

  return GeneratePresignedUrl(endpointResolutionOutcome.GetResult().GetURI(), Aws::Http::HttpMethod::HTTP_GET, region, 3600);
}

AddSourceIdentifierToSubscriptionOutcome DocDBClient::AddSourceIdentifierToSubscription(const AddSourceIdentifierToSubscriptionRequest& request) const
{
  AWS_OPERATION_GUARD(AddSourceIdentifierToSubscription);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AddSourceIdentifierToSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AddSourceIdentifierToSubscriptionOutcome>(
    [&]()-> AddSourceIdentifierToSubscriptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AddSourceIdentifierToSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AddSourceIdentifierToSubscriptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

AddTagsToResourceOutcome DocDBClient::AddTagsToResource(const AddTagsToResourceRequest& request) const
{
  AWS_OPERATION_GUARD(AddTagsToResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AddTagsToResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AddTagsToResourceOutcome>(
    [&]()-> AddTagsToResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AddTagsToResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AddTagsToResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ApplyPendingMaintenanceActionOutcome DocDBClient::ApplyPendingMaintenanceAction(const ApplyPendingMaintenanceActionRequest& request) const
{
  AWS_OPERATION_GUARD(ApplyPendingMaintenanceAction);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ApplyPendingMaintenanceAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ApplyPendingMaintenanceActionOutcome>(
    [&]()-> ApplyPendingMaintenanceActionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ApplyPendingMaintenanceAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ApplyPendingMaintenanceActionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CopyDBClusterParameterGroupOutcome DocDBClient::CopyDBClusterParameterGroup(const CopyDBClusterParameterGroupRequest& request) const
{
  AWS_OPERATION_GUARD(CopyDBClusterParameterGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CopyDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CopyDBClusterParameterGroupOutcome>(
    [&]()-> CopyDBClusterParameterGroupOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CopyDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CopyDBClusterParameterGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CopyDBClusterSnapshotOutcome DocDBClient::CopyDBClusterSnapshot(const CopyDBClusterSnapshotRequest& request) const
{
  AWS_OPERATION_GUARD(CopyDBClusterSnapshot);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CopyDBClusterSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CopyDBClusterSnapshotOutcome>(
    [&]()-> CopyDBClusterSnapshotOutcome {
  CopyDBClusterSnapshotRequest newRequest = request;
      if (request.SourceRegionHasBeenSet() && !request.PreSignedUrlHasBeenSet())
      {
          Aws::Endpoint::EndpointParameters endpointParameters;
          endpointParameters.emplace_back(Aws::Endpoint::EndpointParameter("Region", request.GetSourceRegion()));
          ResolveEndpointOutcome presignedEndpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(endpointParameters);
          AWS_OPERATION_CHECK_SUCCESS(presignedEndpointResolutionOutcome, CopyDBClusterSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, presignedEndpointResolutionOutcome.GetError().GetMessage());
          newRequest.SetPreSignedUrl(GeneratePresignedUrl(request, presignedEndpointResolutionOutcome.GetResult().GetURI(),
                                                  Aws::Http::HttpMethod::HTTP_GET, request.GetSourceRegion().c_str(),
                                                  {{ "DestinationRegion", m_region }}, 3600));
      }
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CopyDBClusterSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CopyDBClusterSnapshotOutcome(MakeRequest(newRequest, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDBClusterOutcome DocDBClient::CreateDBCluster(const CreateDBClusterRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDBCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDBClusterOutcome>(
    [&]()-> CreateDBClusterOutcome {
  CreateDBClusterRequest newRequest = request;
      if (request.SourceRegionHasBeenSet() && !request.PreSignedUrlHasBeenSet())
      {
          Aws::Endpoint::EndpointParameters endpointParameters;
          endpointParameters.emplace_back(Aws::Endpoint::EndpointParameter("Region", request.GetSourceRegion()));
          ResolveEndpointOutcome presignedEndpointResolutionOutcome = m_endpointProvider->ResolveEndpoint(endpointParameters);
          AWS_OPERATION_CHECK_SUCCESS(presignedEndpointResolutionOutcome, CreateDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, presignedEndpointResolutionOutcome.GetError().GetMessage());
          newRequest.SetPreSignedUrl(GeneratePresignedUrl(request, presignedEndpointResolutionOutcome.GetResult().GetURI(),
                                                  Aws::Http::HttpMethod::HTTP_GET, request.GetSourceRegion().c_str(),
                                                  {{ "DestinationRegion", m_region }}, 3600));
      }
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateDBClusterOutcome(MakeRequest(newRequest, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDBClusterParameterGroupOutcome DocDBClient::CreateDBClusterParameterGroup(const CreateDBClusterParameterGroupRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDBClusterParameterGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDBClusterParameterGroupOutcome>(
    [&]()-> CreateDBClusterParameterGroupOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateDBClusterParameterGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDBClusterSnapshotOutcome DocDBClient::CreateDBClusterSnapshot(const CreateDBClusterSnapshotRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDBClusterSnapshot);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDBClusterSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDBClusterSnapshotOutcome>(
    [&]()-> CreateDBClusterSnapshotOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDBClusterSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateDBClusterSnapshotOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDBInstanceOutcome DocDBClient::CreateDBInstance(const CreateDBInstanceRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDBInstance);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDBInstance, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDBInstanceOutcome>(
    [&]()-> CreateDBInstanceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDBInstance, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateDBInstanceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDBSubnetGroupOutcome DocDBClient::CreateDBSubnetGroup(const CreateDBSubnetGroupRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDBSubnetGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDBSubnetGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDBSubnetGroupOutcome>(
    [&]()-> CreateDBSubnetGroupOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDBSubnetGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateDBSubnetGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateEventSubscriptionOutcome DocDBClient::CreateEventSubscription(const CreateEventSubscriptionRequest& request) const
{
  AWS_OPERATION_GUARD(CreateEventSubscription);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateEventSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateEventSubscriptionOutcome>(
    [&]()-> CreateEventSubscriptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateEventSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateEventSubscriptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateGlobalClusterOutcome DocDBClient::CreateGlobalCluster(const CreateGlobalClusterRequest& request) const
{
  AWS_OPERATION_GUARD(CreateGlobalCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateGlobalCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateGlobalClusterOutcome>(
    [&]()-> CreateGlobalClusterOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateGlobalCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateGlobalClusterOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDBClusterOutcome DocDBClient::DeleteDBCluster(const DeleteDBClusterRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDBCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDBClusterOutcome>(
    [&]()-> DeleteDBClusterOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteDBClusterOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDBClusterParameterGroupOutcome DocDBClient::DeleteDBClusterParameterGroup(const DeleteDBClusterParameterGroupRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDBClusterParameterGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDBClusterParameterGroupOutcome>(
    [&]()-> DeleteDBClusterParameterGroupOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteDBClusterParameterGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDBClusterSnapshotOutcome DocDBClient::DeleteDBClusterSnapshot(const DeleteDBClusterSnapshotRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDBClusterSnapshot);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDBClusterSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDBClusterSnapshotOutcome>(
    [&]()-> DeleteDBClusterSnapshotOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDBClusterSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteDBClusterSnapshotOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDBInstanceOutcome DocDBClient::DeleteDBInstance(const DeleteDBInstanceRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDBInstance);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDBInstance, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDBInstanceOutcome>(
    [&]()-> DeleteDBInstanceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDBInstance, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteDBInstanceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDBSubnetGroupOutcome DocDBClient::DeleteDBSubnetGroup(const DeleteDBSubnetGroupRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDBSubnetGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDBSubnetGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDBSubnetGroupOutcome>(
    [&]()-> DeleteDBSubnetGroupOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDBSubnetGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteDBSubnetGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteEventSubscriptionOutcome DocDBClient::DeleteEventSubscription(const DeleteEventSubscriptionRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteEventSubscription);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteEventSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteEventSubscriptionOutcome>(
    [&]()-> DeleteEventSubscriptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteEventSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteEventSubscriptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteGlobalClusterOutcome DocDBClient::DeleteGlobalCluster(const DeleteGlobalClusterRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteGlobalCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteGlobalCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteGlobalClusterOutcome>(
    [&]()-> DeleteGlobalClusterOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteGlobalCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteGlobalClusterOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeCertificatesOutcome DocDBClient::DescribeCertificates(const DescribeCertificatesRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeCertificates);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeCertificates, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeCertificatesOutcome>(
    [&]()-> DescribeCertificatesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeCertificates, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeCertificatesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDBClusterParameterGroupsOutcome DocDBClient::DescribeDBClusterParameterGroups(const DescribeDBClusterParameterGroupsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDBClusterParameterGroups);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDBClusterParameterGroups, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDBClusterParameterGroupsOutcome>(
    [&]()-> DescribeDBClusterParameterGroupsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDBClusterParameterGroups, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeDBClusterParameterGroupsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDBClusterParametersOutcome DocDBClient::DescribeDBClusterParameters(const DescribeDBClusterParametersRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDBClusterParameters);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDBClusterParameters, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDBClusterParametersOutcome>(
    [&]()-> DescribeDBClusterParametersOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDBClusterParameters, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeDBClusterParametersOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDBClusterSnapshotAttributesOutcome DocDBClient::DescribeDBClusterSnapshotAttributes(const DescribeDBClusterSnapshotAttributesRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDBClusterSnapshotAttributes);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDBClusterSnapshotAttributes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDBClusterSnapshotAttributesOutcome>(
    [&]()-> DescribeDBClusterSnapshotAttributesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDBClusterSnapshotAttributes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeDBClusterSnapshotAttributesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDBClusterSnapshotsOutcome DocDBClient::DescribeDBClusterSnapshots(const DescribeDBClusterSnapshotsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDBClusterSnapshots);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDBClusterSnapshots, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDBClusterSnapshotsOutcome>(
    [&]()-> DescribeDBClusterSnapshotsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDBClusterSnapshots, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeDBClusterSnapshotsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDBClustersOutcome DocDBClient::DescribeDBClusters(const DescribeDBClustersRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDBClusters);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDBClusters, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDBClustersOutcome>(
    [&]()-> DescribeDBClustersOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDBClusters, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeDBClustersOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDBEngineVersionsOutcome DocDBClient::DescribeDBEngineVersions(const DescribeDBEngineVersionsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDBEngineVersions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDBEngineVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDBEngineVersionsOutcome>(
    [&]()-> DescribeDBEngineVersionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDBEngineVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeDBEngineVersionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDBInstancesOutcome DocDBClient::DescribeDBInstances(const DescribeDBInstancesRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDBInstances);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDBInstances, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDBInstancesOutcome>(
    [&]()-> DescribeDBInstancesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDBInstances, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeDBInstancesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDBSubnetGroupsOutcome DocDBClient::DescribeDBSubnetGroups(const DescribeDBSubnetGroupsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDBSubnetGroups);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDBSubnetGroups, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDBSubnetGroupsOutcome>(
    [&]()-> DescribeDBSubnetGroupsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDBSubnetGroups, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeDBSubnetGroupsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeEngineDefaultClusterParametersOutcome DocDBClient::DescribeEngineDefaultClusterParameters(const DescribeEngineDefaultClusterParametersRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeEngineDefaultClusterParameters);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeEngineDefaultClusterParameters, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeEngineDefaultClusterParametersOutcome>(
    [&]()-> DescribeEngineDefaultClusterParametersOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeEngineDefaultClusterParameters, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeEngineDefaultClusterParametersOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeEventCategoriesOutcome DocDBClient::DescribeEventCategories(const DescribeEventCategoriesRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeEventCategories);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeEventCategories, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeEventCategoriesOutcome>(
    [&]()-> DescribeEventCategoriesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeEventCategories, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeEventCategoriesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeEventSubscriptionsOutcome DocDBClient::DescribeEventSubscriptions(const DescribeEventSubscriptionsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeEventSubscriptions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeEventSubscriptions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeEventSubscriptionsOutcome>(
    [&]()-> DescribeEventSubscriptionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeEventSubscriptions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeEventSubscriptionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeEventsOutcome DocDBClient::DescribeEvents(const DescribeEventsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeEvents);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeEvents, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeEventsOutcome>(
    [&]()-> DescribeEventsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeEvents, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeEventsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeGlobalClustersOutcome DocDBClient::DescribeGlobalClusters(const DescribeGlobalClustersRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeGlobalClusters);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeGlobalClusters, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeGlobalClustersOutcome>(
    [&]()-> DescribeGlobalClustersOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeGlobalClusters, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeGlobalClustersOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeOrderableDBInstanceOptionsOutcome DocDBClient::DescribeOrderableDBInstanceOptions(const DescribeOrderableDBInstanceOptionsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeOrderableDBInstanceOptions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeOrderableDBInstanceOptions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeOrderableDBInstanceOptionsOutcome>(
    [&]()-> DescribeOrderableDBInstanceOptionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeOrderableDBInstanceOptions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeOrderableDBInstanceOptionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribePendingMaintenanceActionsOutcome DocDBClient::DescribePendingMaintenanceActions(const DescribePendingMaintenanceActionsRequest& request) const
{
  AWS_OPERATION_GUARD(DescribePendingMaintenanceActions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribePendingMaintenanceActions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribePendingMaintenanceActionsOutcome>(
    [&]()-> DescribePendingMaintenanceActionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribePendingMaintenanceActions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribePendingMaintenanceActionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

FailoverDBClusterOutcome DocDBClient::FailoverDBCluster(const FailoverDBClusterRequest& request) const
{
  AWS_OPERATION_GUARD(FailoverDBCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, FailoverDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<FailoverDBClusterOutcome>(
    [&]()-> FailoverDBClusterOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, FailoverDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return FailoverDBClusterOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListTagsForResourceOutcome DocDBClient::ListTagsForResource(const ListTagsForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(ListTagsForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListTagsForResourceOutcome>(
    [&]()-> ListTagsForResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListTagsForResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ModifyDBClusterOutcome DocDBClient::ModifyDBCluster(const ModifyDBClusterRequest& request) const
{
  AWS_OPERATION_GUARD(ModifyDBCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ModifyDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ModifyDBClusterOutcome>(
    [&]()-> ModifyDBClusterOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ModifyDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ModifyDBClusterOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ModifyDBClusterParameterGroupOutcome DocDBClient::ModifyDBClusterParameterGroup(const ModifyDBClusterParameterGroupRequest& request) const
{
  AWS_OPERATION_GUARD(ModifyDBClusterParameterGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ModifyDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ModifyDBClusterParameterGroupOutcome>(
    [&]()-> ModifyDBClusterParameterGroupOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ModifyDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ModifyDBClusterParameterGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ModifyDBClusterSnapshotAttributeOutcome DocDBClient::ModifyDBClusterSnapshotAttribute(const ModifyDBClusterSnapshotAttributeRequest& request) const
{
  AWS_OPERATION_GUARD(ModifyDBClusterSnapshotAttribute);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ModifyDBClusterSnapshotAttribute, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ModifyDBClusterSnapshotAttributeOutcome>(
    [&]()-> ModifyDBClusterSnapshotAttributeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ModifyDBClusterSnapshotAttribute, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ModifyDBClusterSnapshotAttributeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ModifyDBInstanceOutcome DocDBClient::ModifyDBInstance(const ModifyDBInstanceRequest& request) const
{
  AWS_OPERATION_GUARD(ModifyDBInstance);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ModifyDBInstance, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ModifyDBInstanceOutcome>(
    [&]()-> ModifyDBInstanceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ModifyDBInstance, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ModifyDBInstanceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ModifyDBSubnetGroupOutcome DocDBClient::ModifyDBSubnetGroup(const ModifyDBSubnetGroupRequest& request) const
{
  AWS_OPERATION_GUARD(ModifyDBSubnetGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ModifyDBSubnetGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ModifyDBSubnetGroupOutcome>(
    [&]()-> ModifyDBSubnetGroupOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ModifyDBSubnetGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ModifyDBSubnetGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ModifyEventSubscriptionOutcome DocDBClient::ModifyEventSubscription(const ModifyEventSubscriptionRequest& request) const
{
  AWS_OPERATION_GUARD(ModifyEventSubscription);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ModifyEventSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ModifyEventSubscriptionOutcome>(
    [&]()-> ModifyEventSubscriptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ModifyEventSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ModifyEventSubscriptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ModifyGlobalClusterOutcome DocDBClient::ModifyGlobalCluster(const ModifyGlobalClusterRequest& request) const
{
  AWS_OPERATION_GUARD(ModifyGlobalCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ModifyGlobalCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ModifyGlobalClusterOutcome>(
    [&]()-> ModifyGlobalClusterOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ModifyGlobalCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ModifyGlobalClusterOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RebootDBInstanceOutcome DocDBClient::RebootDBInstance(const RebootDBInstanceRequest& request) const
{
  AWS_OPERATION_GUARD(RebootDBInstance);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RebootDBInstance, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RebootDBInstanceOutcome>(
    [&]()-> RebootDBInstanceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RebootDBInstance, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RebootDBInstanceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RemoveFromGlobalClusterOutcome DocDBClient::RemoveFromGlobalCluster(const RemoveFromGlobalClusterRequest& request) const
{
  AWS_OPERATION_GUARD(RemoveFromGlobalCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RemoveFromGlobalCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RemoveFromGlobalClusterOutcome>(
    [&]()-> RemoveFromGlobalClusterOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RemoveFromGlobalCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RemoveFromGlobalClusterOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RemoveSourceIdentifierFromSubscriptionOutcome DocDBClient::RemoveSourceIdentifierFromSubscription(const RemoveSourceIdentifierFromSubscriptionRequest& request) const
{
  AWS_OPERATION_GUARD(RemoveSourceIdentifierFromSubscription);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RemoveSourceIdentifierFromSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RemoveSourceIdentifierFromSubscriptionOutcome>(
    [&]()-> RemoveSourceIdentifierFromSubscriptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RemoveSourceIdentifierFromSubscription, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RemoveSourceIdentifierFromSubscriptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RemoveTagsFromResourceOutcome DocDBClient::RemoveTagsFromResource(const RemoveTagsFromResourceRequest& request) const
{
  AWS_OPERATION_GUARD(RemoveTagsFromResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RemoveTagsFromResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RemoveTagsFromResourceOutcome>(
    [&]()-> RemoveTagsFromResourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RemoveTagsFromResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RemoveTagsFromResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ResetDBClusterParameterGroupOutcome DocDBClient::ResetDBClusterParameterGroup(const ResetDBClusterParameterGroupRequest& request) const
{
  AWS_OPERATION_GUARD(ResetDBClusterParameterGroup);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ResetDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResetDBClusterParameterGroupOutcome>(
    [&]()-> ResetDBClusterParameterGroupOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ResetDBClusterParameterGroup, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ResetDBClusterParameterGroupOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RestoreDBClusterFromSnapshotOutcome DocDBClient::RestoreDBClusterFromSnapshot(const RestoreDBClusterFromSnapshotRequest& request) const
{
  AWS_OPERATION_GUARD(RestoreDBClusterFromSnapshot);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RestoreDBClusterFromSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RestoreDBClusterFromSnapshotOutcome>(
    [&]()-> RestoreDBClusterFromSnapshotOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RestoreDBClusterFromSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RestoreDBClusterFromSnapshotOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

RestoreDBClusterToPointInTimeOutcome DocDBClient::RestoreDBClusterToPointInTime(const RestoreDBClusterToPointInTimeRequest& request) const
{
  AWS_OPERATION_GUARD(RestoreDBClusterToPointInTime);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RestoreDBClusterToPointInTime, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RestoreDBClusterToPointInTimeOutcome>(
    [&]()-> RestoreDBClusterToPointInTimeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RestoreDBClusterToPointInTime, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RestoreDBClusterToPointInTimeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StartDBClusterOutcome DocDBClient::StartDBCluster(const StartDBClusterRequest& request) const
{
  AWS_OPERATION_GUARD(StartDBCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartDBClusterOutcome>(
    [&]()-> StartDBClusterOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return StartDBClusterOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StopDBClusterOutcome DocDBClient::StopDBCluster(const StopDBClusterRequest& request) const
{
  AWS_OPERATION_GUARD(StopDBCluster);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StopDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  auto tracer = m_telemetryProvider->getTracer(this->GetServiceClientName(), {});
  auto span = tracer->CreateSpan(Aws::String(this->GetServiceClientName()) + "." + request.GetServiceRequestName(),
    {{ "rpc.method", request.GetServiceRequestName() }, { "rpc.service", this->GetServiceClientName() }, { "rpc.system", "aws-api" }},
    smithy::components::tracing::SpanKind::CLIENT);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StopDBClusterOutcome>(
    [&]()-> StopDBClusterOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StopDBCluster, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return StopDBClusterOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

