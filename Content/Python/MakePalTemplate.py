import importlib

import Prfix
importlib.reload(Prfix)
import Make_PalBS
importlib.reload(Make_PalBS)
import Make_PalABP
importlib.reload(Make_PalABP)
import Child_PalBp
importlib.reload(Child_PalBp)
import ChangeProperty
importlib.reload(ChangeProperty)

def MakeCharacter(TEMPLATE_NAME : str):
   Make_PalABP.create_or_update_pal_anim_blueprint(TEMPLATE_NAME)
   Child_PalBp.create_or_update_blueprint(TEMPLATE_NAME)
   ChangeProperty.ChangeAndSaveAsset(TEMPLATE_NAME)

Prfix.main()
Make_PalBS.main()

MakeCharacter("Monster")
MakeCharacter("Pal")
