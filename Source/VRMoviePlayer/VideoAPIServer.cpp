// Copyright Epic Games, Inc. All Rights Reserved.

#include "VideoAPIServer.h"
#include "HttpServerModule.h"
#include "HttpServerConstants.h"
#include "IHttpRouter.h"
#include "HttpPath.h"
#include "Json.h"
#include "JsonUtilities.h"

AVideoAPIServer::AVideoAPIServer()
{
	PrimaryActorTick.bCanEverTick = false;
	ServerPort = 8080;
	VideoPlayerController = nullptr;
	HttpServerModule = nullptr;
}

void AVideoAPIServer::BeginPlay()
{
	Super::BeginPlay();
	StartServer();
}

void AVideoAPIServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopServer();
	Super::EndPlay(EndPlayReason);
}

void AVideoAPIServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AVideoAPIServer::StartServer()
{
	if (!FModuleManager::Get().IsModuleLoaded("HTTPServer"))
	{
		FModuleManager::Get().LoadModule("HTTPServer");
	}

	HttpServerModule = &FHttpServerModule::Get();

	if (!HttpServerModule)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get HTTP Server Module"));
		return false;
	}

	// Create HTTP router
	HttpRouter = HttpServerModule->GetHttpRouter(ServerPort);

	if (!HttpRouter.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create HTTP Router on port %d"), ServerPort);
		return false;
	}

	// Bind routes
	HttpRouter->BindRoute(
		FHttpPath(TEXT("/api/filename")),
		EHttpServerRequestVerbs::VERB_GET,
		[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
		{
			return HandleGetFilename(Request, OnComplete);
		}
	);

	HttpRouter->BindRoute(
		FHttpPath(TEXT("/api/state")),
		EHttpServerRequestVerbs::VERB_GET,
		[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
		{
			return HandleGetState(Request, OnComplete);
		}
	);

	HttpRouter->BindRoute(
		FHttpPath(TEXT("/api/timestamp")),
		EHttpServerRequestVerbs::VERB_GET,
		[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
		{
			return HandleGetTimestamp(Request, OnComplete);
		}
	);

	HttpRouter->BindRoute(
		FHttpPath(TEXT("/api/duration")),
		EHttpServerRequestVerbs::VERB_GET,
		[this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
		{
			return HandleGetDuration(Request, OnComplete);
		}
	);

	// Start listening
	HttpServerModule->StartAllListeners();

	UE_LOG(LogTemp, Log, TEXT("REST API Server started on port %d"), ServerPort);
	UE_LOG(LogTemp, Log, TEXT("Available endpoints:"));
	UE_LOG(LogTemp, Log, TEXT("  GET /api/filename   - Get current video filename"));
	UE_LOG(LogTemp, Log, TEXT("  GET /api/state      - Get player state"));
	UE_LOG(LogTemp, Log, TEXT("  GET /api/timestamp  - Get current timestamp"));
	UE_LOG(LogTemp, Log, TEXT("  GET /api/duration   - Get total video duration"));

	return true;
}

void AVideoAPIServer::StopServer()
{
	if (HttpServerModule)
	{
		HttpServerModule->StopAllListeners();
		UE_LOG(LogTemp, Log, TEXT("REST API Server stopped"));
	}

	if (HttpRouter.IsValid())
	{
		HttpRouter->UnbindRoute(FHttpPath(TEXT("/api/filename")), EHttpServerRequestVerbs::VERB_GET);
		HttpRouter->UnbindRoute(FHttpPath(TEXT("/api/state")), EHttpServerRequestVerbs::VERB_GET);
		HttpRouter->UnbindRoute(FHttpPath(TEXT("/api/timestamp")), EHttpServerRequestVerbs::VERB_GET);
		HttpRouter->UnbindRoute(FHttpPath(TEXT("/api/duration")), EHttpServerRequestVerbs::VERB_GET);
		HttpRouter.Reset();
	}
}

bool AVideoAPIServer::HandleGetFilename(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	if (!VideoPlayerController)
	{
		FString ErrorJson = TEXT("{\"error\": \"Video player not initialized\"}");
		OnComplete(CreateJsonResponse(ErrorJson, EHttpServerResponseCodes::ServerError));
		return true;
	}

	FString Filename = VideoPlayerController->GetVideoFilename();
	FString JsonResponse = FString::Printf(TEXT("{\"filename\": \"%s\"}"), *Filename);

	OnComplete(CreateJsonResponse(JsonResponse));
	return true;
}

bool AVideoAPIServer::HandleGetState(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	if (!VideoPlayerController)
	{
		FString ErrorJson = TEXT("{\"error\": \"Video player not initialized\"}");
		OnComplete(CreateJsonResponse(ErrorJson, EHttpServerResponseCodes::ServerError));
		return true;
	}

	FString State = VideoPlayerController->GetPlayerStateString();
	FString JsonResponse = FString::Printf(TEXT("{\"state\": \"%s\"}"), *State);

	OnComplete(CreateJsonResponse(JsonResponse));
	return true;
}

bool AVideoAPIServer::HandleGetTimestamp(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	if (!VideoPlayerController)
	{
		FString ErrorJson = TEXT("{\"error\": \"Video player not initialized\"}");
		OnComplete(CreateJsonResponse(ErrorJson, EHttpServerResponseCodes::ServerError));
		return true;
	}

	float Timestamp = VideoPlayerController->GetCurrentTimestamp();
	FString JsonResponse = FString::Printf(TEXT("{\"timestamp\": %.2f}"), Timestamp);

	OnComplete(CreateJsonResponse(JsonResponse));
	return true;
}

bool AVideoAPIServer::HandleGetDuration(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	if (!VideoPlayerController)
	{
		FString ErrorJson = TEXT("{\"error\": \"Video player not initialized\"}");
		OnComplete(CreateJsonResponse(ErrorJson, EHttpServerResponseCodes::ServerError));
		return true;
	}

	float Duration = VideoPlayerController->GetTotalDuration();
	FString JsonResponse = FString::Printf(TEXT("{\"duration\": %.2f}"), Duration);

	OnComplete(CreateJsonResponse(JsonResponse));
	return true;
}

TUniquePtr<FHttpServerResponse> AVideoAPIServer::CreateJsonResponse(const FString& JsonContent, EHttpServerResponseCodes ResponseCode)
{
	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonContent, TEXT("application/json"));
	Response->Code = ResponseCode;
	return Response;
}
