import importlib

from PalTemplate import Prfix
importlib.reload(Prfix)
from PalTemplate import Make_PalBS
importlib.reload(Make_PalBS)
from PalTemplate import Make_PalABP
importlib.reload(Make_PalABP)
from PalTemplate import Child_PalBp
importlib.reload(Child_PalBp)
from PalTemplate import ChangeProperty
importlib.reload(ChangeProperty)
from PalTemplate import MakeAttackTable
importlib.reload(MakeAttackTable)

def MakeCharacter(TEMPLATE_NAME : str):
   Make_PalABP.create_or_update_pal_anim_blueprint(TEMPLATE_NAME)
   Child_PalBp.create_or_update_blueprint(TEMPLATE_NAME)
   ChangeProperty.ChangeAndSaveAsset(TEMPLATE_NAME)
   MakeAttackTable.set_pal_attack_component_attack_data_asset_to_dt(TEMPLATE_NAME)

Prfix.main()
Make_PalBS.main()

MakeAttackTable.run_build_attack_montages_and_patch_dt()
MakeCharacter("Monster")
MakeCharacter("Pal")


