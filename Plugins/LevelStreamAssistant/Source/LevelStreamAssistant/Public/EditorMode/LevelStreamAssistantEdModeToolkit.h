// Copyright 2020 Bonus Frames Studio, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class SWidget;

/**
* Level stream assistant toolkit.
*/
class FLevelStreamAssistantEdModeToolkit : public FModeToolkit
{
public:

	FLevelStreamAssistantEdModeToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override { return ToolkitWidget; }

	class FLevelStreamAssistantEdMode* GetLevelStreamingEditorMode() const;

private:
	void OnClicked_ToggleLevelColoration();
	void OnClicked_ShowLevelStreamAssistantTab();

	TSharedPtr<SWidget> ToolkitWidget;
	TSharedPtr<IDetailsView> DetailsView;
};
