#ifndef ROGUE_ADVENTUREPATHS_H
#define ROGUE_ADVENTUREPATHS_H

#define ROGUE_WARP_UNSET 0
#define ROGUE_WARP_TO_ROOM 1
#define ROGUE_WARP_TO_ADVPATH 2 

extern EWRAM_DATA struct RogueAdvPath gRogueAdvPath;

bool8 RogueAdv_GenerateAdventurePathsIfRequired();
void RogueAdv_ApplyAdventureMetatiles();
void RogueAdv_UpdateObjectGFX();

u8 RogueAdv_OverrideNextWarp(struct WarpData *warp);

void RogueAdv_GetNodeParams();
void RogueAdv_ExecuteNodeAction();

#endif