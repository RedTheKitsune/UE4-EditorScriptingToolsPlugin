//==========================================================================//
// Copyright Elhoussine Mehnik (ue4resources@gmail.com). All Rights Reserved.
//================== http://unrealengineresources.com/ =====================//


#include "EditorEventsSubsystem.h"

#include "Editor.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "EdMode.h"
#include "EditorModeRegistry.h"
#include "Editor/EditorEngine.h"
#include "EditorActorFolders.h"
#include "Framework/Application/SlateApplication.h"
#include "UObject/ObjectSaveContext.h"
#include "EditorModeManager.h"

#include "EditorUserDefinedActions.h"
#include "EditorUserDefinedCommands.h"
#include "EditorScriptingToolsUtils.h"
#include "LevelEditingViewportUtils.h"
#include "UObject/ObjectSaveContext.h"



UEditorEventsSubsystem::UEditorEventsSubsystem()
{

}


void UEditorEventsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//
	{
		if (!IsRunningCommandlet())
		{
			GLevelEditorModeTools().OnEditorModeIDChanged().AddUObject(this, &UEditorEventsSubsystem::HandleEditorModeChanged);
		}
		FEditorDelegates::OnMapOpened.AddUObject(this, &UEditorEventsSubsystem::HandleMapOpened);
		FEditorDelegates::OnEditorCameraMoved.AddUObject(this, &UEditorEventsSubsystem::HandleEditorCameraMoved);
		FEditorDelegates::OnDollyPerspectiveCamera.AddUObject(this, &UEditorEventsSubsystem::HandleDollyPerspectiveCamera);
		FEditorDelegates::PreBeginPIE.AddUObject(this, &UEditorEventsSubsystem::HandlePreBeginPIE);
		FEditorDelegates::BeginPIE.AddUObject(this, &UEditorEventsSubsystem::HandleBeginPIE);
		FEditorDelegates::PostPIEStarted.AddUObject(this, &UEditorEventsSubsystem::HandlePostPIEStarted);
		FEditorDelegates::PrePIEEnded.AddUObject(this, &UEditorEventsSubsystem::HandlePrePIEEnded);
		FEditorDelegates::EndPIE.AddUObject(this, &UEditorEventsSubsystem::HandleEndPIE);
		FEditorDelegates::PausePIE.AddUObject(this, &UEditorEventsSubsystem::HandlePausePIE);
		FEditorDelegates::ResumePIE.AddUObject(this, &UEditorEventsSubsystem::HandleResumePIE);
		FEditorDelegates::SingleStepPIE.AddUObject(this, &UEditorEventsSubsystem::HandleSingleStepPIE);
		FEditorDelegates::OnPreSwitchBeginPIEAndSIE.AddUObject(this, &UEditorEventsSubsystem::HandlePreSwitchBeginPIEAndSIE);
		FEditorDelegates::OnSwitchBeginPIEAndSIE.AddUObject(this, &UEditorEventsSubsystem::HandleSwitchBeginPIEAndSIE);
		FEditorDelegates::OnAssetsPreDelete.AddUObject(this, &UEditorEventsSubsystem::HandleAssetsPreDelete);
		FEditorDelegates::PreSaveWorldWithContext.AddUObject(this, &UEditorEventsSubsystem::HandleWorldPreSaved);
		FEditorDelegates::PostSaveWorldWithContext.AddUObject(this, &UEditorEventsSubsystem::HandleWorldPostSaved);
	}

	{
		FActorFolders::OnFolderCreated.AddUObject(this, &UEditorEventsSubsystem::HandleFolderCreated);
		FActorFolders::OnFolderMoved.AddUObject(this, &UEditorEventsSubsystem::HandleFolderMoved);
		FActorFolders::OnFolderDeleted.AddUObject(this, &UEditorEventsSubsystem::HandleFolderDeleted);
	}


	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.OnActorSelectionChanged().AddUObject(this, &UEditorEventsSubsystem::HandleActorSelectionChanged);
		LevelEditorModule.OnMapChanged().AddUObject(this, &UEditorEventsSubsystem::HandleMapChanged);
	}

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationPreInputKeyDownListener().AddUObject(this, &UEditorEventsSubsystem::HandleApplicationPreInputKeyDownListener);
		FSlateApplication::Get().OnApplicationMousePreInputButtonDownListener().AddUObject(this, &UEditorEventsSubsystem::HandleApplicationMousePreInputButtonDownListener);
	}

	{
		FEditorUserDefinedCommands::OnExecuteActionDelegate.AddUObject(this, &UEditorEventsSubsystem::HandleActionExecuted);
	}
}

void UEditorEventsSubsystem::Deinitialize()
{
	//
	{
		if (!IsRunningCommandlet())
		{
			GLevelEditorModeTools().OnEditorModeIDChanged().RemoveAll(this);
		}
		FEditorDelegates::OnMapOpened.RemoveAll(this);
		FEditorDelegates::OnEditorCameraMoved.RemoveAll(this);
		FEditorDelegates::OnDollyPerspectiveCamera.RemoveAll(this);
		FEditorDelegates::PreBeginPIE.RemoveAll(this);
		FEditorDelegates::BeginPIE.RemoveAll(this);
		FEditorDelegates::PostPIEStarted.RemoveAll(this);
		FEditorDelegates::PrePIEEnded.RemoveAll(this);
		FEditorDelegates::EndPIE.RemoveAll(this);
		FEditorDelegates::PausePIE.RemoveAll(this);
		FEditorDelegates::ResumePIE.RemoveAll(this);
		FEditorDelegates::SingleStepPIE.RemoveAll(this);
		FEditorDelegates::OnPreSwitchBeginPIEAndSIE.RemoveAll(this);
		FEditorDelegates::OnSwitchBeginPIEAndSIE.RemoveAll(this);
		FEditorDelegates::OnAssetsPreDelete.RemoveAll(this);
		FEditorDelegates::PreSaveWorldWithContext.RemoveAll(this);
		FEditorDelegates::PostSaveWorldWithContext.RemoveAll(this);
	}

	{
		FActorFolders::OnFolderCreated.RemoveAll(this);
		FActorFolders::OnFolderMoved.RemoveAll(this);
		FActorFolders::OnFolderDeleted.RemoveAll(this);
	}

	if (FLevelEditorModule* LevelEditorModulePtr = FModuleManager::GetModulePtr<FLevelEditorModule>("LevelEditor"))
	{
		LevelEditorModulePtr->OnActorSelectionChanged().RemoveAll(this);
	}

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationPreInputKeyDownListener().RemoveAll(this);
		FSlateApplication::Get().OnApplicationMousePreInputButtonDownListener().RemoveAll(this);
	}

	{
		FEditorUserDefinedCommands::OnExecuteActionDelegate.RemoveAll(this);
	}
}

void UEditorEventsSubsystem::HandleMapOpened(const FString& Filename, bool bAsTemplate)
{
	OnMapOpened.Broadcast(Filename);
}

void UEditorEventsSubsystem::HandleEditorModeChanged(const FEditorModeID& ModeID, bool bEntered)
{
	if (bEntered)
	{
		OnEditorModeEnter.Broadcast(ModeID);
	}
	else
	{
		OnEditorModeExit.Broadcast(ModeID);
	}
}

void UEditorEventsSubsystem::HandleEditorCameraMoved(const FVector& ViewLocation, const FRotator& ViewRotation, ELevelViewportType ViewportType, int32 ViewIndex)
{
	OnEditorCameraMoved.Broadcast(ViewLocation, ViewRotation);
}

void UEditorEventsSubsystem::HandleDollyPerspectiveCamera(const FVector& Drag, int32 ViewIndex)
{
	OnDollyPerspectiveCamera.Broadcast(Drag);
}

void UEditorEventsSubsystem::HandlePreBeginPIE(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::PreBeginPIE);
}

void UEditorEventsSubsystem::HandleBeginPIE(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::BeginPIE);
}

void UEditorEventsSubsystem::HandlePostPIEStarted(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::PostPIEStarted);
}

void UEditorEventsSubsystem::HandlePrePIEEnded(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::PrePIEEnded);
}

void UEditorEventsSubsystem::HandleEndPIE(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::EndPIE);
}

void UEditorEventsSubsystem::HandlePausePIE(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::PausePIE);
}

void UEditorEventsSubsystem::HandleResumePIE(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::ResumePIE);
}

void UEditorEventsSubsystem::HandleSingleStepPIE(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::SingleStepPIE);
}

void UEditorEventsSubsystem::HandlePreSwitchBeginPIEAndSIE(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::OnPreSwitchBeginPIEAndSIE);
}

void UEditorEventsSubsystem::HandleSwitchBeginPIEAndSIE(bool bIsSimulating)
{
	OnPlayInEditorEvent.Broadcast(bIsSimulating, EOnPIEEventType::OnSwitchBeginPIEAndSIE);
}

void UEditorEventsSubsystem::HandleAssetsPreDelete(const TArray<UObject*>& Assets)
{
	OnAssetsPreDeleted.Broadcast(Assets);
}


void UEditorEventsSubsystem::HandleFolderCreated(UWorld& InWorld, const FFolder& NewPath)
{
	OnWorldFolderCreate.Broadcast(NewPath.GetPath());
}

void UEditorEventsSubsystem::HandleFolderMoved(UWorld& InWorld, const FFolder& OldPath, const FFolder& NewPath)
{
	OnWorldFolderMove.Broadcast(OldPath.GetPath(), NewPath.GetPath());
}

void UEditorEventsSubsystem::HandleFolderDeleted(UWorld& InWorld, const FFolder& Path)
{
	OnWorldFolderDelete.Broadcast(Path.GetPath());
}

void UEditorEventsSubsystem::HandleActorSelectionChanged(const TArray<UObject*>& NewSelection, bool bForceRefresh /*= false*/)
{
	TArray<AActor*> SelectedActors;
	if (NewSelection.Num() > 0)
	{
		for (UObject* SelectedObject : NewSelection)
		{
			if (AActor* Actor = Cast<AActor>(SelectedObject))
			{
				SelectedActors.Add(Actor);
			}
		}
	}

	if (SelectedActors.Num() > 0)
	{
		OnActorsSelected.Broadcast(SelectedActors);
	}
	else
	{
		OnOnActorsUnselected.Broadcast();
	}
}

void UEditorEventsSubsystem::HandleMapChanged(UWorld* InWorld, EMapChangeType MapChangeType)
{
	OnMapChanged.Broadcast(InWorld, static_cast<ELevelMapChangeType>(MapChangeType));
}

void UEditorEventsSubsystem::HandleApplicationPreInputKeyDownListener(const FKeyEvent& InKeyEvent)
{
	OnInputKeyDown.Broadcast(InKeyEvent, LevelEditingViewportUtils::IsEditingViewportFocused());
}

void UEditorEventsSubsystem::HandleApplicationMousePreInputButtonDownListener(const FPointerEvent& MouseEvent)
{
	OnMouseButtonDown.Broadcast(MouseEvent, LevelEditingViewportUtils::IsEditingViewportFocused());
}

void UEditorEventsSubsystem::HandleWorldPreSaved(UWorld* World, FObjectPreSaveContext ObjectSaveContext)
{
	OnPreSaveWorld.Broadcast(World);
}

void UEditorEventsSubsystem::HandleWorldPostSaved(UWorld* World, FObjectPostSaveContext ObjectSaveContext)
{
	OnPostSaveWorld.Broadcast(World, ObjectSaveContext.SaveSucceeded());
}

void UEditorEventsSubsystem::HandleActionExecuted(UEditorUserDefinedActions* ActionsAsset, int32 ActionIndex , bool bIsRepeated)
{
	OnExecuteUserDefinedAction.Broadcast(ActionsAsset, static_cast<uint8>(ActionIndex), bIsRepeated);
}
