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

#include <aws/kinesisanalyticsv2/KinesisAnalyticsV2Client.h>
#include <aws/kinesisanalyticsv2/KinesisAnalyticsV2ErrorMarshaller.h>
#include <aws/kinesisanalyticsv2/KinesisAnalyticsV2EndpointProvider.h>
#include <aws/kinesisanalyticsv2/model/AddApplicationCloudWatchLoggingOptionRequest.h>
#include <aws/kinesisanalyticsv2/model/AddApplicationInputRequest.h>
#include <aws/kinesisanalyticsv2/model/AddApplicationInputProcessingConfigurationRequest.h>
#include <aws/kinesisanalyticsv2/model/AddApplicationOutputRequest.h>
#include <aws/kinesisanalyticsv2/model/AddApplicationReferenceDataSourceRequest.h>
#include <aws/kinesisanalyticsv2/model/AddApplicationVpcConfigurationRequest.h>
#include <aws/kinesisanalyticsv2/model/CreateApplicationRequest.h>
#include <aws/kinesisanalyticsv2/model/CreateApplicationPresignedUrlRequest.h>
#include <aws/kinesisanalyticsv2/model/CreateApplicationSnapshotRequest.h>
#include <aws/kinesisanalyticsv2/model/DeleteApplicationRequest.h>
#include <aws/kinesisanalyticsv2/model/DeleteApplicationCloudWatchLoggingOptionRequest.h>
#include <aws/kinesisanalyticsv2/model/DeleteApplicationInputProcessingConfigurationRequest.h>
#include <aws/kinesisanalyticsv2/model/DeleteApplicationOutputRequest.h>
#include <aws/kinesisanalyticsv2/model/DeleteApplicationReferenceDataSourceRequest.h>
#include <aws/kinesisanalyticsv2/model/DeleteApplicationSnapshotRequest.h>
#include <aws/kinesisanalyticsv2/model/DeleteApplicationVpcConfigurationRequest.h>
#include <aws/kinesisanalyticsv2/model/DescribeApplicationRequest.h>
#include <aws/kinesisanalyticsv2/model/DescribeApplicationSnapshotRequest.h>
#include <aws/kinesisanalyticsv2/model/DescribeApplicationVersionRequest.h>
#include <aws/kinesisanalyticsv2/model/DiscoverInputSchemaRequest.h>
#include <aws/kinesisanalyticsv2/model/ListApplicationSnapshotsRequest.h>
#include <aws/kinesisanalyticsv2/model/ListApplicationVersionsRequest.h>
#include <aws/kinesisanalyticsv2/model/ListApplicationsRequest.h>
#include <aws/kinesisanalyticsv2/model/ListTagsForResourceRequest.h>
#include <aws/kinesisanalyticsv2/model/RollbackApplicationRequest.h>
#include <aws/kinesisanalyticsv2/model/StartApplicationRequest.h>
#include <aws/kinesisanalyticsv2/model/StopApplicationRequest.h>
#include <aws/kinesisanalyticsv2/model/TagResourceRequest.h>
#include <aws/kinesisanalyticsv2/model/UntagResourceRequest.h>
#include <aws/kinesisanalyticsv2/model/UpdateApplicationRequest.h>
#include <aws/kinesisanalyticsv2/model/UpdateApplicationMaintenanceConfigurationRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::KinesisAnalyticsV2;
using namespace Aws::KinesisAnalyticsV2::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* KinesisAnalyticsV2Client::SERVICE_NAME = "kinesisanalytics";
const char* KinesisAnalyticsV2Client::ALLOCATION_TAG = "KinesisAnalyticsV2Client";

KinesisAnalyticsV2Client::KinesisAnalyticsV2Client(const KinesisAnalyticsV2::KinesisAnalyticsV2ClientConfiguration& clientConfiguration,
                                                   std::shared_ptr<KinesisAnalyticsV2EndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KinesisAnalyticsV2ErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

KinesisAnalyticsV2Client::KinesisAnalyticsV2Client(const AWSCredentials& credentials,
                                                   std::shared_ptr<KinesisAnalyticsV2EndpointProviderBase> endpointProvider,
                                                   const KinesisAnalyticsV2::KinesisAnalyticsV2ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KinesisAnalyticsV2ErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

KinesisAnalyticsV2Client::KinesisAnalyticsV2Client(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                                   std::shared_ptr<KinesisAnalyticsV2EndpointProviderBase> endpointProvider,
                                                   const KinesisAnalyticsV2::KinesisAnalyticsV2ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KinesisAnalyticsV2ErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  KinesisAnalyticsV2Client::KinesisAnalyticsV2Client(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KinesisAnalyticsV2ErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<KinesisAnalyticsV2EndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

KinesisAnalyticsV2Client::KinesisAnalyticsV2Client(const AWSCredentials& credentials,
                                                   const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KinesisAnalyticsV2ErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<KinesisAnalyticsV2EndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

KinesisAnalyticsV2Client::KinesisAnalyticsV2Client(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                                   const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<KinesisAnalyticsV2ErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<KinesisAnalyticsV2EndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
KinesisAnalyticsV2Client::~KinesisAnalyticsV2Client()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<KinesisAnalyticsV2EndpointProviderBase>& KinesisAnalyticsV2Client::accessEndpointProvider()
{
  return m_endpointProvider;
}

void KinesisAnalyticsV2Client::init(const KinesisAnalyticsV2::KinesisAnalyticsV2ClientConfiguration& config)
{
  AWSClient::SetServiceClientName("Kinesis Analytics V2");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void KinesisAnalyticsV2Client::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

AddApplicationCloudWatchLoggingOptionOutcome KinesisAnalyticsV2Client::AddApplicationCloudWatchLoggingOption(const AddApplicationCloudWatchLoggingOptionRequest& request) const
{
  AWS_OPERATION_GUARD(AddApplicationCloudWatchLoggingOption);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AddApplicationCloudWatchLoggingOption, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AddApplicationCloudWatchLoggingOptionOutcome>(
    [&]()-> AddApplicationCloudWatchLoggingOptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AddApplicationCloudWatchLoggingOption, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AddApplicationCloudWatchLoggingOptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

AddApplicationInputOutcome KinesisAnalyticsV2Client::AddApplicationInput(const AddApplicationInputRequest& request) const
{
  AWS_OPERATION_GUARD(AddApplicationInput);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AddApplicationInput, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AddApplicationInputOutcome>(
    [&]()-> AddApplicationInputOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AddApplicationInput, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AddApplicationInputOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

AddApplicationInputProcessingConfigurationOutcome KinesisAnalyticsV2Client::AddApplicationInputProcessingConfiguration(const AddApplicationInputProcessingConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(AddApplicationInputProcessingConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AddApplicationInputProcessingConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AddApplicationInputProcessingConfigurationOutcome>(
    [&]()-> AddApplicationInputProcessingConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AddApplicationInputProcessingConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AddApplicationInputProcessingConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

AddApplicationOutputOutcome KinesisAnalyticsV2Client::AddApplicationOutput(const AddApplicationOutputRequest& request) const
{
  AWS_OPERATION_GUARD(AddApplicationOutput);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AddApplicationOutput, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AddApplicationOutputOutcome>(
    [&]()-> AddApplicationOutputOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AddApplicationOutput, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AddApplicationOutputOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

AddApplicationReferenceDataSourceOutcome KinesisAnalyticsV2Client::AddApplicationReferenceDataSource(const AddApplicationReferenceDataSourceRequest& request) const
{
  AWS_OPERATION_GUARD(AddApplicationReferenceDataSource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AddApplicationReferenceDataSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AddApplicationReferenceDataSourceOutcome>(
    [&]()-> AddApplicationReferenceDataSourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AddApplicationReferenceDataSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AddApplicationReferenceDataSourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

AddApplicationVpcConfigurationOutcome KinesisAnalyticsV2Client::AddApplicationVpcConfiguration(const AddApplicationVpcConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(AddApplicationVpcConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AddApplicationVpcConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AddApplicationVpcConfigurationOutcome>(
    [&]()-> AddApplicationVpcConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AddApplicationVpcConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AddApplicationVpcConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateApplicationOutcome KinesisAnalyticsV2Client::CreateApplication(const CreateApplicationRequest& request) const
{
  AWS_OPERATION_GUARD(CreateApplication);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateApplicationOutcome>(
    [&]()-> CreateApplicationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateApplicationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateApplicationPresignedUrlOutcome KinesisAnalyticsV2Client::CreateApplicationPresignedUrl(const CreateApplicationPresignedUrlRequest& request) const
{
  AWS_OPERATION_GUARD(CreateApplicationPresignedUrl);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateApplicationPresignedUrl, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateApplicationPresignedUrlOutcome>(
    [&]()-> CreateApplicationPresignedUrlOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateApplicationPresignedUrl, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateApplicationPresignedUrlOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateApplicationSnapshotOutcome KinesisAnalyticsV2Client::CreateApplicationSnapshot(const CreateApplicationSnapshotRequest& request) const
{
  AWS_OPERATION_GUARD(CreateApplicationSnapshot);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateApplicationSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateApplicationSnapshotOutcome>(
    [&]()-> CreateApplicationSnapshotOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateApplicationSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return CreateApplicationSnapshotOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteApplicationOutcome KinesisAnalyticsV2Client::DeleteApplication(const DeleteApplicationRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteApplication);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteApplicationOutcome>(
    [&]()-> DeleteApplicationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteApplicationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteApplicationCloudWatchLoggingOptionOutcome KinesisAnalyticsV2Client::DeleteApplicationCloudWatchLoggingOption(const DeleteApplicationCloudWatchLoggingOptionRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteApplicationCloudWatchLoggingOption);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteApplicationCloudWatchLoggingOption, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteApplicationCloudWatchLoggingOptionOutcome>(
    [&]()-> DeleteApplicationCloudWatchLoggingOptionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteApplicationCloudWatchLoggingOption, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteApplicationCloudWatchLoggingOptionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteApplicationInputProcessingConfigurationOutcome KinesisAnalyticsV2Client::DeleteApplicationInputProcessingConfiguration(const DeleteApplicationInputProcessingConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteApplicationInputProcessingConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteApplicationInputProcessingConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteApplicationInputProcessingConfigurationOutcome>(
    [&]()-> DeleteApplicationInputProcessingConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteApplicationInputProcessingConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteApplicationInputProcessingConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteApplicationOutputOutcome KinesisAnalyticsV2Client::DeleteApplicationOutput(const DeleteApplicationOutputRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteApplicationOutput);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteApplicationOutput, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteApplicationOutputOutcome>(
    [&]()-> DeleteApplicationOutputOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteApplicationOutput, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteApplicationOutputOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteApplicationReferenceDataSourceOutcome KinesisAnalyticsV2Client::DeleteApplicationReferenceDataSource(const DeleteApplicationReferenceDataSourceRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteApplicationReferenceDataSource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteApplicationReferenceDataSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteApplicationReferenceDataSourceOutcome>(
    [&]()-> DeleteApplicationReferenceDataSourceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteApplicationReferenceDataSource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteApplicationReferenceDataSourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteApplicationSnapshotOutcome KinesisAnalyticsV2Client::DeleteApplicationSnapshot(const DeleteApplicationSnapshotRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteApplicationSnapshot);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteApplicationSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteApplicationSnapshotOutcome>(
    [&]()-> DeleteApplicationSnapshotOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteApplicationSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteApplicationSnapshotOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteApplicationVpcConfigurationOutcome KinesisAnalyticsV2Client::DeleteApplicationVpcConfiguration(const DeleteApplicationVpcConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteApplicationVpcConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteApplicationVpcConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteApplicationVpcConfigurationOutcome>(
    [&]()-> DeleteApplicationVpcConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteApplicationVpcConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DeleteApplicationVpcConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeApplicationOutcome KinesisAnalyticsV2Client::DescribeApplication(const DescribeApplicationRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeApplication);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeApplicationOutcome>(
    [&]()-> DescribeApplicationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeApplicationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeApplicationSnapshotOutcome KinesisAnalyticsV2Client::DescribeApplicationSnapshot(const DescribeApplicationSnapshotRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeApplicationSnapshot);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeApplicationSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeApplicationSnapshotOutcome>(
    [&]()-> DescribeApplicationSnapshotOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeApplicationSnapshot, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeApplicationSnapshotOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeApplicationVersionOutcome KinesisAnalyticsV2Client::DescribeApplicationVersion(const DescribeApplicationVersionRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeApplicationVersion);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeApplicationVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeApplicationVersionOutcome>(
    [&]()-> DescribeApplicationVersionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeApplicationVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DescribeApplicationVersionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DiscoverInputSchemaOutcome KinesisAnalyticsV2Client::DiscoverInputSchema(const DiscoverInputSchemaRequest& request) const
{
  AWS_OPERATION_GUARD(DiscoverInputSchema);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DiscoverInputSchema, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DiscoverInputSchemaOutcome>(
    [&]()-> DiscoverInputSchemaOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DiscoverInputSchema, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DiscoverInputSchemaOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListApplicationSnapshotsOutcome KinesisAnalyticsV2Client::ListApplicationSnapshots(const ListApplicationSnapshotsRequest& request) const
{
  AWS_OPERATION_GUARD(ListApplicationSnapshots);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListApplicationSnapshots, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListApplicationSnapshotsOutcome>(
    [&]()-> ListApplicationSnapshotsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListApplicationSnapshots, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListApplicationSnapshotsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListApplicationVersionsOutcome KinesisAnalyticsV2Client::ListApplicationVersions(const ListApplicationVersionsRequest& request) const
{
  AWS_OPERATION_GUARD(ListApplicationVersions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListApplicationVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListApplicationVersionsOutcome>(
    [&]()-> ListApplicationVersionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListApplicationVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListApplicationVersionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListApplicationsOutcome KinesisAnalyticsV2Client::ListApplications(const ListApplicationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListApplications);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListApplications, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListApplicationsOutcome>(
    [&]()-> ListApplicationsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListApplications, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return ListApplicationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListTagsForResourceOutcome KinesisAnalyticsV2Client::ListTagsForResource(const ListTagsForResourceRequest& request) const
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

RollbackApplicationOutcome KinesisAnalyticsV2Client::RollbackApplication(const RollbackApplicationRequest& request) const
{
  AWS_OPERATION_GUARD(RollbackApplication);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, RollbackApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<RollbackApplicationOutcome>(
    [&]()-> RollbackApplicationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, RollbackApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return RollbackApplicationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StartApplicationOutcome KinesisAnalyticsV2Client::StartApplication(const StartApplicationRequest& request) const
{
  AWS_OPERATION_GUARD(StartApplication);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartApplicationOutcome>(
    [&]()-> StartApplicationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return StartApplicationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StopApplicationOutcome KinesisAnalyticsV2Client::StopApplication(const StopApplicationRequest& request) const
{
  AWS_OPERATION_GUARD(StopApplication);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StopApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StopApplicationOutcome>(
    [&]()-> StopApplicationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StopApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return StopApplicationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

TagResourceOutcome KinesisAnalyticsV2Client::TagResource(const TagResourceRequest& request) const
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

UntagResourceOutcome KinesisAnalyticsV2Client::UntagResource(const UntagResourceRequest& request) const
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

UpdateApplicationOutcome KinesisAnalyticsV2Client::UpdateApplication(const UpdateApplicationRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateApplication);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateApplicationOutcome>(
    [&]()-> UpdateApplicationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateApplication, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateApplicationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateApplicationMaintenanceConfigurationOutcome KinesisAnalyticsV2Client::UpdateApplicationMaintenanceConfiguration(const UpdateApplicationMaintenanceConfigurationRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateApplicationMaintenanceConfiguration);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateApplicationMaintenanceConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateApplicationMaintenanceConfigurationOutcome>(
    [&]()-> UpdateApplicationMaintenanceConfigurationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateApplicationMaintenanceConfiguration, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return UpdateApplicationMaintenanceConfigurationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

