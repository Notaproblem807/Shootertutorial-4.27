// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class ShootertutorialTarget : TargetRules
{
	public ShootertutorialTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		//new added by me
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Shootertutorial" } );
	}
}
