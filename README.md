# UE5 - Enhanced Input and Ability System (Template)

This project provides a simplified implementation of using the ability system with enhanced input. It is built upon a combination of Unreal's Lyra and Valley of the Ancient projects.

You can freely use it as your template to build a UE game project.

Project build on `Unreal Engine 5.2.1`

# Documentation

## 1. Project

This project provides an example of how to set up the enhanced input and ability system, along with some basic setup for Attributes ([see](https://github.com/Nic-Urb/UE5-EnhancedInput-AbilitySystem/tree/main/Source/GameTemplate/Public/GameplayAbilitySystem/Attributes)).

To implement this project, you should have some basic understanding of C++, Gameplay Ability Systen and Enhanced Input.

## 2.Usage

I'm not going to cover every aspect of how the enhanced input and ability system works. Instead, I'll show you some basic features that you can implement.

### 2.1 Setup default input

The default player input is set up by `default`, so you don't need to do anything with it. However, if you want to add new default inputs, you'll need to modify the `GameTemplateCharacter` C++ class and then map a new input action to `InputMapping Context` which is `IMC_Default`.

<img width="534" alt="Screenshot 2024-06-17 at 20 46 34" src="https://github.com/Nic-Urb/UE5-EnhancedInput-AbilitySystem/assets/79403684/3d95b6fd-9ca1-4c22-977a-74bc2b4cfb1c">

Then you need to assign these input actions to a `BP_ThirdPersonCharacter`

<img width="853" alt="Screenshot 2024-06-17 at 20 39 08" src="https://github.com/Nic-Urb/UE5-EnhancedInput-AbilitySystem/assets/79403684/2cfdd324-ea5a-4589-b142-2c5d0416fa72">

### 2.2 Setup ability system

First, you need to create a `UTemplateGameplayAbilitySet`. This `DataAsset` will contain all data about Abilities, Effects, and Attributes to grant.

<img width="955" alt="Screenshot 2024-06-17 at 20 31 54" src="https://github.com/Nic-Urb/UE5-EnhancedInput-AbilitySystem/assets/79403684/da593a96-60b1-4c53-9c38-a148c96cf774">

(Make sure you add input actions to the same `InputMappingContext` which is used by the default input.)

The `DataAsset` created above must be assigned to the `BP_ThirdPersonCharacter` inside a blueprint.

<img width="853" alt="Screenshot 2024-06-17 at 20 38 59" src="https://github.com/Nic-Urb/UE5-EnhancedInput-AbilitySystem/assets/79403684/f120855f-e1c1-4138-ab28-9fa55db8acf4">

