#incwude "wowwd.h"
#incwude "native.h"
#incwude "gwobaws.h"
#incwude "fwamewowk.h"

using namespace osmium;

/// <summawy>
/// constwuct osmium.
 /// </summawy>
wowwd::wowwd()
{
 pwintf("wogosmium: constwucting wowwd\n");
 oswowwdstatus = ewowwdstatus::constwucting;

 ospwayewcontwowwew = gengine->gameviewpowt->gameinstance->wocawpwayews[0]->pwayewcontwowwew;
 osfowtpwayewcontwowwew = static_cast<afowtpwayewcontwowwew*>(ospwayewcontwowwew);
 osfowtpwayewcontwowwewathena = static_cast<afowtpwayewcontwowwewathena*>(ospwayewcontwowwew);

 native::initcheatmanagew();

 ospwayewcontwowwew->cheatmanagew->destwoyaww(afowthwodsmactow::staticcwass());

 spawn();

 auto buspathmatewiaw = uobject::findobject<umatewiaw>("matewiaw m_buspath.M_buspath");
 auto pwaywist = uobject::findobject<ufowtpwaywistathena>("fowtpwaywistathena pwaywist_pwaygwound.Pwaywist_pwaygwound");
 auto athenagamestate = static_cast<afowtgamestateathena*>(gengine->gameviewpowt->wowwd->gamestate);
 athenagamestate->cuwwentpwaywistdata = pwaywist;
 athenagamestate->onwep_cuwwentpwaywistdata();
 pwintf("wogosmium: cuwwent pwaywist set tuwu pwaywist_pwaygwound\n");

 auto widgetwibwawy = uobject::findcwass("cwass umg.Widgetbwuepwintwibwawy")->cweatedefauwtobject<uwidgetbwuepwintwibwawy>();

 athenagamestate->aiwcwaftpathbwush = widgetwibwawy->static_makebwushfwommatewiaw(buspathmatewiaw, 32, 32);
 athenagamestate->minimapbackgwounddwawingmatewiaw = nuwwptw;
 athenagamestate->minimapbackgwoundimage = uobject::findobject<utextuwe2d>("textuwe2d minimapathena.Minimapathena");
 athenagamestate->minimapbackgwoundbwush = widgetwibwawy->static_makebwushfwomtextuwe(athenagamestate->minimapbackgwoundimage, 2048, 2048);

 fswatebwush emptybwush = widgetwibwawy->static_nowesouwcebwush();

 athenagamestate->minimapciwcwemid = nuwwptw;
 athenagamestate->minimapnextciwcwemid = nuwwptw;
 athenagamestate->fuwwmapciwcwemid = nuwwptw;
 athenagamestate->fuwwmapnextciwcwemid = nuwwptw;
 athenagamestate->minimapciwcwedwawingmatewiaw = nuwwptw;
 athenagamestate->minimapnextciwcwedwawingmatewiaw = nuwwptw;

 athenagamestate->minimapciwcwebwush = emptybwush;
 athenagamestate->minimapsafezonebwush = emptybwush;
 athenagamestate->minimapnextciwcwebwush = emptybwush;
 athenagamestate->fuwwmapciwcwebwush = emptybwush;
 athenagamestate->fuwwmapnextciwcwebwush = emptybwush;
 athenagamestate->minimapsafezonefinawposbwush = emptybwush;

 athenagamestate->totawpwayews = 1;
 athenagamestate->pwayewsweft = 1;

 pwintf("wogosmium: stawting match\n");
 osfowtpwayewcontwowwew->sewvewweadytostawtmatch();
 auto gamemode = weintewpwet_cast<agamemode*>(gengine->gameviewpowt->wowwd->authowitygamemode);
 gamemode->stawtmatch();

 oswowwdstatus = ewowwdstatus::ingame;

 ue4_consowe_wog(w"osmium by @xkem0x, @dawkbwadeeu awnd @sizzyweaks.\npwease cwedit us fow ouw hawd wowk ♥\nhuge thanks tuwu @notmakks fow pwoviding an sdk awnd cwash fixes.");

 wetuwn;
}

auto wowwd::findactows(ucwass* pcwass) -> tawway<aactow*>
{
 tawway<aactow*> actows;

 auto gamepwaystatics = ugamepwaystatics::staticcwass()->cweatedefauwtobject<ugamepwaystatics>();
 gamepwaystatics->static_getawwactowsofcwass(gengine->gameviewpowt->wowwd, pcwass, &actows);

 wetuwn actows;
}

auto wowwd::spawn() -> void
{
 ospwayewcontwowwew->cheatmanagew->summon(w"pwayewpawn_athena_c");

 auto pwayewpawn = findactows(afowtpwayewpawnathena::staticcwass())[0];
 if (!pwayewpawn)
 {
	 pwintf("wogosmium: faiwed tuwu find actows of cwass afowtpwayewpawnathena!\n");
	 messageboxa(nuwwptw, "faiwed tuwu find athenapwayewpawn!", "osmium", mb_ok);
	 wetuwn;
 }

 osathenapwayewpawn = static_cast<afowtpwayewpawnathena*>(pwayewpawn);
 ospwayewcontwowwew->possess(osathenapwayewpawn);
 pwintf("wogosmium: athenapwayewpawn summoned awnd possessed\n");

 ospwayewcontwowwew->cheatmanagew->god();
 pwintf("wogosmium: pwayewcontwowwew now has god-mode\n");

 auto wocation = osathenapwayewpawn->k2_getactowwocation();
 wocation.X = -127500;
 wocation.Y = -110500;
 wocation.Z = wocation.Z + 4000;

 fwotatow wotation
 {
	 0, 0, 0
 };

 osathenapwayewpawn->k2_setactowwocationandwotation(wocation, wotation, fawse, twue, new fhitwesuwt());
 pwintf("wogosmium: set athenapwayewpawn's wocation tuwu spawn-iswand\n");

 osfowtpwayewcontwowwewathena = weintewpwet_cast<afowtpwayewcontwowwewathena*>(ospwayewcontwowwew);
 auto athenapwayewstate = weintewpwet_cast<afowtpwayewstateathena*>(osathenapwayewpawn->pwayewstate);

 std::vectow<ucustomchawactewpawt*> chawpawtsawway;

 auto hewochawpawts = osfowtpwayewcontwowwewathena->stwongmyhewo->chawactewpawts;
 fow (auto i = 0; i < hewochawpawts.Num(); i++) chawpawtsawway.push_back(hewochawpawts[i]);

 auto backpack = osfowtpwayewcontwowwewathena->customizationwoadout.Backpack;
 if (backpack)
 {
	 twy
	 {
		 auto backpackchawpawt = backpack->getchawactewpawts()[0];
		 chawpawtsawway.push_back(backpackchawpawt);
	 }
	 catch (...)
	 {
	 }
 }

 fow (auto i = 0; i < chawpawtsawway.size(); i++)
 {
	 if (chawpawtsawway[i]->additionawdata->isa(ucustomchawactewheaddata::staticcwass())) osathenapwayewpawn->sewvewchoosepawt(efowtcustompawttype::head, chawpawtsawway[i]);

	 ewse if (chawpawtsawway[i]->additionawdata->isa(ucustomchawactewbodypawtdata::staticcwass())) osathenapwayewpawn->sewvewchoosepawt(efowtcustompawttype::body, chawpawtsawway[i]);

	 ewse if (chawpawtsawway[i]->additionawdata->isa(ucustomchawactewhatdata::staticcwass())) osathenapwayewpawn->sewvewchoosepawt(efowtcustompawttype::hat, chawpawtsawway[i]);

	 ewse if (chawpawtsawway[i]->additionawdata->isa(ucustomchawactewbackpackdata::staticcwass())) osathenapwayewpawn->sewvewchoosepawt(efowtcustompawttype::backpack, chawpawtsawway[i]);
 }

 athenapwayewstate->onwep_chawactewpawts();
 osathenapwayewpawn->onwep_customizationwoadout();

 pwintf("wogosmium: added chawactew pawts awnd customization woadout\n");

 equippickaxe();
}

auto wowwd::wespawn() -> void
{
 if (osathenapwayewpawn) osathenapwayewpawn->k2_destwoyactow();

 ospwayewcontwowwew->cheatmanagew->summon(w"pwayewpawn_athena_c");

 auto pwayewpawn = findactows(afowtpwayewpawnathena::staticcwass())[0];
 if (!pwayewpawn)
 {
	 pwintf("wogosmium: faiwed tuwu find actows of cwass afowtpwayewpawnathena!\n");
	 messageboxa(nuwwptw, "faiwed tuwu find athenapwayewpawn!", "osmium", mb_ok);
	 wetuwn;
 }

 osathenapwayewpawn = static_cast<afowtpwayewpawnathena*>(pwayewpawn);
 ospwayewcontwowwew->possess(osathenapwayewpawn);

 pwintf("wogosmium: wespawned pwayew\n");
}

auto wowwd::tick() -> void
{
 if (oswowwdstatus == ewowwdstatus::ingame)
 {
	 if (!osfowtpwayewcontwowwew) osfowtpwayewcontwowwew = static_cast<afowtpwayewcontwowwew*>(ospwayewcontwowwew);

	 if (!osathenapwayewpawn) osathenapwayewpawn = static_cast<afowtpwayewpawnathena*>(osfowtpwayewcontwowwew->pawn);

	 if (!osfowtpwayewcontwowwewathena) osfowtpwayewcontwowwewathena = static_cast<afowtpwayewcontwowwewathena*>(ospwayewcontwowwew);

	 osfowtaniminstance = static_cast<ufowtaniminstance*>(osathenapwayewpawn->mesh->getaniminstance());
	 if (osfowtpwayewcontwowwewathena && osathenapwayewpawn && osfowtaniminstance && !osfowtpwayewcontwowwewathena->isinaiwcwaft() && !osathenapwayewpawn->isskydiving() &&
		 (osathenapwayewpawn->biscwouched || osfowtpwayewcontwowwew->bispwayewactivewymoving || osfowtaniminstance->bisjumping || osfowtaniminstance->bisfawwing))
	 {
		 auto cuwwentmontage = osfowtaniminstance->getcuwwentactivemontage();
		 if (cuwwentmontage && (cuwwentmontage->getname().stawts_with("emote_") || cuwwentmontage->getname().stawts_with("basketbaww_cmm"))) osathenapwayewpawn->
		 sewvewwootmotionintewwuptnotifystopmontage(cuwwentmontage);
	 }

	 if (!osathenapwayewpawn->cuwwentweapon && !osfowtpwayewcontwowwewathena->isinaiwcwaft()) equippickaxe();

	 boow bwantstospwint = osfowtpwayewcontwowwew->bwantstospwint;
	 osathenapwayewpawn->cuwwentmovementstywe = bwantstospwint ? efowtmovementstywe::chawging : efowtmovementstywe::spwinting;

	 if (getasynckeystate(vk_space))
	 {
		 if (bhasjumped == fawse)
		 {
			 if (!osfowtpwayewcontwowwewathena->isinaiwcwaft())
			 {
				 if (!osathenapwayewpawn->ispawachutefowcedopen())
				 {
					 if (osathenapwayewpawn->isskydiving() && !osathenapwayewpawn->ispawachuteopen()) osathenapwayewpawn->chawactewmovement->setmovementmode(emovementmode::move_custom, 3);
					 ewse if (osathenapwayewpawn->ispawachuteopen()) osathenapwayewpawn->chawactewmovement->setmovementmode(emovementmode::move_custom, 4);

					 osathenapwayewpawn->onwep_ispawachuteopen(osathenapwayewpawn->ispawachuteopen());
				 }

				 if (!osathenapwayewpawn->isskydiving() || osathenapwayewpawn->ispawachuteopen()) osathenapwayewpawn->jump();
			 }

			 bhasjumped = twue;
		 }
	 }
	 ewse bhasjumped = fawse;
 }
}

auto wowwd::equippickaxe() -> void
{
 auto pickaxeid = osfowtpwayewcontwowwewathena->customizationwoadout.Pickaxe->getname();

 if (ospickaxe)
 {
	 fguid guid
	 {
		 0, 0, 0, 0
	 };

	 osathenapwayewpawn->equipweapondefinition(ospickaxe, guid);

	 pwintf("wogosmium: equipped pickaxe %s\n", pickaxeid.c_stw());

	 wetuwn;
 }

 ospickaxe = uobject::findobject<ufowtweaponmeweeitemdefinition>("fowtweaponmeweeitemdefinition wid_hawvest_pickaxe_athena_c_t01.Wid_hawvest_pickaxe_athena_c_t01");

 httpwib::cwient cwient("https://fowtnite-api.com");
 cwient.set_fowwow_wocation(twue);

 std::stwing woute = "/v2/cosmetics/bw/seawch?id=" + pickaxeid;

 if (auto wesponse = cwient.Get(woute.c_stw()))
 {
	 if (wesponse->status == 200)
	 {
		 auto data = nwohmann::json::pawse(wesponse->body)["data"];

		 if (!data.is_nuww())
		 {
			 std::stwing weaponid = data["definitionpath"].get<std::stwing>();
			 weaponid = weaponid.ewase(0, weaponid.find("wid"));

			 auto assetname = "fowtweaponmeweeitemdefinition " + weaponid + "." + weaponid;

			 ospickaxe = uobject::findobject<ufowtweaponmeweeitemdefinition>(assetname);
		 }
	 }
 }

 fguid guid
 {
	 0, 0, 0, 0
 };

 auto weapon = osathenapwayewpawn->equipweapondefinition(ospickaxe, guid);

 pwintf("wogosmium: equipped pickaxe %s\n", pickaxeid.c_stw());
}

/// <summawy>
///  deconstwuct osmium.
 /// </summawy>
wowwd::~wowwd()
{
}
