/* Copyright (C) 2021 Hugo ATTAL - All Rights Reserved
* This plugin is downloadable from the Unreal Engine Marketplace
*/

#include "Colorizer.h"

void FColorizer::ColorIcons()
{
	if (DarkerNodesSettings->UseCustomIcons)
	{
		// Opacity 50%

		// Red #b20000
		// Blue #0060b2
		// Yellow #dfb300
		// Green #3aa111

		FColor YellowGroupColor = ImageColor;
		FColor BlueGroupColor = ImageColor;
		FColor GreenGroupColor = ImageColor;

		if (DarkerNodesSettings->UseIconColorization)
		{
			YellowGroupColor = FColor(255,255,63);
			BlueGroupColor = FColor(0,191,255);
			GreenGroupColor = FColor(63,255,63);
		}

		AddMenuIcon(EditorStyle, "Kismet.Status.Unknown", "/Menu/CompileStatus_Working.png", ImageColor);
		AddMenuIcon(EditorStyle, "Kismet.Status.Error", "/Menu/CompileStatus_Fail.png", ImageColor);
		AddMenuIcon(EditorStyle, "Kismet.Status.Good", "/Menu/CompileStatus_Good.png", ImageColor);
		AddMenuIcon(EditorStyle, "Kismet.Status.Warning", "/Menu/CompileStatus_Warning.png", ImageColor);

		AddMenuIcon(EditorStyle, "AssetEditor.SaveAsset", "/Menu/icon_SaveAsset_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "SystemWideCommands.FindInContentBrowser", "/Menu/icon_toolbar_genericfinder_40px.png", ImageColor);
		AddMenuIcon(EditorStyle, "BlueprintEditor.FindInBlueprint", "/Menu/icon_Blueprint_Find_40px.png", ImageColor);
		AddMenuIcon(EditorStyle, "MaterialEditor.FindInMaterial", "/Menu/icon_Blueprint_Find_40px.png", ImageColor);
		AddMenuIcon(EditorStyle, "TranslationEditor.Search", "/Menu/icon_Blueprint_Find_40px.png", ImageColor);

		AddMenuIcon(EditorStyle, "FullBlueprintEditor.EditGlobalOptions", "/Menu/icon_Blueprint_Options_40px.png", BlueGroupColor);
		AddMenuIcon(EditorStyle, "FullBlueprintEditor.EditClassDefaults", "/Menu/icon_BlueprintEditor_Defaults_40x.png", BlueGroupColor);
		AddMenuIcon(EditorStyle, "FullBlueprintEditor.SwitchToBlueprintDefaultsMode", "/Menu/icon_BlueprintEditor_Defaults_40x.png", BlueGroupColor);
		AddMenuIcon(EditorStyle, "BlueprintEditor.EnableSimulation", "/Menu/icon_Enable_Simulation_40px.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.PlayInViewport", "/Menu/icon_playInSelectedViewport_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "GraphEditor.ToggleHideUnrelatedNodes", "/Menu/icon_HideUnrelatedNodes_40x.png", ImageColor);

		AddMenuIcon(EditorStyle, "LevelEditor.Build", "/Menu/icon_build_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "LevelEditor.Recompile", "/Menu/icon_compile_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "LevelEditor.OpenContentBrowser", "/Menu/icon_ContentBrowser_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "LevelEditor.EditorModes", "/Menu/icon_Editor_Modes_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "LevelEditor.ToggleVR", "/Menu/VR_Editor_Toolbar_Icon.png", ImageColor);
		AddMenuIcon(EditorStyle, "LevelEditor.GameSettings", "/Menu/icon_game_settings_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "LevelEditor.OpenLevelBlueprint", "/Menu/icon_kismet2_40x.png", BlueGroupColor);
		AddMenuIcon(EditorStyle, "LevelEditor.OpenMarketplace", "/Menu/icon_Marketplace_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "LevelEditor.EditMatinee", "/Menu/icon_matinee_40x.png", BlueGroupColor);

		AddMenuIcon(EditorStyle, "LevelEditor.SourceControl", "/Menu/icon_source_control_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "LevelEditor.SourceControl.On", "/Menu/icon_source_control_40x_on.png", ImageColor);
		AddMenuIcon(EditorStyle, "LevelEditor.SourceControl.Off", "/Menu/icon_source_control_40x_off.png", ImageColor);
		AddMenuIcon(EditorStyle, "LevelEditor.SourceControl.Unknown", "/Menu/icon_source_control_40x_unknown.png", ImageColor);
		AddMenuIcon(EditorStyle, "LevelEditor.SourceControl.Problem", "/Menu/icon_source_control_40x_problem.png", ImageColor);

		AddMenuIcon(EditorStyle, "MaterialEditor.Apply", "/Menu/icon_MatEd_Apply_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "MaterialEditor.CameraHome", "/Menu/icon_MatEd_Home_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "MaterialEditor.ToggleRealtimeExpressions", "/Menu/icon_MatEd_LiveNodes_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "MaterialEditor.AlwaysRefreshAllPreviews", "/Menu/icon_MatEd_Refresh_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "MaterialEditor.ToggleLivePreview", "/Menu/icon_MatEd_LivePreview_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "MaterialEditor.ToggleMaterialStats", "/Menu/icon_MatEd_Stats_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "MaterialEditor.TogglePlatformStats", "/Menu/icon_MobileStats_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "MaterialEditor.CleanUnusedExpressions", "/Menu/icon_MatEd_CleanUp_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "MaterialEditor.ShowHideConnectors", "/Menu/icon_MatEd_Connectors_40x.png", ImageColor);

		AddMenuIcon(EditorStyle, "PlayWorld.PausePlaySession", "/Menu/icon_pause_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.StopPlaySession", "/Menu/icon_stop_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.PossessPlayer", "/Menu/icon_possess_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.EjectFromPlayer", "/Menu/icon_eject_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.RepeatLastLaunch", "/Menu/icon_PlayOnDevice_40px.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.PlayInNewProcess", "/Menu/icon_PlayStandalone_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.PlayInEditorFloating", "/Menu/icon_playInWindow_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.PlayInMobilePreview", "/Menu/icon_PlayMobilePreview_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.PlayInVR", "/Menu/icon_playInVR_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.LateJoinSession", "/Menu/icon_simulate_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.ResumePlaySession", "/Menu/icon_simulate_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.Simulate", "/Menu/icon_simulate_40x.png", YellowGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.RepeatLastPlay", "/Menu/icon_simulate_40x.png", YellowGroupColor);

		AddMenuIcon(EditorStyle, "PlayWorld.SingleFrameAdvance", "/Menu/icon_advance_40x.png", BlueGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.ShowCurrentStatement", "/Menu/icon_findnode_40x.png", BlueGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.StepOut", "/Menu/icon_DebugStepOut_40x.png", BlueGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.StepInto", "/Menu/icon_DebugStepIn_40x.png", BlueGroupColor);
		AddMenuIcon(EditorStyle, "PlayWorld.StepOver", "/Menu/icon_DebugStepOver_40x.png", BlueGroupColor);

		AddMenuIcon(EditorStyle, "BTEditor.SwitchToBehaviorTreeMode", "/Menu/BehaviorTreeMode_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "UMGEditor.SwitchToDesigner", "/Menu/Designer_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "FullBlueprintEditor.SwitchToScriptingMode", "/Menu/icon_BlueprintEditor_EventGraph_40x.png", ImageColor);
		AddMenuIcon(CoreStyle, "WidgetReflector.Icon", "/Menu/icon_tab_WidgetReflector_40x.png", ImageColor);

		AddMenuIcon(EditorStyle, "EditorViewport.ToggleRealTime", "/Menu/icon_MatEd_Realtime_40x.png", ImageColor);

		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowWireframe", "/Menu/icon_StaticMeshEd_Wireframe_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowVertexColor", "/Menu/icon_StaticMeshEd_VertColor_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetRealtimePreview", "/Menu/icon_MatEd_Realtime_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ReimportMesh", "/Menu/icon_Persona_ReimportMesh_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowBounds", "/Menu/icon_StaticMeshEd_Bounds_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowCollision", "/Menu/icon_StaticMeshEd_Collision_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowGrid", "/Menu/icon_MatEd_Grid_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetDrawUVs", "/Menu/icon_StaticMeshEd_UVOverlay_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ResetCamera", "/Menu/icon_Camera_Reset_40px.png", ImageColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowPivot", "/Menu/icon_StaticMeshEd_ShowPivot_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowSockets", "/Menu/icon_StaticMeshEd_ShowSockets_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowNormals", "/Menu/icon_StaticMeshEd_Normals_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowTangents", "/Menu/icon_StaticMeshEd_Tangents_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowBinormals", "/Menu/icon_StaticMeshEd_Binormals_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetDrawAdditionalData", "/Menu/icon_StaticMeshEd_AdditionalData_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.SetShowVertices", "/Menu/icon_StaticMeshEd_Vertices_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowPivots", "/Menu/icon_StaticMeshEd_ShowPivot_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowSockets", "/Menu/icon_StaticMeshEd_ShowSockets_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowNormals", "/Menu/icon_StaticMeshEd_Normals_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowTangents", "/Menu/icon_StaticMeshEd_Tangents_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowBinormals", "/Menu/icon_StaticMeshEd_Binormals_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowBounds", "/Menu/icon_StaticMeshEd_Bounds_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowGrids", "/Menu/icon_MatEd_Grid_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowVertices", "/Menu/icon_StaticMeshEd_Vertices_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowWireframes", "/Menu/icon_StaticMeshEd_Wireframe_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "StaticMeshEditor.ToggleShowVertexColors", "/Menu/icon_StaticMeshEd_VertColor_40x.png", GreenGroupColor);
		AddMenuIcon(EditorStyle, "Persona.BakeMaterials", "/Menu/icon_tab_Layers_40x.png", ImageColor);

		AddMenuIcon(EditorStyle, "AnimationEditor.ApplyCompression", "/Menu/icon_Persona_Compression_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "AnimationEditor.ExportToFBX", "/Menu/icon_Persona_ExportFBX_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "AnimationEditor.ReimportAnimation", "/Menu/icon_Persona_ReimportMesh_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "AnimationEditor.CreateAsset", "/Menu/icon_Persona_CreateAsset_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "AnimationEditor.SetKey", "/Menu/icon_Persona_SetKey_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "AnimationEditor.ApplyAnimation", "/Menu/icon_MatEd_Apply_40x.png", ImageColor);

		AddMenuIcon(EditorStyle, "Persona.TogglePreviewAsset", "/Menu/icon_Persona_PreviewAsset_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "Persona.CreateAsset", "/Menu/icon_Persona_CreateAsset_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "Persona.ExportToFBX", "/Menu/icon_Persona_ExportFBX_40x.png", ImageColor);
		AddMenuIcon(EditorStyle, "Persona.ConvertToStaticMesh", "/Menu/icon_ShowStaticMeshes_40x.png", ImageColor);
	}

	ApplyImageBrush(EditorStyle, "EditorViewport.LocationGridSnap", "/Icons/Viewport/LocationGridSnap.png", Icon14, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.RotationGridSnap", "/Icons/Viewport/RotationGridSnap.png", Icon14, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.Layer2DSnap", "/Icons/Viewport/Layer2DSnap.png", Icon14, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.ScaleGridSnap", "/Icons/Viewport/ScaleGridSnap.png", Icon14, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.ToggleSurfaceSnapping", "/Icons/Viewport/icon_surface_snapping_14px.png", Icon14, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.RelativeCoordinateSystem_Local", "/Icons/Viewport/icon_axis_local_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.RelativeCoordinateSystem_Local.Small", "/Icons/Viewport/icon_axis_local_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.RelativeCoordinateSystem_World", "/Icons/Viewport/icon_axis_world_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.RelativeCoordinateSystem_World.Small", "/Icons/Viewport/icon_axis_world_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.CamSpeedSetting", "/Icons/Viewport/icon_CameraSpeed_24x16px.png", FVector2D(24, 16), TextColor);

	ApplyImageBrush(EditorStyle, "DetailsView.EditRawProperties", "/Icons/DetailsView/icon_PropertyMatrix_16px.png", Icon16, TextColor);

	ApplyImageBrush(EditorStyle, "CurveEd.Visible", "/Icons/DetailsView/icon_visible_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "CurveEd.VisibleHighlight", "/Icons/DetailsView/icon_visible_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "CurveEd.Invisible", "/Icons/DetailsView/icon_invisible_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "CurveEd.InvisibleHighlight", "/Icons/DetailsView/icon_invisible_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "Level.VisibleIcon16x", "/Icons/DetailsView/icon_visible_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "Level.VisibleHighlightIcon16x", "/Icons/DetailsView/icon_visible_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "Level.NotVisibleIcon16x", "/Icons/DetailsView/icon_invisible_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "Level.NotVisibleHighlightIcon16x", "/Icons/DetailsView/icon_invisible_16px.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "GenericViewButton", "/Icons/DetailsView/icon_visible_16px.png", Icon16, TextColor);

	const FColor TextColor50 = TextColor.WithAlpha(255 * 0.5);

	ApplyCenterIcon(EditorStyle, "PropertyWindow.Button_CreateNewBlueprint", "/Icons/DetailsView/PlusSymbol_12x.png", Icon12, TextColor50);
	ApplyCenterIcon(EditorStyle, "PropertyWindow.Button_Browse", "/Icons/DetailsView/lens_12x.png", Icon12, TextColor50);
	ApplyCenterIcon(EditorStyle, "PropertyWindow.Button_Use", "/Icons/DetailsView/assign_12x.png", Icon12, TextColor50);

	ApplyCenterIcon(EditorStyle, "GenericLock", "/Icons/DetailsView/padlock_locked_16x.png", Icon16, TextColor50);
	ApplyCenterIcon(EditorStyle, "GenericLock.Small", "/Icons/DetailsView/padlock_locked_16x.png", Icon16, TextColor50);
	ApplyCenterIcon(EditorStyle, "GenericUnlock", "/Icons/DetailsView/padlock_unlocked_16x.png", Icon16, TextColor50);
	ApplyCenterIcon(EditorStyle, "GenericUnlock.Small", "/Icons/DetailsView/padlock_unlocked_16x.png", Icon16, TextColor50);
	ApplyCenterIcon(EditorStyle, "PropertyWindow.Locked", "/Icons/DetailsView/padlock_locked_16x.png", Icon16, TextColor50);
	ApplyCenterIcon(EditorStyle, "PropertyWindow.Unlocked", "/Icons/DetailsView/padlock_unlocked_16x.png", Icon16, TextColor50);
	ApplyCenterIcon(EditorStyle, "FindResults.LockButton_Locked", "/Icons/DetailsView/padlock_locked_16x.png", Icon16, TextColor50);
	ApplyCenterIcon(EditorStyle, "FindResults.LockButton_Unlocked", "/Icons/DetailsView/padlock_unlocked_16x.png", Icon16, TextColor50);
	ApplyCenterIcon(EditorStyle, "ContentBrowser.LockButton_Locked", "/Icons/DetailsView/padlock_locked_16x.png", Icon16, TextColor50);
	ApplyCenterIcon(EditorStyle, "ContentBrowser.LockButton_Unlocked", "/Icons/DetailsView/padlock_unlocked_16x.png", Icon16, TextColor50);

	ApplyImageBrush(EditorStyle, "DetailsView.PulldownArrow.Down", "/Icons/PropertyView/AdvancedButton_Down.png",
	                FVector2D(10, 8), TextColor50);
	ApplyImageBrush(EditorStyle, "DetailsView.PulldownArrow.Down.Hovered", "/Icons/PropertyView/AdvancedButton_Down.png",
	                FVector2D(10, 8), TextColor);
	ApplyImageBrush(EditorStyle, "DetailsView.PulldownArrow.Up", "/Icons/PropertyView/AdvancedButton_Up.png",
	                FVector2D(10, 8), TextColor50);
	ApplyImageBrush(EditorStyle, "DetailsView.PulldownArrow.Up.Hovered", "/Icons/PropertyView/AdvancedButton_Up.png",
	                FVector2D(10, 8), TextColor);

	ApplyImageBrush(CoreStyle, "TreeArrow_Collapsed", "/Panel/TreeArrow_Collapsed.png", Icon10, TextColor50);
	ApplyImageBrush(CoreStyle, "TreeArrow_Collapsed_Hovered", "/Panel/TreeArrow_Collapsed.png", Icon10, TextColor);
	ApplyImageBrush(CoreStyle, "TreeArrow_Expanded", "/Panel/TreeArrow_Expanded.png", Icon10, TextColor50);
	ApplyImageBrush(CoreStyle, "TreeArrow_Expanded_Hovered", "/Panel/TreeArrow_Expanded.png", Icon10, TextColor);

	ApplyImageBrush(CoreStyle, "NotificationList.DefaultMessage", "/Icons/EventMessage_Default.png", Icon40, TextColor);

	ApplyImageBrush(EditorStyle, "TreeArrow_Collapsed", "/Panel/TreeArrow_Collapsed.png", Icon10, TextColor.WithAlpha(255 * 0.25));
	ApplyImageBrush(EditorStyle, "TreeArrow_Collapsed_Hovered", "/Panel/TreeArrow_Collapsed.png", Icon10, TextColor.WithAlpha(255 * 0.75));
	ApplyImageBrush(EditorStyle, "TreeArrow_Expanded", "/Panel/TreeArrow_Expanded.png", Icon10, TextColor.WithAlpha(255 * 0.25));
	ApplyImageBrush(EditorStyle, "TreeArrow_Expanded_Hovered", "/Panel/TreeArrow_Expanded.png", Icon10, TextColor.WithAlpha(255 * 0.75));

	ApplyImageBrush(EditorStyle, "EditorViewport.TranslateMode", "/Icons/Viewport/icon_translateb_16x.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.RotateMode", "/Icons/Viewport/icon_rotateb_16x.png", Icon16, TextColor);
	ApplyImageBrush(EditorStyle, "EditorViewport.ScaleMode", "/Icons/Viewport/icon_scaleb_16x.png", Icon16, TextColor);

	ApplyImageBrush(EditorStyle, "TimelineEditor.AddFloatTrack", "/Icons/Timeline/icon_TrackAddFloat_36x24px.png", FVector2D(36, 24), TextColor);
	ApplyImageBrush(EditorStyle, "TimelineEditor.AddVectorTrack", "/Icons/Timeline/icon_TrackAddVector_36x24px.png", FVector2D(36, 24), TextColor);
	ApplyImageBrush(EditorStyle, "TimelineEditor.AddEventTrack", "/Icons/Timeline/icon_TrackAddEvent_36x24px.png", FVector2D(36, 24), TextColor);
	ApplyImageBrush(EditorStyle, "TimelineEditor.AddColorTrack", "/Icons/Timeline/icon_TrackAddColor_36x24px.png", FVector2D(36, 24), TextColor);
	ApplyImageBrush(EditorStyle, "TimelineEditor.AddCurveAssetTrack", "/Icons/Timeline/icon_TrackAddCurve_36x24px.png", FVector2D(36, 24), TextColor);
	ApplyImageBrush(EditorStyle, "TimelineEditor.DeleteTrack", "/Icons/Timeline/icon_TrackDelete_36x24px.png", FVector2D(36, 24), TextColor);
}
