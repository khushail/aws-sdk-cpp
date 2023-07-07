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

#include <aws/databrew/GlueDataBrewClient.h>
#include <aws/databrew/GlueDataBrewErrorMarshaller.h>
#include <aws/databrew/GlueDataBrewEndpointProvider.h>
#include <aws/databrew/model/BatchDeleteRecipeVersionRequest.h>
#include <aws/databrew/model/CreateDatasetRequest.h>
#include <aws/databrew/model/CreateProfileJobRequest.h>
#include <aws/databrew/model/CreateProjectRequest.h>
#include <aws/databrew/model/CreateRecipeRequest.h>
#include <aws/databrew/model/CreateRecipeJobRequest.h>
#include <aws/databrew/model/CreateRulesetRequest.h>
#include <aws/databrew/model/CreateScheduleRequest.h>
#include <aws/databrew/model/DeleteDatasetRequest.h>
#include <aws/databrew/model/DeleteJobRequest.h>
#include <aws/databrew/model/DeleteProjectRequest.h>
#include <aws/databrew/model/DeleteRecipeVersionRequest.h>
#include <aws/databrew/model/DeleteRulesetRequest.h>
#include <aws/databrew/model/DeleteScheduleRequest.h>
#include <aws/databrew/model/DescribeDatasetRequest.h>
#include <aws/databrew/model/DescribeJobRequest.h>
#include <aws/databrew/model/DescribeJobRunRequest.h>
#include <aws/databrew/model/DescribeProjectRequest.h>
#include <aws/databrew/model/DescribeRecipeRequest.h>
#include <aws/databrew/model/DescribeRulesetRequest.h>
#include <aws/databrew/model/DescribeScheduleRequest.h>
#include <aws/databrew/model/ListDatasetsRequest.h>
#include <aws/databrew/model/ListJobRunsRequest.h>
#include <aws/databrew/model/ListJobsRequest.h>
#include <aws/databrew/model/ListProjectsRequest.h>
#include <aws/databrew/model/ListRecipeVersionsRequest.h>
#include <aws/databrew/model/ListRecipesRequest.h>
#include <aws/databrew/model/ListRulesetsRequest.h>
#include <aws/databrew/model/ListSchedulesRequest.h>
#include <aws/databrew/model/ListTagsForResourceRequest.h>
#include <aws/databrew/model/PublishRecipeRequest.h>
#include <aws/databrew/model/SendProjectSessionActionRequest.h>
#include <aws/databrew/model/StartJobRunRequest.h>
#include <aws/databrew/model/StartProjectSessionRequest.h>
#include <aws/databrew/model/StopJobRunRequest.h>
#include <aws/databrew/model/TagResourceRequest.h>
#include <aws/databrew/model/UntagResourceRequest.h>
#include <aws/databrew/model/UpdateDatasetRequest.h>
#include <aws/databrew/model/UpdateProfileJobRequest.h>
#include <aws/databrew/model/UpdateProjectRequest.h>
#include <aws/databrew/model/UpdateRecipeRequest.h>
#include <aws/databrew/model/UpdateRecipeJobRequest.h>
#include <aws/databrew/model/UpdateRulesetRequest.h>
#include <aws/databrew/model/UpdateScheduleRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::GlueDataBrew;
using namespace Aws::GlueDataBrew::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* GlueDataBrewClient::SERVICE_NAME = "databrew";
const char* GlueDataBrewClient::ALLOCATION_TAG = "GlueDataBrewClient";

GlueDataBrewClient::GlueDataBrewClient(const GlueDataBrew::GlueDataBrewClientConfiguration& clientConfiguration,
                                       std::shared_ptr<GlueDataBrewEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<GlueDataBrewErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

GlueDataBrewClient::GlueDataBrewClient(const AWSCredentials& credentials,
                                       std::shared_ptr<GlueDataBrewEndpointProviderBase> endpointProvider,
                                       const GlueDataBrew::GlueDataBrewClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<GlueDataBrewErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

GlueDataBrewClient::GlueDataBrewClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                       std::shared_ptr<GlueDataBrewEndpointProviderBase> endpointProvider,
                                       const GlueDataBrew::GlueDataBrewClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<GlueDataBrewErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  GlueDataBrewClient::GlueDataBrewClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<GlueDataBrewErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<GlueDataBrewEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

GlueDataBrewClient::GlueDataBrewClient(const AWSCredentials& credentials,
                                       const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<GlueDataBrewErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<GlueDataBrewEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

GlueDataBrewClient::GlueDataBrewClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                                       const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<GlueDataBrewErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<GlueDataBrewEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
GlueDataBrewClient::~GlueDataBrewClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<GlueDataBrewEndpointProviderBase>& GlueDataBrewClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void GlueDataBrewClient::init(const GlueDataBrew::GlueDataBrewClientConfiguration& config)
{
  AWSClient::SetServiceClientName("DataBrew");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void GlueDataBrewClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

BatchDeleteRecipeVersionOutcome GlueDataBrewClient::BatchDeleteRecipeVersion(const BatchDeleteRecipeVersionRequest& request) const
{
  AWS_OPERATION_GUARD(BatchDeleteRecipeVersion);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, BatchDeleteRecipeVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("BatchDeleteRecipeVersion", "Required field: Name, is not set");
    return BatchDeleteRecipeVersionOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<BatchDeleteRecipeVersionOutcome>(
    [&]()-> BatchDeleteRecipeVersionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, BatchDeleteRecipeVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/batchDeleteRecipeVersion");
      return BatchDeleteRecipeVersionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateDatasetOutcome GlueDataBrewClient::CreateDataset(const CreateDatasetRequest& request) const
{
  AWS_OPERATION_GUARD(CreateDataset);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateDataset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateDatasetOutcome>(
    [&]()-> CreateDatasetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateDataset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/datasets");
      return CreateDatasetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateProfileJobOutcome GlueDataBrewClient::CreateProfileJob(const CreateProfileJobRequest& request) const
{
  AWS_OPERATION_GUARD(CreateProfileJob);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateProfileJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateProfileJobOutcome>(
    [&]()-> CreateProfileJobOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateProfileJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profileJobs");
      return CreateProfileJobOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateProjectOutcome GlueDataBrewClient::CreateProject(const CreateProjectRequest& request) const
{
  AWS_OPERATION_GUARD(CreateProject);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateProject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateProjectOutcome>(
    [&]()-> CreateProjectOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateProject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/projects");
      return CreateProjectOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateRecipeOutcome GlueDataBrewClient::CreateRecipe(const CreateRecipeRequest& request) const
{
  AWS_OPERATION_GUARD(CreateRecipe);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateRecipe, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateRecipeOutcome>(
    [&]()-> CreateRecipeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateRecipe, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipes");
      return CreateRecipeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateRecipeJobOutcome GlueDataBrewClient::CreateRecipeJob(const CreateRecipeJobRequest& request) const
{
  AWS_OPERATION_GUARD(CreateRecipeJob);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateRecipeJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateRecipeJobOutcome>(
    [&]()-> CreateRecipeJobOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateRecipeJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipeJobs");
      return CreateRecipeJobOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateRulesetOutcome GlueDataBrewClient::CreateRuleset(const CreateRulesetRequest& request) const
{
  AWS_OPERATION_GUARD(CreateRuleset);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateRuleset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateRulesetOutcome>(
    [&]()-> CreateRulesetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateRuleset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/rulesets");
      return CreateRulesetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

CreateScheduleOutcome GlueDataBrewClient::CreateSchedule(const CreateScheduleRequest& request) const
{
  AWS_OPERATION_GUARD(CreateSchedule);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, CreateSchedule, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<CreateScheduleOutcome>(
    [&]()-> CreateScheduleOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, CreateSchedule, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/schedules");
      return CreateScheduleOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteDatasetOutcome GlueDataBrewClient::DeleteDataset(const DeleteDatasetRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteDataset);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteDataset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteDataset", "Required field: Name, is not set");
    return DeleteDatasetOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteDatasetOutcome>(
    [&]()-> DeleteDatasetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteDataset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/datasets/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DeleteDatasetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteJobOutcome GlueDataBrewClient::DeleteJob(const DeleteJobRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteJob);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteJob", "Required field: Name, is not set");
    return DeleteJobOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteJobOutcome>(
    [&]()-> DeleteJobOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobs/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DeleteJobOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteProjectOutcome GlueDataBrewClient::DeleteProject(const DeleteProjectRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteProject);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteProject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteProject", "Required field: Name, is not set");
    return DeleteProjectOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteProjectOutcome>(
    [&]()-> DeleteProjectOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteProject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/projects/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DeleteProjectOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteRecipeVersionOutcome GlueDataBrewClient::DeleteRecipeVersion(const DeleteRecipeVersionRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteRecipeVersion);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteRecipeVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteRecipeVersion", "Required field: Name, is not set");
    return DeleteRecipeVersionOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  if (!request.RecipeVersionHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteRecipeVersion", "Required field: RecipeVersion, is not set");
    return DeleteRecipeVersionOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [RecipeVersion]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteRecipeVersionOutcome>(
    [&]()-> DeleteRecipeVersionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteRecipeVersion, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipeVersion/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetRecipeVersion());
      return DeleteRecipeVersionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteRulesetOutcome GlueDataBrewClient::DeleteRuleset(const DeleteRulesetRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteRuleset);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteRuleset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteRuleset", "Required field: Name, is not set");
    return DeleteRulesetOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteRulesetOutcome>(
    [&]()-> DeleteRulesetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteRuleset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/rulesets/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DeleteRulesetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DeleteScheduleOutcome GlueDataBrewClient::DeleteSchedule(const DeleteScheduleRequest& request) const
{
  AWS_OPERATION_GUARD(DeleteSchedule);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DeleteSchedule, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DeleteSchedule", "Required field: Name, is not set");
    return DeleteScheduleOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DeleteScheduleOutcome>(
    [&]()-> DeleteScheduleOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DeleteSchedule, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/schedules/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DeleteScheduleOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_DELETE, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeDatasetOutcome GlueDataBrewClient::DescribeDataset(const DescribeDatasetRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeDataset);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeDataset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeDataset", "Required field: Name, is not set");
    return DescribeDatasetOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeDatasetOutcome>(
    [&]()-> DescribeDatasetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeDataset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/datasets/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DescribeDatasetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeJobOutcome GlueDataBrewClient::DescribeJob(const DescribeJobRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeJob);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeJob", "Required field: Name, is not set");
    return DescribeJobOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeJobOutcome>(
    [&]()-> DescribeJobOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobs/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DescribeJobOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeJobRunOutcome GlueDataBrewClient::DescribeJobRun(const DescribeJobRunRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeJobRun);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeJobRun, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeJobRun", "Required field: Name, is not set");
    return DescribeJobRunOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  if (!request.RunIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeJobRun", "Required field: RunId, is not set");
    return DescribeJobRunOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [RunId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeJobRunOutcome>(
    [&]()-> DescribeJobRunOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeJobRun, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobs/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobRun/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetRunId());
      return DescribeJobRunOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeProjectOutcome GlueDataBrewClient::DescribeProject(const DescribeProjectRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeProject);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeProject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeProject", "Required field: Name, is not set");
    return DescribeProjectOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeProjectOutcome>(
    [&]()-> DescribeProjectOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeProject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/projects/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DescribeProjectOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeRecipeOutcome GlueDataBrewClient::DescribeRecipe(const DescribeRecipeRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeRecipe);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeRecipe, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeRecipe", "Required field: Name, is not set");
    return DescribeRecipeOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeRecipeOutcome>(
    [&]()-> DescribeRecipeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeRecipe, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DescribeRecipeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeRulesetOutcome GlueDataBrewClient::DescribeRuleset(const DescribeRulesetRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeRuleset);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeRuleset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeRuleset", "Required field: Name, is not set");
    return DescribeRulesetOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeRulesetOutcome>(
    [&]()-> DescribeRulesetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeRuleset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/rulesets/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DescribeRulesetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DescribeScheduleOutcome GlueDataBrewClient::DescribeSchedule(const DescribeScheduleRequest& request) const
{
  AWS_OPERATION_GUARD(DescribeSchedule);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DescribeSchedule, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("DescribeSchedule", "Required field: Name, is not set");
    return DescribeScheduleOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DescribeScheduleOutcome>(
    [&]()-> DescribeScheduleOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DescribeSchedule, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/schedules/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return DescribeScheduleOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListDatasetsOutcome GlueDataBrewClient::ListDatasets(const ListDatasetsRequest& request) const
{
  AWS_OPERATION_GUARD(ListDatasets);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListDatasets, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListDatasetsOutcome>(
    [&]()-> ListDatasetsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListDatasets, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/datasets");
      return ListDatasetsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListJobRunsOutcome GlueDataBrewClient::ListJobRuns(const ListJobRunsRequest& request) const
{
  AWS_OPERATION_GUARD(ListJobRuns);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListJobRuns, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListJobRuns", "Required field: Name, is not set");
    return ListJobRunsOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListJobRunsOutcome>(
    [&]()-> ListJobRunsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListJobRuns, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobs/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobRuns");
      return ListJobRunsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListJobsOutcome GlueDataBrewClient::ListJobs(const ListJobsRequest& request) const
{
  AWS_OPERATION_GUARD(ListJobs);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListJobs, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListJobsOutcome>(
    [&]()-> ListJobsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListJobs, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobs");
      return ListJobsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListProjectsOutcome GlueDataBrewClient::ListProjects(const ListProjectsRequest& request) const
{
  AWS_OPERATION_GUARD(ListProjects);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListProjects, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListProjectsOutcome>(
    [&]()-> ListProjectsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListProjects, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/projects");
      return ListProjectsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListRecipeVersionsOutcome GlueDataBrewClient::ListRecipeVersions(const ListRecipeVersionsRequest& request) const
{
  AWS_OPERATION_GUARD(ListRecipeVersions);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListRecipeVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListRecipeVersions", "Required field: Name, is not set");
    return ListRecipeVersionsOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListRecipeVersionsOutcome>(
    [&]()-> ListRecipeVersionsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListRecipeVersions, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipeVersions");
      return ListRecipeVersionsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListRecipesOutcome GlueDataBrewClient::ListRecipes(const ListRecipesRequest& request) const
{
  AWS_OPERATION_GUARD(ListRecipes);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListRecipes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListRecipesOutcome>(
    [&]()-> ListRecipesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListRecipes, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipes");
      return ListRecipesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListRulesetsOutcome GlueDataBrewClient::ListRulesets(const ListRulesetsRequest& request) const
{
  AWS_OPERATION_GUARD(ListRulesets);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListRulesets, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListRulesetsOutcome>(
    [&]()-> ListRulesetsOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListRulesets, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/rulesets");
      return ListRulesetsOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListSchedulesOutcome GlueDataBrewClient::ListSchedules(const ListSchedulesRequest& request) const
{
  AWS_OPERATION_GUARD(ListSchedules);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListSchedules, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<ListSchedulesOutcome>(
    [&]()-> ListSchedulesOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, ListSchedules, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/schedules");
      return ListSchedulesOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_GET, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

ListTagsForResourceOutcome GlueDataBrewClient::ListTagsForResource(const ListTagsForResourceRequest& request) const
{
  AWS_OPERATION_GUARD(ListTagsForResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, ListTagsForResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("ListTagsForResource", "Required field: ResourceArn, is not set");
    return ListTagsForResourceOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
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

PublishRecipeOutcome GlueDataBrewClient::PublishRecipe(const PublishRecipeRequest& request) const
{
  AWS_OPERATION_GUARD(PublishRecipe);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, PublishRecipe, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("PublishRecipe", "Required field: Name, is not set");
    return PublishRecipeOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<PublishRecipeOutcome>(
    [&]()-> PublishRecipeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, PublishRecipe, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/publishRecipe");
      return PublishRecipeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

SendProjectSessionActionOutcome GlueDataBrewClient::SendProjectSessionAction(const SendProjectSessionActionRequest& request) const
{
  AWS_OPERATION_GUARD(SendProjectSessionAction);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, SendProjectSessionAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("SendProjectSessionAction", "Required field: Name, is not set");
    return SendProjectSessionActionOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<SendProjectSessionActionOutcome>(
    [&]()-> SendProjectSessionActionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, SendProjectSessionAction, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/projects/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/sendProjectSessionAction");
      return SendProjectSessionActionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StartJobRunOutcome GlueDataBrewClient::StartJobRun(const StartJobRunRequest& request) const
{
  AWS_OPERATION_GUARD(StartJobRun);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartJobRun, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("StartJobRun", "Required field: Name, is not set");
    return StartJobRunOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartJobRunOutcome>(
    [&]()-> StartJobRunOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartJobRun, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobs/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/startJobRun");
      return StartJobRunOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StartProjectSessionOutcome GlueDataBrewClient::StartProjectSession(const StartProjectSessionRequest& request) const
{
  AWS_OPERATION_GUARD(StartProjectSession);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartProjectSession, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("StartProjectSession", "Required field: Name, is not set");
    return StartProjectSessionOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartProjectSessionOutcome>(
    [&]()-> StartProjectSessionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartProjectSession, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/projects/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/startProjectSession");
      return StartProjectSessionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StopJobRunOutcome GlueDataBrewClient::StopJobRun(const StopJobRunRequest& request) const
{
  AWS_OPERATION_GUARD(StopJobRun);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StopJobRun, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("StopJobRun", "Required field: Name, is not set");
    return StopJobRunOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  if (!request.RunIdHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("StopJobRun", "Required field: RunId, is not set");
    return StopJobRunOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [RunId]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StopJobRunOutcome>(
    [&]()-> StopJobRunOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StopJobRun, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobs/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      endpointResolutionOutcome.GetResult().AddPathSegments("/jobRun/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetRunId());
      endpointResolutionOutcome.GetResult().AddPathSegments("/stopJobRun");
      return StopJobRunOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

TagResourceOutcome GlueDataBrewClient::TagResource(const TagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(TagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, TagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("TagResource", "Required field: ResourceArn, is not set");
    return TagResourceOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
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

UntagResourceOutcome GlueDataBrewClient::UntagResource(const UntagResourceRequest& request) const
{
  AWS_OPERATION_GUARD(UntagResource);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UntagResource, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.ResourceArnHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: ResourceArn, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [ResourceArn]", false));
  }
  if (!request.TagKeysHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UntagResource", "Required field: TagKeys, is not set");
    return UntagResourceOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [TagKeys]", false));
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

UpdateDatasetOutcome GlueDataBrewClient::UpdateDataset(const UpdateDatasetRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateDataset);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateDataset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateDataset", "Required field: Name, is not set");
    return UpdateDatasetOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateDatasetOutcome>(
    [&]()-> UpdateDatasetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateDataset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/datasets/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return UpdateDatasetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateProfileJobOutcome GlueDataBrewClient::UpdateProfileJob(const UpdateProfileJobRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateProfileJob);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateProfileJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateProfileJob", "Required field: Name, is not set");
    return UpdateProfileJobOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateProfileJobOutcome>(
    [&]()-> UpdateProfileJobOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateProfileJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/profileJobs/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return UpdateProfileJobOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateProjectOutcome GlueDataBrewClient::UpdateProject(const UpdateProjectRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateProject);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateProject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateProject", "Required field: Name, is not set");
    return UpdateProjectOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateProjectOutcome>(
    [&]()-> UpdateProjectOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateProject, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/projects/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return UpdateProjectOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateRecipeOutcome GlueDataBrewClient::UpdateRecipe(const UpdateRecipeRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateRecipe);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateRecipe, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateRecipe", "Required field: Name, is not set");
    return UpdateRecipeOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateRecipeOutcome>(
    [&]()-> UpdateRecipeOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateRecipe, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipes/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return UpdateRecipeOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateRecipeJobOutcome GlueDataBrewClient::UpdateRecipeJob(const UpdateRecipeJobRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateRecipeJob);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateRecipeJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateRecipeJob", "Required field: Name, is not set");
    return UpdateRecipeJobOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateRecipeJobOutcome>(
    [&]()-> UpdateRecipeJobOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateRecipeJob, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/recipeJobs/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return UpdateRecipeJobOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateRulesetOutcome GlueDataBrewClient::UpdateRuleset(const UpdateRulesetRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateRuleset);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateRuleset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateRuleset", "Required field: Name, is not set");
    return UpdateRulesetOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateRulesetOutcome>(
    [&]()-> UpdateRulesetOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateRuleset, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/rulesets/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return UpdateRulesetOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

UpdateScheduleOutcome GlueDataBrewClient::UpdateSchedule(const UpdateScheduleRequest& request) const
{
  AWS_OPERATION_GUARD(UpdateSchedule);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, UpdateSchedule, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  if (!request.NameHasBeenSet())
  {
    AWS_LOGSTREAM_ERROR("UpdateSchedule", "Required field: Name, is not set");
    return UpdateScheduleOutcome(Aws::Client::AWSError<GlueDataBrewErrors>(GlueDataBrewErrors::MISSING_PARAMETER, "MISSING_PARAMETER", "Missing required field [Name]", false));
  }
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<UpdateScheduleOutcome>(
    [&]()-> UpdateScheduleOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, UpdateSchedule, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      endpointResolutionOutcome.GetResult().AddPathSegments("/schedules/");
      endpointResolutionOutcome.GetResult().AddPathSegment(request.GetName());
      return UpdateScheduleOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_PUT, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

