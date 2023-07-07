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

#include <aws/wellarchitected/WellArchitectedClient.h>
#include <aws/wellarchitected/WellArchitectedErrorMarshaller.h>
#include <aws/wellarchitected/WellArchitectedEndpointProvider.h>
#include <aws/wellarchitected/model/AssociateLensesRequest.h>
#include <aws/wellarchitected/model/AssociateProfilesRequest.h>
#include <aws/wellarchitected/model/CreateLensShareRequest.h>
#include <aws/wellarchitected/model/CreateLensVersionRequest.h>
#include <aws/wellarchitected/model/CreateMilestoneRequest.h>
#include <aws/wellarchitected/model/CreateProfileRequest.h>
#include <aws/wellarchitected/model/CreateProfileShareRequest.h>
#include <aws/wellarchitected/model/CreateWorkloadRequest.h>
#include <aws/wellarchitected/model/CreateWorkloadShareRequest.h>
#include <aws/wellarchitected/model/DeleteLensRequest.h>
#include <aws/wellarchitected/model/DeleteLensShareRequest.h>
#include <aws/wellarchitected/model/DeleteProfileRequest.h>
#include <aws/wellarchitected/model/DeleteProfileShareRequest.h>
#include <aws/wellarchitected/model/DeleteWorkloadRequest.h>
#include <aws/wellarchitected/model/DeleteWorkloadShareRequest.h>
#include <aws/wellarchitected/model/DisassociateLensesRequest.h>
#include <aws/wellarchitected/model/DisassociateProfilesRequest.h>
#include <aws/wellarchitected/model/ExportLensRequest.h>
#include <aws/wellarchitected/model/GetAnswerRequest.h>
#include <aws/wellarchitected/model/GetConsolidatedReportRequest.h>
#include <aws/wellarchitected/model/GetLensRequest.h>
#include <aws/wellarchitected/model/GetLensReviewRequest.h>
#include <aws/wellarchitected/model/GetLensReviewReportRequest.h>
#include <aws/wellarchitected/model/GetLensVersionDifferenceRequest.h>
#include <aws/wellarchitected/model/GetMilestoneRequest.h>
#include <aws/wellarchitected/model/GetProfileRequest.h>
#include <aws/wellarchitected/model/GetProfileTemplateRequest.h>
#include <aws/wellarchitected/model/GetWorkloadRequest.h>
#include <aws/wellarchitected/model/ImportLensRequest.h>
#include <aws/wellarchitected/model/ListAnswersRequest.h>
#include <aws/wellarchitected/model/ListCheckDetailsRequest.h>
#include <aws/wellarchitected/model/ListCheckSummariesRequest.h>
#include <aws/wellarchitected/model/ListLensReviewImprovementsRequest.h>
#include <aws/wellarchitected/model/ListLensReviewsRequest.h>
#include <aws/wellarchitected/model/ListLensSharesRequest.h>
#include <aws/wellarchitected/model/ListLensesRequest.h>
#include <aws/wellarchitected/model/ListMilestonesRequest.h>
#include <aws/wellarchitected/model/ListNotificationsRequest.h>
#include <aws/wellarchitected/model/ListProfileNotificationsRequest.h>
#include <aws/wellarchitected/model/ListProfileSharesRequest.h>
#include <aws/wellarchitected/model/ListProfilesRequest.h>
#include <aws/wellarchitected/model/ListShareInvitationsRequest.h>
#include <aws/wellarchitected/model/ListTagsForResourceRequest.h>
#include <aws/wellarchitected/model/ListWorkloadSharesRequest.h>
#include <aws/wellarchitected/model/ListWorkloadsRequest.h>
#include <aws/wellarchitected/model/TagResourceRequest.h>
#include <aws/wellarchitected/model/UntagResourceRequest.h>
#include <aws/wellarchitected/model/UpdateAnswerRequest.h>
#include <aws/wellarchitected/model/UpdateGlobalSettingsRequest.h>
#include <aws/wellarchitected/model/UpdateLensReviewRequest.h>
#include <aws/wellarchitected/model/UpdateProfileRequest.h>
#include <aws/wellarchitected/model/UpdateShareInvitationRequest.h>
#include <aws/wellarchitected/model/UpdateWorkloadRequest.h>
#include <aws/wellarchitected/model/UpdateWorkloadShareRequest.h>
#include <aws/wellarchitected/model/UpgradeLensReviewRequest.h>
#include <aws/wellarchitected/model/UpgradeProfileVersionRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::WellArchitected;
using namespace Aws::WellArchitected::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* WellArchitectedClient::SERVICE_NAME = "wellarchitected";
const char* WellArchitectedClient::ALLOCATION_TAG = "WellArchitectedClient";

WellArchitectedClient::WellArchitectedClient(const WellArchitected::WellArchitectedClientConfiguration& clientConfiguration,
                                             std::shared_ptr<WellArchitectedEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<WellArchitectedErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

WellArchitectedClient::WellArchitectedClient(const AWSCredentials& credentials,
                                             std::shared_ptr<WellArchitectedEndpointProviderBase> endpointProvider,
                                             const WellArchitected::WellArchitectedClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<WellArchitectedErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

WellArchitectedClient::WellArchitectedClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                             std::shared_ptr<WellArchitectedEndpointProviderBase> endpointProvider,
                                             const WellArchitected::WellArchitectedClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<WellArchitectedErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  WellArchitectedClient::WellArchitectedClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<WellArchitectedErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<WellArchitectedEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

WellArchitectedClient::WellArchitectedClient(const AWSCredentials& credentials,
                                             const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<WellArchitectedErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<WellArchitectedEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

WellArchitectedClient::WellArchitectedClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                             const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<WellArchitectedErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<WellArchitectedEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
WellArchitectedClient::~WellArchitectedClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<WellArchitectedEndpointProviderBase>& WellArchitectedClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void WellArchitectedClient::init(const WellArchitected::WellArchitectedClientConfiguration& config)
{
  AWSClient::SetServiceClientName("WellArchitected");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void WellArchitectedClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

AssociateLensesOutcome WellArchitectedClient::AssociateLenses(const AssociateLensesRequest& request) const
{
  AWS_OPERATION_GUARD(AssociateLenses);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AssociateLenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("AssociateLenses", "Required field: WorkloadId, is not set");
    return AssociateLensesOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AssociateLensesOutcome>(
    [&]()-> AssociateLensesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AssociateLenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/associateLenses");
      return AssociateLensesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

AssociateProfilesOutcome WellArchitectedClient::AssociateProfiles(const AssociateProfilesRequest& request) const
{
  AWS_OPERATION_GUARD(AssociateProfiles);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AssociateProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("AssociateProfiles", "Required field: WorkloadId, is not set");
    return AssociateProfilesOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AssociateProfilesOutcome>(
    [&]()-> AssociateProfilesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AssociateProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/associateProfiles");
      return AssociateProfilesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateLensShareOutcome WellArchitectedClient::CreateLensShare(const CreateLensShareRequest& request) const
{
  AWS_OPERATION_GUARD(CreateLensShare);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateLensShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateLensShare", "Required field: LensAlias, is not set");
    return CreateLensShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateLensShareOutcome>(
    [&]()-> CreateLensShareOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateLensShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lenses/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares");
      return CreateLensShareOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateLensVersionOutcome WellArchitectedClient::CreateLensVersion(const CreateLensVersionRequest& request) const
{
  AWS_OPERATION_GUARD(CreateLensVersion);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateLensVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateLensVersion", "Required field: LensAlias, is not set");
    return CreateLensVersionOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateLensVersionOutcome>(
    [&]()-> CreateLensVersionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateLensVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lenses/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/versions");
      return CreateLensVersionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateMilestoneOutcome WellArchitectedClient::CreateMilestone(const CreateMilestoneRequest& request) const
{
  AWS_OPERATION_GUARD(CreateMilestone);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateMilestone, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateMilestone", "Required field: WorkloadId, is not set");
    return CreateMilestoneOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateMilestoneOutcome>(
    [&]()-> CreateMilestoneOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateMilestone, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/milestones");
      return CreateMilestoneOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateProfileOutcome WellArchitectedClient::CreateProfile(const CreateProfileRequest& request) const
{
  AWS_OPERATION_GUARD(CreateProfile);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateProfileOutcome>(
    [&]()-> CreateProfileOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles");
      return CreateProfileOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateProfileShareOutcome WellArchitectedClient::CreateProfileShare(const CreateProfileShareRequest& request) const
{
  AWS_OPERATION_GUARD(CreateProfileShare);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateProfileShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ProfileArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateProfileShare", "Required field: ProfileArn, is not set");
    return CreateProfileShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ProfileArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateProfileShareOutcome>(
    [&]()-> CreateProfileShareOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateProfileShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetProfileArn());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares");
      return CreateProfileShareOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateWorkloadOutcome WellArchitectedClient::CreateWorkload(const CreateWorkloadRequest& request) const
{
  AWS_OPERATION_GUARD(CreateWorkload);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateWorkload, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateWorkloadOutcome>(
    [&]()-> CreateWorkloadOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateWorkload, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads");
      return CreateWorkloadOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateWorkloadShareOutcome WellArchitectedClient::CreateWorkloadShare(const CreateWorkloadShareRequest& request) const
{
  AWS_OPERATION_GUARD(CreateWorkloadShare);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateWorkloadShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("CreateWorkloadShare", "Required field: WorkloadId, is not set");
    return CreateWorkloadShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateWorkloadShareOutcome>(
    [&]()-> CreateWorkloadShareOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateWorkloadShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares");
      return CreateWorkloadShareOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteLensOutcome WellArchitectedClient::DeleteLens(const DeleteLensRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteLens);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteLens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteLens", "Required field: LensAlias, is not set");
    return DeleteLensOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  if (!request.ClientRequestTokenHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteLens", "Required field: ClientRequestToken, is not set");
    return DeleteLensOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ClientRequestToken]", false));
  }
  if (!request.LensStatusHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteLens", "Required field: LensStatus, is not set");
    return DeleteLensOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensStatus]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteLensOutcome>(
    [&]()-> DeleteLensOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteLens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lenses/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      return DeleteLensOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteLensShareOutcome WellArchitectedClient::DeleteLensShare(const DeleteLensShareRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteLensShare);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteLensShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ShareIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteLensShare", "Required field: ShareId, is not set");
    return DeleteLensShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ShareId]", false));
  }
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteLensShare", "Required field: LensAlias, is not set");
    return DeleteLensShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  if (!request.ClientRequestTokenHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteLensShare", "Required field: ClientRequestToken, is not set");
    return DeleteLensShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ClientRequestToken]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteLensShareOutcome>(
    [&]()-> DeleteLensShareOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteLensShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lenses/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetShareId());
      return DeleteLensShareOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteProfileOutcome WellArchitectedClient::DeleteProfile(const DeleteProfileRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteProfile);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ProfileArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfile", "Required field: ProfileArn, is not set");
    return DeleteProfileOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ProfileArn]", false));
  }
  if (!request.ClientRequestTokenHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfile", "Required field: ClientRequestToken, is not set");
    return DeleteProfileOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ClientRequestToken]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteProfileOutcome>(
    [&]()-> DeleteProfileOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetProfileArn());
      return DeleteProfileOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteProfileShareOutcome WellArchitectedClient::DeleteProfileShare(const DeleteProfileShareRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteProfileShare);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteProfileShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ShareIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfileShare", "Required field: ShareId, is not set");
    return DeleteProfileShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ShareId]", false));
  }
  if (!request.ProfileArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfileShare", "Required field: ProfileArn, is not set");
    return DeleteProfileShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ProfileArn]", false));
  }
  if (!request.ClientRequestTokenHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProfileShare", "Required field: ClientRequestToken, is not set");
    return DeleteProfileShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ClientRequestToken]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteProfileShareOutcome>(
    [&]()-> DeleteProfileShareOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteProfileShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetProfileArn());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetShareId());
      return DeleteProfileShareOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteWorkloadOutcome WellArchitectedClient::DeleteWorkload(const DeleteWorkloadRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteWorkload);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteWorkload, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteWorkload", "Required field: WorkloadId, is not set");
    return DeleteWorkloadOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.ClientRequestTokenHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteWorkload", "Required field: ClientRequestToken, is not set");
    return DeleteWorkloadOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ClientRequestToken]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteWorkloadOutcome>(
    [&]()-> DeleteWorkloadOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteWorkload, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      return DeleteWorkloadOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteWorkloadShareOutcome WellArchitectedClient::DeleteWorkloadShare(const DeleteWorkloadShareRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteWorkloadShare);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteWorkloadShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ShareIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteWorkloadShare", "Required field: ShareId, is not set");
    return DeleteWorkloadShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ShareId]", false));
  }
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteWorkloadShare", "Required field: WorkloadId, is not set");
    return DeleteWorkloadShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.ClientRequestTokenHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteWorkloadShare", "Required field: ClientRequestToken, is not set");
    return DeleteWorkloadShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ClientRequestToken]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteWorkloadShareOutcome>(
    [&]()-> DeleteWorkloadShareOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteWorkloadShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetShareId());
      return DeleteWorkloadShareOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DisassociateLensesOutcome WellArchitectedClient::DisassociateLenses(const DisassociateLensesRequest& request) const
{
  AWS_OPERATION_GUARD(DisassociateLenses);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DisassociateLenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DisassociateLenses", "Required field: WorkloadId, is not set");
    return DisassociateLensesOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DisassociateLensesOutcome>(
    [&]()-> DisassociateLensesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DisassociateLenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/disassociateLenses");
      return DisassociateLensesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DisassociateProfilesOutcome WellArchitectedClient::DisassociateProfiles(const DisassociateProfilesRequest& request) const
{
  AWS_OPERATION_GUARD(DisassociateProfiles);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DisassociateProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DisassociateProfiles", "Required field: WorkloadId, is not set");
    return DisassociateProfilesOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DisassociateProfilesOutcome>(
    [&]()-> DisassociateProfilesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DisassociateProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/disassociateProfiles");
      return DisassociateProfilesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ExportLensOutcome WellArchitectedClient::ExportLens(const ExportLensRequest& request) const
{
  AWS_OPERATION_GUARD(ExportLens);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ExportLens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ExportLens", "Required field: LensAlias, is not set");
    return ExportLensOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ExportLensOutcome>(
    [&]()-> ExportLensOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ExportLens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lenses/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/export");
      return ExportLensOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetAnswerOutcome WellArchitectedClient::GetAnswer(const GetAnswerRequest& request) const
{
  AWS_OPERATION_GUARD(GetAnswer);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetAnswer, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetAnswer", "Required field: WorkloadId, is not set");
    return GetAnswerOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetAnswer", "Required field: LensAlias, is not set");
    return GetAnswerOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  if (!request.QuestionIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetAnswer", "Required field: QuestionId, is not set");
    return GetAnswerOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [QuestionId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetAnswerOutcome>(
    [&]()-> GetAnswerOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetAnswer, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lensReviews/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/answers/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetQuestionId());
      return GetAnswerOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetConsolidatedReportOutcome WellArchitectedClient::GetConsolidatedReport(const GetConsolidatedReportRequest& request) const
{
  AWS_OPERATION_GUARD(GetConsolidatedReport);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetConsolidatedReport, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.FormatHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetConsolidatedReport", "Required field: Format, is not set");
    return GetConsolidatedReportOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Format]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetConsolidatedReportOutcome>(
    [&]()-> GetConsolidatedReportOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetConsolidatedReport, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/consolidatedReport");
      return GetConsolidatedReportOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLensOutcome WellArchitectedClient::GetLens(const GetLensRequest& request) const
{
  AWS_OPERATION_GUARD(GetLens);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetLens", "Required field: LensAlias, is not set");
    return GetLensOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLensOutcome>(
    [&]()-> GetLensOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lenses/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      return GetLensOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLensReviewOutcome WellArchitectedClient::GetLensReview(const GetLensReviewRequest& request) const
{
  AWS_OPERATION_GUARD(GetLensReview);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLensReview, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetLensReview", "Required field: WorkloadId, is not set");
    return GetLensReviewOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetLensReview", "Required field: LensAlias, is not set");
    return GetLensReviewOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLensReviewOutcome>(
    [&]()-> GetLensReviewOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLensReview, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lensReviews/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      return GetLensReviewOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLensReviewReportOutcome WellArchitectedClient::GetLensReviewReport(const GetLensReviewReportRequest& request) const
{
  AWS_OPERATION_GUARD(GetLensReviewReport);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLensReviewReport, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetLensReviewReport", "Required field: WorkloadId, is not set");
    return GetLensReviewReportOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetLensReviewReport", "Required field: LensAlias, is not set");
    return GetLensReviewReportOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLensReviewReportOutcome>(
    [&]()-> GetLensReviewReportOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLensReviewReport, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lensReviews/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/report");
      return GetLensReviewReportOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLensVersionDifferenceOutcome WellArchitectedClient::GetLensVersionDifference(const GetLensVersionDifferenceRequest& request) const
{
  AWS_OPERATION_GUARD(GetLensVersionDifference);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLensVersionDifference, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetLensVersionDifference", "Required field: LensAlias, is not set");
    return GetLensVersionDifferenceOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLensVersionDifferenceOutcome>(
    [&]()-> GetLensVersionDifferenceOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLensVersionDifference, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lenses/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/versionDifference");
      return GetLensVersionDifferenceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetMilestoneOutcome WellArchitectedClient::GetMilestone(const GetMilestoneRequest& request) const
{
  AWS_OPERATION_GUARD(GetMilestone);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetMilestone, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetMilestone", "Required field: WorkloadId, is not set");
    return GetMilestoneOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.MilestoneNumberHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetMilestone", "Required field: MilestoneNumber, is not set");
    return GetMilestoneOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [MilestoneNumber]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetMilestoneOutcome>(
    [&]()-> GetMilestoneOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetMilestone, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/milestones/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetMilestoneNumber());
      return GetMilestoneOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetProfileOutcome WellArchitectedClient::GetProfile(const GetProfileRequest& request) const
{
  AWS_OPERATION_GUARD(GetProfile);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ProfileArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetProfile", "Required field: ProfileArn, is not set");
    return GetProfileOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ProfileArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetProfileOutcome>(
    [&]()-> GetProfileOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetProfileArn());
      return GetProfileOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetProfileTemplateOutcome WellArchitectedClient::GetProfileTemplate(const GetProfileTemplateRequest& request) const
{
  AWS_OPERATION_GUARD(GetProfileTemplate);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetProfileTemplate, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetProfileTemplateOutcome>(
    [&]()-> GetProfileTemplateOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetProfileTemplate, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profileTemplate");
      return GetProfileTemplateOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetWorkloadOutcome WellArchitectedClient::GetWorkload(const GetWorkloadRequest& request) const
{
  AWS_OPERATION_GUARD(GetWorkload);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetWorkload, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("GetWorkload", "Required field: WorkloadId, is not set");
    return GetWorkloadOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetWorkloadOutcome>(
    [&]()-> GetWorkloadOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetWorkload, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      return GetWorkloadOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ImportLensOutcome WellArchitectedClient::ImportLens(const ImportLensRequest& request) const
{
  AWS_OPERATION_GUARD(ImportLens);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ImportLens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ImportLensOutcome>(
    [&]()-> ImportLensOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ImportLens, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/importLens");
      return ImportLensOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListAnswersOutcome WellArchitectedClient::ListAnswers(const ListAnswersRequest& request) const
{
  AWS_OPERATION_GUARD(ListAnswers);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListAnswers, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListAnswers", "Required field: WorkloadId, is not set");
    return ListAnswersOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListAnswers", "Required field: LensAlias, is not set");
    return ListAnswersOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListAnswersOutcome>(
    [&]()-> ListAnswersOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListAnswers, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lensReviews/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/answers");
      return ListAnswersOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListCheckDetailsOutcome WellArchitectedClient::ListCheckDetails(const ListCheckDetailsRequest& request) const
{
  AWS_OPERATION_GUARD(ListCheckDetails);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListCheckDetails, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListCheckDetails", "Required field: WorkloadId, is not set");
    return ListCheckDetailsOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListCheckDetailsOutcome>(
    [&]()-> ListCheckDetailsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListCheckDetails, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/checks");
      return ListCheckDetailsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListCheckSummariesOutcome WellArchitectedClient::ListCheckSummaries(const ListCheckSummariesRequest& request) const
{
  AWS_OPERATION_GUARD(ListCheckSummaries);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListCheckSummaries, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListCheckSummaries", "Required field: WorkloadId, is not set");
    return ListCheckSummariesOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListCheckSummariesOutcome>(
    [&]()-> ListCheckSummariesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListCheckSummaries, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/checkSummaries");
      return ListCheckSummariesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLensReviewImprovementsOutcome WellArchitectedClient::ListLensReviewImprovements(const ListLensReviewImprovementsRequest& request) const
{
  AWS_OPERATION_GUARD(ListLensReviewImprovements);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLensReviewImprovements, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListLensReviewImprovements", "Required field: WorkloadId, is not set");
    return ListLensReviewImprovementsOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListLensReviewImprovements", "Required field: LensAlias, is not set");
    return ListLensReviewImprovementsOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLensReviewImprovementsOutcome>(
    [&]()-> ListLensReviewImprovementsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLensReviewImprovements, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lensReviews/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/improvements");
      return ListLensReviewImprovementsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLensReviewsOutcome WellArchitectedClient::ListLensReviews(const ListLensReviewsRequest& request) const
{
  AWS_OPERATION_GUARD(ListLensReviews);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLensReviews, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListLensReviews", "Required field: WorkloadId, is not set");
    return ListLensReviewsOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLensReviewsOutcome>(
    [&]()-> ListLensReviewsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLensReviews, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lensReviews");
      return ListLensReviewsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLensSharesOutcome WellArchitectedClient::ListLensShares(const ListLensSharesRequest& request) const
{
  AWS_OPERATION_GUARD(ListLensShares);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLensShares, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListLensShares", "Required field: LensAlias, is not set");
    return ListLensSharesOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLensSharesOutcome>(
    [&]()-> ListLensSharesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLensShares, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lenses/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares");
      return ListLensSharesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListLensesOutcome WellArchitectedClient::ListLenses(const ListLensesRequest& request) const
{
  AWS_OPERATION_GUARD(ListLenses);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListLenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListLensesOutcome>(
    [&]()-> ListLensesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListLenses, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lenses");
      return ListLensesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListMilestonesOutcome WellArchitectedClient::ListMilestones(const ListMilestonesRequest& request) const
{
  AWS_OPERATION_GUARD(ListMilestones);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListMilestones, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListMilestones", "Required field: WorkloadId, is not set");
    return ListMilestonesOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListMilestonesOutcome>(
    [&]()-> ListMilestonesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListMilestones, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/milestonesSummaries");
      return ListMilestonesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListNotificationsOutcome WellArchitectedClient::ListNotifications(const ListNotificationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListNotifications);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListNotifications, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListNotificationsOutcome>(
    [&]()-> ListNotificationsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListNotifications, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/notifications");
      return ListNotificationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListProfileNotificationsOutcome WellArchitectedClient::ListProfileNotifications(const ListProfileNotificationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListProfileNotifications);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListProfileNotifications, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListProfileNotificationsOutcome>(
    [&]()-> ListProfileNotificationsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListProfileNotifications, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profileNotifications/");
      return ListProfileNotificationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListProfileSharesOutcome WellArchitectedClient::ListProfileShares(const ListProfileSharesRequest& request) const
{
  AWS_OPERATION_GUARD(ListProfileShares);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListProfileShares, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ProfileArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListProfileShares", "Required field: ProfileArn, is not set");
    return ListProfileSharesOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ProfileArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListProfileSharesOutcome>(
    [&]()-> ListProfileSharesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListProfileShares, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetProfileArn());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares");
      return ListProfileSharesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListProfilesOutcome WellArchitectedClient::ListProfiles(const ListProfilesRequest& request) const
{
  AWS_OPERATION_GUARD(ListProfiles);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListProfilesOutcome>(
    [&]()-> ListProfilesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListProfiles, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profileSummaries");
      return ListProfilesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListShareInvitationsOutcome WellArchitectedClient::ListShareInvitations(const ListShareInvitationsRequest& request) const
{
  AWS_OPERATION_GUARD(ListShareInvitations);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListShareInvitations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListShareInvitationsOutcome>(
    [&]()-> ListShareInvitationsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListShareInvitations, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shareInvitations");
      return ListShareInvitationsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListTagsForResourceOutcome WellArchitectedClient::ListTagsForResource(const ListTagsForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(ListTagsForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListTagsForResource", "Required field: WorkloadArn, is not set");
    return ListTagsForResourceOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadArn]", false));
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
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadArn());
      return ListTagsForResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListWorkloadSharesOutcome WellArchitectedClient::ListWorkloadShares(const ListWorkloadSharesRequest& request) const
{
  AWS_OPERATION_GUARD(ListWorkloadShares);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListWorkloadShares, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListWorkloadShares", "Required field: WorkloadId, is not set");
    return ListWorkloadSharesOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListWorkloadSharesOutcome>(
    [&]()-> ListWorkloadSharesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListWorkloadShares, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares");
      return ListWorkloadSharesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListWorkloadsOutcome WellArchitectedClient::ListWorkloads(const ListWorkloadsRequest& request) const
{
  AWS_OPERATION_GUARD(ListWorkloads);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListWorkloads, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListWorkloadsOutcome>(
    [&]()-> ListWorkloadsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListWorkloads, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloadsSummaries");
      return ListWorkloadsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

TagResourceOutcome WellArchitectedClient::TagResource(const TagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(TagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, TagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("TagResource", "Required field: WorkloadArn, is not set");
    return TagResourceOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadArn]", false));
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
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadArn());
      return TagResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UntagResourceOutcome WellArchitectedClient::UntagResource(const UntagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(UntagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UntagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: WorkloadArn, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadArn]", false));
  }
  if (!request.TagKeysHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: TagKeys, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [TagKeys]", false));
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
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadArn());
      return UntagResourceOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateAnswerOutcome WellArchitectedClient::UpdateAnswer(const UpdateAnswerRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateAnswer);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateAnswer, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateAnswer", "Required field: WorkloadId, is not set");
    return UpdateAnswerOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateAnswer", "Required field: LensAlias, is not set");
    return UpdateAnswerOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  if (!request.QuestionIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateAnswer", "Required field: QuestionId, is not set");
    return UpdateAnswerOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [QuestionId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateAnswerOutcome>(
    [&]()-> UpdateAnswerOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateAnswer, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lensReviews/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/answers/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetQuestionId());
      return UpdateAnswerOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateGlobalSettingsOutcome WellArchitectedClient::UpdateGlobalSettings(const UpdateGlobalSettingsRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateGlobalSettings);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateGlobalSettings, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateGlobalSettingsOutcome>(
    [&]()-> UpdateGlobalSettingsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateGlobalSettings, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/global-settings");
      return UpdateGlobalSettingsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateLensReviewOutcome WellArchitectedClient::UpdateLensReview(const UpdateLensReviewRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateLensReview);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateLensReview, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateLensReview", "Required field: WorkloadId, is not set");
    return UpdateLensReviewOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateLensReview", "Required field: LensAlias, is not set");
    return UpdateLensReviewOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateLensReviewOutcome>(
    [&]()-> UpdateLensReviewOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateLensReview, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lensReviews/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      return UpdateLensReviewOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateProfileOutcome WellArchitectedClient::UpdateProfile(const UpdateProfileRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateProfile);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ProfileArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateProfile", "Required field: ProfileArn, is not set");
    return UpdateProfileOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ProfileArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateProfileOutcome>(
    [&]()-> UpdateProfileOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateProfile, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetProfileArn());
      return UpdateProfileOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateShareInvitationOutcome WellArchitectedClient::UpdateShareInvitation(const UpdateShareInvitationRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateShareInvitation);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateShareInvitation, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ShareInvitationIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateShareInvitation", "Required field: ShareInvitationId, is not set");
    return UpdateShareInvitationOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ShareInvitationId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateShareInvitationOutcome>(
    [&]()-> UpdateShareInvitationOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateShareInvitation, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shareInvitations/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetShareInvitationId());
      return UpdateShareInvitationOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateWorkloadOutcome WellArchitectedClient::UpdateWorkload(const UpdateWorkloadRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateWorkload);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateWorkload, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateWorkload", "Required field: WorkloadId, is not set");
    return UpdateWorkloadOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateWorkloadOutcome>(
    [&]()-> UpdateWorkloadOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateWorkload, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      return UpdateWorkloadOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateWorkloadShareOutcome WellArchitectedClient::UpdateWorkloadShare(const UpdateWorkloadShareRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateWorkloadShare);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateWorkloadShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ShareIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateWorkloadShare", "Required field: ShareId, is not set");
    return UpdateWorkloadShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ShareId]", false));
  }
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateWorkloadShare", "Required field: WorkloadId, is not set");
    return UpdateWorkloadShareOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateWorkloadShareOutcome>(
    [&]()-> UpdateWorkloadShareOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateWorkloadShare, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/shares/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetShareId());
      return UpdateWorkloadShareOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PATCH, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpgradeLensReviewOutcome WellArchitectedClient::UpgradeLensReview(const UpgradeLensReviewRequest& request) const
{
  AWS_OPERATION_GUARD(UpgradeLensReview);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpgradeLensReview, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpgradeLensReview", "Required field: WorkloadId, is not set");
    return UpgradeLensReviewOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.LensAliasHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpgradeLensReview", "Required field: LensAlias, is not set");
    return UpgradeLensReviewOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [LensAlias]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpgradeLensReviewOutcome>(
    [&]()-> UpgradeLensReviewOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpgradeLensReview, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/lensReviews/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetLensAlias());
      endpointResolutionOutcome.GetResult().AddPathSegments("/upgrade");
      return UpgradeLensReviewOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpgradeProfileVersionOutcome WellArchitectedClient::UpgradeProfileVersion(const UpgradeProfileVersionRequest& request) const
{
  AWS_OPERATION_GUARD(UpgradeProfileVersion);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpgradeProfileVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.WorkloadIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpgradeProfileVersion", "Required field: WorkloadId, is not set");
    return UpgradeProfileVersionOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [WorkloadId]", false));
  }
  if (!request.ProfileArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpgradeProfileVersion", "Required field: ProfileArn, is not set");
    return UpgradeProfileVersionOutcome(Aws::Client::AWSError<WellArchitectedErrors>(WellArchitectedErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ProfileArn]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpgradeProfileVersionOutcome>(
    [&]()-> UpgradeProfileVersionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpgradeProfileVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/workloads/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetWorkloadId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profiles/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetProfileArn());
      endpointResolutionOutcome.GetResult().AddPathSegments("/upgrade");
      return UpgradeProfileVersionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

