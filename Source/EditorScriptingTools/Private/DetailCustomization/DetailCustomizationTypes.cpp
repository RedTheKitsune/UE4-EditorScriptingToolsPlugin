//==========================================================================//
// Copyright Elhoussine Mehnik (ue4resources@gmail.com). All Rights Reserved.
//================== http://unrealengineresources.com/ =====================//

#include "DetailCustomizationTypes.h"
#include "Widgets/Layout/SBorder.h"
#include "EditorStyleSet.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#define LOCTEXT_NAMESPACE "DetailCustomizationTypes"

const FText FPropertyWidgetHelpers::UnknownPropertyText = LOCTEXT("UnknownProperty", "Unknown Property");

TSharedRef<SWidget> FPropertyWidgetHelpers::UnknownPropertyWidget = SNew(SBorder)
																			.Padding(0.0f)
																			.HAlign(HAlign_Fill)
																			.VAlign(VAlign_Fill)
																			.BorderImage(FEditorStyle::GetBrush("NoBorder"))
																			[
																				SNew(STextBlock)
																				.Text(FPropertyWidgetHelpers::UnknownPropertyText)
																			];


#undef LOCTEXT_NAMESPACE 

