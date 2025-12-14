// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "VideoPlayerController.h"
#include "VideoAPIServer.generated.h"

UCLASS()
class VRMOVIEPLAYER_API AVideoAPIServer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVideoAPIServer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Server port
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
	int32 ServerPort;

	// Reference to video player controller
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
	AVideoPlayerController* VideoPlayerController;

	// Start the HTTP server
	UFUNCTION(BlueprintCallable, Category = "API")
	bool StartServer();

	// Stop the HTTP server
	UFUNCTION(BlueprintCallable, Category = "API")
	void StopServer();

private:
	// HTTP Router
	TSharedPtr<IHttpRouter> HttpRouter;

	// Server handle
	FHttpServerModule* HttpServerModule;

	// Route handles for cleanup
	FHttpRouteHandle FilenameRouteHandle;
	FHttpRouteHandle StateRouteHandle;
	FHttpRouteHandle TimestampRouteHandle;
	FHttpRouteHandle DurationRouteHandle;

	// Route handlers
	bool HandleGetFilename(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	bool HandleGetState(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	bool HandleGetTimestamp(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	bool HandleGetDuration(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	// Helper to create JSON response
	TUniquePtr<FHttpServerResponse> CreateJsonResponse(const FString& JsonContent, EHttpServerResponseCodes ResponseCode = EHttpServerResponseCodes::Ok);
};
