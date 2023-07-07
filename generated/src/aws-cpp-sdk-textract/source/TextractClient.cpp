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

#include <aws/textract/TextractClient.h>
#include <aws/textract/TextractErrorMarshaller.h>
#include <aws/textract/TextractEndpointProvider.h>
#include <aws/textract/model/AnalyzeDocumentRequest.h>
#include <aws/textract/model/AnalyzeExpenseRequest.h>
#include <aws/textract/model/AnalyzeIDRequest.h>
#include <aws/textract/model/DetectDocumentTextRequest.h>
#include <aws/textract/model/GetDocumentAnalysisRequest.h>
#include <aws/textract/model/GetDocumentTextDetectionRequest.h>
#include <aws/textract/model/GetExpenseAnalysisRequest.h>
#include <aws/textract/model/GetLendingAnalysisRequest.h>
#include <aws/textract/model/GetLendingAnalysisSummaryRequest.h>
#include <aws/textract/model/StartDocumentAnalysisRequest.h>
#include <aws/textract/model/StartDocumentTextDetectionRequest.h>
#include <aws/textract/model/StartExpenseAnalysisRequest.h>
#include <aws/textract/model/StartLendingAnalysisRequest.h>

#include <smithy/tracing/TracingUtils.h>


using namespace Aws;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::Textract;
using namespace Aws::Textract::Model;
using namespace Aws::Http;
using namespace Aws::Utils::Json;
using ResolveEndpointOutcome = Aws::Endpoint::ResolveEndpointOutcome;

const char* TextractClient::SERVICE_NAME = "textract";
const char* TextractClient::ALLOCATION_TAG = "TextractClient";

TextractClient::TextractClient(const Textract::TextractClientConfiguration& clientConfiguration,
                               std::shared_ptr<TextractEndpointProviderBase> endpointProvider) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<TextractErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

TextractClient::TextractClient(const AWSCredentials& credentials,
                               std::shared_ptr<TextractEndpointProviderBase> endpointProvider,
                               const Textract::TextractClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<TextractErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

TextractClient::TextractClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                               std::shared_ptr<TextractEndpointProviderBase> endpointProvider,
                               const Textract::TextractClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<TextractErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(std::move(endpointProvider))
{
  init(m_clientConfiguration);
}

    /* Legacy constructors due deprecation */
  TextractClient::TextractClient(const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<DefaultAWSCredentialsProviderChain>(ALLOCATION_TAG),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<TextractErrorMarshaller>(ALLOCATION_TAG)),
  m_clientConfiguration(clientConfiguration),
  m_executor(clientConfiguration.executor),
  m_endpointProvider(Aws::MakeShared<TextractEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

TextractClient::TextractClient(const AWSCredentials& credentials,
                               const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             Aws::MakeShared<SimpleAWSCredentialsProvider>(ALLOCATION_TAG, credentials),
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<TextractErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<TextractEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

TextractClient::TextractClient(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                               const Client::ClientConfiguration& clientConfiguration) :
  BASECLASS(clientConfiguration,
            Aws::MakeShared<AWSAuthV4Signer>(ALLOCATION_TAG,
                                             credentialsProvider,
                                             SERVICE_NAME,
                                             Aws::Region::ComputeSignerRegion(clientConfiguration.region)),
            Aws::MakeShared<TextractErrorMarshaller>(ALLOCATION_TAG)),
    m_clientConfiguration(clientConfiguration),
    m_executor(clientConfiguration.executor),
    m_endpointProvider(Aws::MakeShared<TextractEndpointProvider>(ALLOCATION_TAG))
{
  init(m_clientConfiguration);
}

    /* End of legacy constructors due deprecation */
TextractClient::~TextractClient()
{
  ShutdownSdkClient(this, -1);
}

std::shared_ptr<TextractEndpointProviderBase>& TextractClient::accessEndpointProvider()
{
  return m_endpointProvider;
}

void TextractClient::init(const Textract::TextractClientConfiguration& config)
{
  AWSClient::SetServiceClientName("Textract");
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->InitBuiltInParameters(config);
}

void TextractClient::OverrideEndpoint(const Aws::String& endpoint)
{
  AWS_CHECK_PTR(SERVICE_NAME, m_endpointProvider);
  m_endpointProvider->OverrideEndpoint(endpoint);
}

AnalyzeDocumentOutcome TextractClient::AnalyzeDocument(const AnalyzeDocumentRequest& request) const
{
  AWS_OPERATION_GUARD(AnalyzeDocument);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AnalyzeDocument, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AnalyzeDocumentOutcome>(
    [&]()-> AnalyzeDocumentOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AnalyzeDocument, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AnalyzeDocumentOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

AnalyzeExpenseOutcome TextractClient::AnalyzeExpense(const AnalyzeExpenseRequest& request) const
{
  AWS_OPERATION_GUARD(AnalyzeExpense);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AnalyzeExpense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AnalyzeExpenseOutcome>(
    [&]()-> AnalyzeExpenseOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AnalyzeExpense, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AnalyzeExpenseOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

AnalyzeIDOutcome TextractClient::AnalyzeID(const AnalyzeIDRequest& request) const
{
  AWS_OPERATION_GUARD(AnalyzeID);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, AnalyzeID, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<AnalyzeIDOutcome>(
    [&]()-> AnalyzeIDOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, AnalyzeID, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return AnalyzeIDOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

DetectDocumentTextOutcome TextractClient::DetectDocumentText(const DetectDocumentTextRequest& request) const
{
  AWS_OPERATION_GUARD(DetectDocumentText);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, DetectDocumentText, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<DetectDocumentTextOutcome>(
    [&]()-> DetectDocumentTextOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, DetectDocumentText, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return DetectDocumentTextOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetDocumentAnalysisOutcome TextractClient::GetDocumentAnalysis(const GetDocumentAnalysisRequest& request) const
{
  AWS_OPERATION_GUARD(GetDocumentAnalysis);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetDocumentAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetDocumentAnalysisOutcome>(
    [&]()-> GetDocumentAnalysisOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetDocumentAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetDocumentAnalysisOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetDocumentTextDetectionOutcome TextractClient::GetDocumentTextDetection(const GetDocumentTextDetectionRequest& request) const
{
  AWS_OPERATION_GUARD(GetDocumentTextDetection);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetDocumentTextDetection, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetDocumentTextDetectionOutcome>(
    [&]()-> GetDocumentTextDetectionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetDocumentTextDetection, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetDocumentTextDetectionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetExpenseAnalysisOutcome TextractClient::GetExpenseAnalysis(const GetExpenseAnalysisRequest& request) const
{
  AWS_OPERATION_GUARD(GetExpenseAnalysis);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetExpenseAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetExpenseAnalysisOutcome>(
    [&]()-> GetExpenseAnalysisOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetExpenseAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetExpenseAnalysisOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLendingAnalysisOutcome TextractClient::GetLendingAnalysis(const GetLendingAnalysisRequest& request) const
{
  AWS_OPERATION_GUARD(GetLendingAnalysis);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLendingAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLendingAnalysisOutcome>(
    [&]()-> GetLendingAnalysisOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLendingAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetLendingAnalysisOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

GetLendingAnalysisSummaryOutcome TextractClient::GetLendingAnalysisSummary(const GetLendingAnalysisSummaryRequest& request) const
{
  AWS_OPERATION_GUARD(GetLendingAnalysisSummary);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, GetLendingAnalysisSummary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<GetLendingAnalysisSummaryOutcome>(
    [&]()-> GetLendingAnalysisSummaryOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, GetLendingAnalysisSummary, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return GetLendingAnalysisSummaryOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StartDocumentAnalysisOutcome TextractClient::StartDocumentAnalysis(const StartDocumentAnalysisRequest& request) const
{
  AWS_OPERATION_GUARD(StartDocumentAnalysis);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartDocumentAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartDocumentAnalysisOutcome>(
    [&]()-> StartDocumentAnalysisOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartDocumentAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return StartDocumentAnalysisOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StartDocumentTextDetectionOutcome TextractClient::StartDocumentTextDetection(const StartDocumentTextDetectionRequest& request) const
{
  AWS_OPERATION_GUARD(StartDocumentTextDetection);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartDocumentTextDetection, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartDocumentTextDetectionOutcome>(
    [&]()-> StartDocumentTextDetectionOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartDocumentTextDetection, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return StartDocumentTextDetectionOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StartExpenseAnalysisOutcome TextractClient::StartExpenseAnalysis(const StartExpenseAnalysisRequest& request) const
{
  AWS_OPERATION_GUARD(StartExpenseAnalysis);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartExpenseAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartExpenseAnalysisOutcome>(
    [&]()-> StartExpenseAnalysisOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartExpenseAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return StartExpenseAnalysisOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

StartLendingAnalysisOutcome TextractClient::StartLendingAnalysis(const StartLendingAnalysisRequest& request) const
{
  AWS_OPERATION_GUARD(StartLendingAnalysis);
  AWS_OPERATION_CHECK_PTR(m_endpointProvider, StartLendingAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE);
  return smithy::components::tracing::TracingUtils::MakeCallWithTiming<StartLendingAnalysisOutcome>(
    [&]()-> StartLendingAnalysisOutcome {
      auto endpointResolutionOutcome = smithy::components::tracing::TracingUtils::MakeCallWithTiming<ResolveEndpointOutcome>(
          [&]() -> ResolveEndpointOutcome { return m_endpointProvider->ResolveEndpoint(request.GetEndpointContextParams()); },
          "smithy.client.resolve_endpoint_duration",
          m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
          {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
      AWS_OPERATION_CHECK_SUCCESS(endpointResolutionOutcome, StartLendingAnalysis, CoreErrors, CoreErrors::ENDPOINT_RESOLUTION_FAILURE, endpointResolutionOutcome.GetError().GetMessage());
      return StartLendingAnalysisOutcome(MakeRequest(request, endpointResolutionOutcome.GetResult(), Aws::Http::HttpMethod::HTTP_POST, Aws::Auth::SIGV4_SIGNER));
    },
    "smithy.client.duration",
    m_telemetryProvider->getMeter(this->GetServiceClientName(), {}),
    {{"rpc.method", request.GetServiceRequestName()}, {"rpc.service", this->GetServiceClientName()}});
}

