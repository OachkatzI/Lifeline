Hello developer! 

Looking for Free UMG reticles for your games? I've got your back!

There are 34 Reticles (UMG Widgets) in this project file. I used 8 texture files to create them and they are included as well (16 including the ones with the black strokes).

All of the 34 Reticle widgets are extended from a Parent Widget (WBP_Reticle_Base). Before you migrate the widgets to your project,
 remove the example code (It should be clearly commented) I've added to the function "CanIncreaseDynamicReticleSpread?" in the Parent Widget (WBP_Reticle_Base).

Migrate the Reticles folder (Content->Reticles) to your project. Replace "CanIncreaseDynamicReticleSpread?" functions code with your logic. And that should be it! 

All of the Projects content are Free for both commercial and non-commercial use. This includes the texture files as well. 
Why is it Free? I like free stuff myself and most of the content was developed for a personal hobby project I am working on.

If you need support with this project, please feel free to Tweet me @_Sithila (www.twitter.com/_Sithila) or email; sithila.s@outlook.com.

UE Forums : https://forums.unrealengine.com/community/community-content-tools-and-tutorials/1344077-free-34-dynamic-umg-reticles-crosshairs-asset-pack

I hope you will find this useful,
Sith. 

UPDATE V 1.2 
?- Upgraded the project to Unreal Engine 4.19
- Added support for Input scale based Crosshair/ Reticle scaling/spreading. 
- Instead of creating Widgets on mouse wheel up/down (old system), the new system accesses an array of a constructed Widgets (pool).    
?
UPDATE V 1.1
?- Upgraded the project to Unreal Engine 4.18
- Replaced crosshair WBP_Reticle_TypeF_2_v05 with a new design/ texture.