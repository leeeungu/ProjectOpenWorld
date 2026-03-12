import unreal
import json
import importlib
from PalTemplate import PalConfig
importlib.reload(PalConfig)
from PalTemplate.PalConfig import (
    MONSTER_ROOT,
    PAL_BP_PREFIX,
    PAL_ATTACK_DT_STRUCT_PATH,
    ANIM_SEQUENCE_NAMES,
)


PalList = PalConfig.get_pal_names_from_monster_root()
#/Game/Pal/DataTable

DT = PalConfig.load_or_create_datatable("/Game/Pal/DataTable", "DT_PalMonsterData", "PalMonsterData")

Rows = PalConfig.Get_DtRows_as_list(DT)

for row in Rows:
    if row["Name"] in PalList:
        pal = row["Name"]
        row["MonsterName"] = pal
        MonsterDT = PalConfig.Get_MonsterLevelDT(pal)
        row["LevelDataTable"] = f"BlueprintGeneratedClass'{MonsterDT.get_path_name()}'"


for pal in PalList:
    Rows.append({})
    row = Rows[-1]
    row["Name"] = pal
    row["MonsterName"] = pal
    PalMonster =  PalConfig.Get_MonsterBp(pal)
    MonsterDT = PalConfig.Get_MonsterLevelDT(pal)
    if not PalMonster:    
        unreal.log("MonsterClass Load Fail")
    else:
        row["MonsterClass"] = f"BlueprintGeneratedClass'{PalMonster.get_path_name()}_C'"
        unreal.log(  row["MonsterClass"])
    row["LevelDataTable"] = f"BlueprintGeneratedClass'{MonsterDT.get_path_name()}'"
    
    
PalConfig.Apply_dt_from_rows_list(DT, Rows)
    
    
