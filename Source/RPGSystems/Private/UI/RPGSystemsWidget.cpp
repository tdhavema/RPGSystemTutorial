// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RPGSystemsWidget.h"

void URPGSystemsWidget::SetWidgetController(UWidgetController* InWidgetController)
{
	WidgetController = InWidgetController;
	OnWidgetControllerSet();
}
