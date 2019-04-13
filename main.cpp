#include <gamebase/Gamebase.h>
#include <algorithm>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
	int gold, skill, ore, gems, sera, wood;

	bool isFighting = false;

	int days;

	deque <IntVec2> direction;

	bool heroMenuOpen = false;

	Timer timer;

	int stepPoints;

	enum ownerType
	{
		neutral, humanplayer, others, noOwner
	};

	enum nishType
	{
		golda,
		chest
	};

	enum recType
	{
		noneRec, mine, goldMine, castle, castle_entry, gem_mine, sera_mine, fight, sawmill, rock
	};

	struct nishData
	{
		nishType type;
		IntVec2 cords;
	};

	struct Price
	{
		enum Enum
		{
			gold, ore, wood, crystal, sera
		};
		Enum type;
		int n;
	};

	struct Magic
	{
		int id;
		int damage;
		string opisalovo;
	};

	vector <Magic> AllMagic = {
		{1, 10, "fireball, damage: 100"},
	};

	struct unitType
	{
		enum Enum
		{
			myaso, archer, grifon, chuvak, horserider, angel
		};
		int hp;
		int attack;
		int protect;
		float damage;
		int stepPoint;
		int initiative;
		int lvl;
		float k;
		int skill;
		bool strelyarcher;
		vector <int> availibleMagic;
		vector <Price> price;
		Enum type;
		string icon_name;
		string img_name;
	};

	vector<unitType> allTypes = {
		{
			4,
			4,
			8, // protect
			3,
			4,
			8,
			1,
			0.5,
			20,
			false,
			{},
			{{Price::gold, 85}},
			unitType::myaso,
			"myaso_icon.png",
			"myaso.png"
		},
		{
			10,
			4,
			3,// protect
			8,
			4,
			9,
			2,
			0.8,
			10,
			true,
			{},
			{{Price::gold, 50}},
			unitType::archer,
			"archer_icon.png",
			"Archer.png"
		},
		{
			30,
			15,
			17,// protect
			15,
			1,
			15,
			3,
			0.7,
			100,
			false,
			{},
			{{Price::gold, 260}},
			unitType::grifon,
			"griffin_icon.png",
			"griffin.png"
		},
		{
			54,
			14,
			14,// protect
			25,
			5,
			10,
			4,
			0.6,
			150,
			true,
			{1},
			{{Price::gold, 600}},
			unitType::chuvak,
			"chuvak_icon.png",
			"chuvak.png"
		},
		{
			90,
			23,
			21,// protect
			35,
			7,
			11,
			5,
			0.4,
			350,
			false,
			{},
			{{Price::gold, 1300}},
			unitType::horserider,
			"horserider_icon.png",
			"horserider.png"
		},
		{
			180,
			27,
			25,// protect
			45,
			6,
			12,
			6,
			0.3,
			450,
			false,
			{},
			{{Price::gold, 2800}, {Price::crystal, 1}},
			unitType::angel,
			"Angel_icon.png",
			"Angel.png"
		},
	};

	struct groundData {
		IntVec2 cords;
	};

	struct Unit
	{
		int n;
		unitType type;
	};
	
	struct Army
	{
		ownerType owner;
		vector <Unit> units;
	};
	//djsifefhwefhihiwefihwehfhwhefihweuhfwhehfewhfhiewihfiwehfhweuihfwhfwhefhiwehfiwehfheuihfwiuhfewhfhiewhfuhweihfwhefhewfhweifhwehifhewieuwhfefhiwehfifuweiuhfiwhfiuehiewihfwehfheuwhifwhifewhifewh
	enum class IsBuild
	{
		build, not_build
	};

	struct build
	{
		IsBuild isBuild;
		vector<Price> price;
		string description;
		string name;
		string icon_name;
		int prirost;
	};

	struct cityCenter
	{
		vector <build> magicBranch = {

			{ IsBuild::build, { {Price::gold, 2000}, {Price::ore, 2}, {Price::wood, 2}, {Price::crystal, 2}, {Price::sera, 2} }, "Позволяет вашему герою изучить заклинания первого круга", "Гильдия магии первого уровня", "cityBranches/magic/magic1", 0 },
			{ IsBuild::not_build, { {Price::gold, 3000}, {Price::ore, 3}, {Price::wood, 3}, {Price::crystal, 3}, {Price::sera, 3} }, "Позволяет вашему герою изучить заклинания второго круга", "Гильдия магии второго уровня", "cityBranches/magic/magic2", 0 },
			{ IsBuild::not_build, { {Price::gold, 4000}, {Price::ore, 4}, {Price::wood, 4}, {Price::crystal, 4}, {Price::sera, 4} }, "Позволяет вашему герою изучить заклинания третьего круга", "Гильдия магии третьего уровня", "cityBranches/magic/magic3", 0 },
			{ IsBuild::not_build, { {Price::gold, 5000}, {Price::ore, 5}, {Price::wood, 5}, {Price::crystal, 5}, {Price::sera, 5} }, "Позволяет вашему герою изучить заклинания четвертого круга", "Храм Христа спасителя", "cityBranches/magic/magic4", 0 }
		};
		vector <build> armyBranch = {

			{ IsBuild::build, { {Price::gold, 0}, {Price::ore, 0}, {Price::wood, 0}, {Price::sera, 0}, {Price::crystal, 0} }, "Позволяет нанимать в городе копейщиков", "Гарнизон", "cityBranches/army/army1", 20},
			{ IsBuild::not_build, { {Price::gold, 2500}, {Price::ore, 10}, {Price::wood, 0}, {Price::sera, 0}, {Price::crystal, 0} }, "Позволяет нанимать в городе лучников", "Башня лучников", "cityBranches/army/army2", 16 },
			{ IsBuild::not_build, { {Price::gold, 4000}, {Price::ore, 10}, {Price::wood, 10}, {Price::sera, 5}, {Price::crystal, 0} }, "Позволяет нанимать в городе грифонов", "Башня грифонов", "cityBranches/army/army3", 8 },
			{ IsBuild::not_build, { {Price::gold, 6000}, {Price::ore, 5}, {Price::wood, 5}, {Price::sera, 5}, {Price::crystal, 5} }, "Позволяет нанимать в городе инквизиторов", "Монастырь", "cityBranches/army/army4", 5 },
			{ IsBuild::not_build, { {Price::gold, 9000}, {Price::ore, 0}, {Price::wood, 15}, {Price::sera, 0}, {Price::crystal, 10} }, "Позволяет нанимать в городе паладинов", "Орден паладинов", "cityBranches/army/army5", 3 },
			{ IsBuild::not_build, { {Price::gold, 15000}, {Price::ore, 20}, {Price::wood, 0}, {Price::sera, 0}, {Price::crystal, 15} }, "Позволяет нанимать в городе ангелов", "Цитадель", "cityBranches/army/army6", 2 }
		};
		vector <build> ecoBranch = {

			{ IsBuild::build, { {Price::gold, 500}, {Price::ore, 0}, {Price::wood, 5}, {Price::sera, 0}, {Price::crystal, 0} }, "Даёт 250 золота за ход", "Дом старейшин", "cityBranches/cityHall/eco1", 0 },
			{ IsBuild::not_build, { {Price::gold, 4000}, {Price::ore, 10}, {Price::wood, 10}, {Price::sera, 5}, {Price::crystal, 0} }, "Даёт 1000 золота за ход", "Ратуша", "cityBranches/cityHall/eco2", 0 },
			{ IsBuild::not_build, { {Price::gold, 8000}, {Price::ore, 5}, {Price::wood, 5}, {Price::sera, 5}, {Price::crystal, 5} }, "Даёт 2000 золота за ход", "Магистрат", "cityBranches/cityHall/eco3", 0 },
			{ IsBuild::not_build, { {Price::gold, 12000}, {Price::ore, 0}, {Price::wood, 15}, {Price::sera, 0}, {Price::crystal, 10} }, "Даёт 4000 золота за ход", "Капитолий", "cityBranches/cityHall/eco4", 0 }
		};
	};

	struct recData
	{
		recType type;
		Army castleArmy;
		Army protectCastleArmy;
		ownerType owner;
		IntVec2 cords;
		cityCenter castleRec;
		bool heroInCastle = false;
	};

	struct playerData
	{
		Army army;
		int exp;
		ownerType owner;
	};

	struct neutralData
	{
		Army army;
		ownerType owner;
		IntVec2 cords;
	};

	struct eggData {
		IntVec2 cords;
	};

	struct unitsData
	{
		Unit unit;
		ownerType owner;
		IntVec2 cords;
		float k;
	};

	struct rockData
	{
		IntVec2 cords;
	};

	enum grass
	{
		grassDark,
		Player,
		Chest,
		Gold
	};

	/*float get_k(unitsData unit)
	{
		float k = 0;
		if (unit.unit.type.type == archer)
		{
			k += 0.5;
		}
		if (unit.unit.type.protect <= 7)
		{
			k += 0.3;
		}
		if (unit.unit.type.protect <= 12 && unit.unit.type.protect > 7)
		{
			k += 0.2;
		}
		if (unit.unit.type.protect > 12)
		{
			k += 0.1;
		}
		return k;
	}*/

	Army createArmy(vector<pair<unitType::Enum, int>> newArmy, ownerType owner)
	{
		Army army;
		for (auto armyUnit : newArmy)
		{
			Unit unit;
			unit.type = allTypes[armyUnit.first];
			unit.type.hp = allTypes[armyUnit.first].hp * armyUnit.second;
			unit.n = armyUnit.second;
			army.units.push_back(unit);
		}
		army.owner = owner;
		return army;
	}

	void loadArmy(Army army)
	{
		for (int i = 0; i < 6; i++)
		{
			auto unitstats = design.child<Layout>("unitstats" + toString(i + 1));
			auto icon = unitstats.child<Texture>("icon");
			auto label = unitstats.child<Label>("label");
			icon.setImageName(army.units[i].type.icon_name);
			label << army.units[i].n;
		}
	}

	void loadArmyCastle(Army army, GameObj castle)
	{
		for (int i = 0; i < 6; i++)
		{
			auto unitstats = design.child<Layout>("unitstatscastle" + toString(i + 1));
			connect(unitstats.child<Button>("unitButton"), merge_units, i, army.units[i].n, false, castle);
			auto icon = unitstats.child<Texture>("icon");
			auto label = unitstats.child<Label>("label");
			icon.setImageName(army.units[i].type.icon_name);
			label << army.units[i].n;
		}
	}

	void loadArmyHeroCastle(Army army, GameObj castle)
	{
		for (int i = 0; i < 6; i++)
		{
			auto unitstats = design.child<Layout>("unitstatsherocastle" + toString(i + 1));
			connect(unitstats.child<Button>("unitButton"), merge_units, i, army.units[i].n, true, castle);
			auto icon = unitstats.child<Texture>("icon");
			auto label = unitstats.child<Label>("label");
			icon.setImageName(army.units[i].type.icon_name);
			label << army.units[i].n;
		}
	}

	void loadMainArmy(Army army)
	{
		for (int i = 0; i < 6; i++)
		{
			auto unitstats = design.child<Layout>("unitstatsmain" + toString(i + 1));
			auto icon = unitstats.child<Texture>("icon");
			auto label = unitstats.child<Label>("label");
			icon.setImageName(army.units[i].type.icon_name);
			label << army.units[i].n;
		}
		design.update();
	}

	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void loadfinishstats(Army army, Army army2) // 1 - твоя 2 - враг
	{
		for (int i = 0; i < 6; i++)
		{
			auto unitstats = design.child<Layout>("yourfinishfightstats" + toString(i + 1));
			auto icon = unitstats.child<Texture>("icon");
			auto label = unitstats.child<Label>("label");
			icon.setImageName(army.units[i].type.icon_name);
			label << army.units[i].n;
		} 
		for (int i = 0; i < 6; i++)
		{
			auto unitstats = design.child<Layout>("finishfightstats" + toString(i + 1));
			auto icon = unitstats.child<Texture>("icon");
			auto label = unitstats.child<Label>("label");
			icon.setImageName(army2.units[i].type.icon_name);
			label << army2.units[i].n;
		}
	}  
	// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	/*struct merge_object
	{
		bool isSelect = false;
		int n = 0;
		int num = 0;
		bool fromHero = false;
	};*/

	//merge_object merge_obj;

	int UnitNum;

	void send_army(int n, bool fromHero, GameObj castle)
	{
		if (fromHero)
		{
			playerLayer.data(playerLayer.get(0)).army.units[n].n -= UnitNum;
			rec.data(castle).protectCastleArmy.units[n].n += UnitNum;
		}
		else
		{
			playerLayer.data(playerLayer.get(0)).army.units[n].n += UnitNum;
			rec.data(castle).protectCastleArmy.units[n].n -= UnitNum;
		}
		loadArmyHeroCastle(playerLayer.data(playerLayer.get(0)).army, castle);
		loadArmyCastle(rec.data(castle).protectCastleArmy, castle);
		auto merge_window = design.child<Layout>("mergeArmyWindow");
		merge_window.hide();
		design.child<Button>("getArmyButton").enable();
		design.child<Button>("castleBuildings").enable();
		UnitNum = 0;
		design.child<TextBox>("unitNum") << 0;
	}

	void more_units(int n, GameObj castle, bool fromHero)
	{
		if (fromHero)
		{
			if (UnitNum >= playerLayer.data(playerLayer.get(0)).army.units[n].n)
			{
				return;
			}
			else
			{
				UnitNum++;
				design.child<TextBox>("unitNum") << UnitNum;
			}
		}
		else
		{
			if (UnitNum == rec.data(castle).protectCastleArmy.units[n].n)
			{
				return;
			}
			else
			{
				UnitNum++;
			}
		}
	}

	void min_units(int n, GameObj castle)
	{
		if (UnitNum <= 0)
		{
			return;
		}
		else
		{
			UnitNum--;
			design.child<TextBox>("unitNum") << UnitNum;
		}
	}

	void change_textbox(int n, GameObj castle, bool fromHero)
	{
		design.child<TextBox>("unitNum") >> UnitNum;

		if (fromHero)
		{
			if (UnitNum > playerLayer.data(playerLayer.get(0)).army.units[n].n)
			{
				UnitNum = playerLayer.data(playerLayer.get(0)).army.units[n].n;
			}
		}
		else
		{
			if (UnitNum > rec.data(castle).protectCastleArmy.units[n].n)
			{
				UnitNum = rec.data(castle).protectCastleArmy.units[n].n;
			}
		}
		if (UnitNum < 0)
		{
			UnitNum = 0;
		}
		design.child<TextBox>("unitNum") << UnitNum;
	}

	void merge_units(int n, int num, bool fromHero, GameObj castle)
	{
		auto merge_window = design.child<Layout>("mergeArmyWindow");
		merge_window.show();
		design.child<Button>("getArmyButton").disable();
		design.child<Button>("castleBuildings").disable();
		connect(merge_window.child<TextBox>("unitNum"), change_textbox, n, castle, fromHero);
		connect(merge_window.child<Button>("more_units"), more_units, n, castle, fromHero);
		connect(merge_window.child<Button>("min_units"), min_units, n, castle);
		connect(merge_window.child<Button>("sendArmyButton"), send_army, n, fromHero, castle);
		/*if (merge_obj.isSelect)
		{
			if (n == merge_obj.n)
			{
				if (merge_obj.fromHero)
				{
					rec.data(castle).protectCastleArmy.units[n].n += playerLayer.data(playerLayer.get(0)).army.units[n].n;
					playerLayer.data(playerLayer.get(0)).army.units[n].n = 0;
					design.child<Layout>("unitstatsherocastle" + toString(merge_obj.n + 1)).child<ToggleButton>("unitButton").unpress();
					design.child<Layout>("unitstatscastle" + toString(n + 1)).child<ToggleButton>("unitButton").unpress();
				}
				else
				{
					playerLayer.data(playerLayer.get(0)).army.units[n].n += rec.data(castle).protectCastleArmy.units[n].n;
					rec.data(castle).protectCastleArmy.units[n].n = 0;
					design.child<Layout>("unitstatsherocastle" + toString(n + 1)).child<ToggleButton>("unitButton").unpress();
					design.child<Layout>("unitstatscastle" + toString(merge_obj.n + 1)).child<ToggleButton>("unitButton").unpress();
				}
				merge_obj.isSelect = false;
				loadArmyHeroCastle(playerLayer.data(playerLayer.get(0)).army, castle);
				loadArmyCastle(rec.data(castle).protectCastleArmy, castle);
			}
			else
			{

			}
		}
		else
		{
			merge_obj.isSelect = true;
			merge_obj.n = n;
			merge_obj.num = num;
			merge_obj.fromHero = fromHero;
		}*/
	}

	Army createEmptyArmy(ownerType owner)
	{
		return createArmy({
			   { unitType::myaso, 0 },
			   { unitType::archer, 0 },
			   { unitType::grifon, 0 },
			   { unitType::chuvak, 0 },
			   { unitType::horserider, 0 },
			   { unitType::angel, 0 }
			}, owner);
	}

	void readNeutralsData()
	{
		ifstream File("neutrals_data.txt");
		int rows = 0;
		File >> rows;
		for (int i = 0; i < rows; i++)
		{
			int n = 0;
			int unitNum = 0;
			IntVec2 cords;
			File >> n >> unitNum >> cords.x >> cords.y;
			auto Neutral = neutrals.load("neutral.json", cords.x * 150, cords.y * 150);
			neutrals.data(Neutral).army = createEmptyArmy(neutral);
			Neutral.skin<Texture>().setImageName(allTypes[n - 1].img_name);
			neutrals.data(Neutral).army.units[n - 1].n = unitNum;
			neutrals.data(Neutral).army.owner = neutral;
			neutrals.data(Neutral).owner = neutral;
			neutrals.data(Neutral).cords = cords;
			neutrals.data(Neutral).army.units[n - 1].type.hp = unitNum * allTypes[n - 1].hp;
		}

	}

	void clearArmy(Army& army, ownerType owner)
	{
		army = createArmy({
					{ unitType::myaso, 0 },
					{ unitType::archer, 0 },
					{ unitType::grifon, 0 },
					{ unitType::chuvak, 0 },
					{ unitType::horserider, 1 },
					{ unitType::angel, 0 }
				}, owner);
	}

	void fightExit(bool lose)
	{
		isFighting = false;
		units.clear();
		selector.select(1);
		loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
		design.child<Layout>("statusBar").show();
		if (lose)
		{
			clearArmy(playerLayer.data(playerLayer.get(0)).army, humanplayer);
			bool isHaveCastle = false;
			for (auto r : rec.all())
			{
				auto type = rec.data(r).type;
				auto owner = rec.data(r).owner;
				auto powner = playerLayer.data(playerLayer.get(0)).owner;
				if (type == castle)
				{
					if (owner == powner)
					{
						playerLayer.get(0).setPos(pixels({ rec.data(r).cords.x,rec.data(r).cords.y - 1 }, isZoomDown));
						clearSteps();
						isHaveCastle = true;
					}
				}
			}
			if (!isHaveCastle)
			{
				clearSteps();
				setStartPlayerPos(false);
			}
		}
	}

	void initializePlayer(int x,int y)
	{
		playerLayer.load("Heroes.json", x * 150, y * 150);
		auto& player = playerLayer.data(playerLayer.get(0));
		player.army.owner = humanplayer;
		player.owner = humanplayer;
		player.army = createArmy({
			{ unitType::myaso, 20 },
			{ unitType::archer, 50 },
			{ unitType::grifon, 50 },
			{ unitType::chuvak, 100 },
			{ unitType::horserider, 1 },
			{ unitType::angel, 1000 } },
			humanplayer
			);
		player.army.owner = humanplayer;
	}

	void setStartPlayerPos(bool load)
	{
		map<Color, int> colorToTypeMobs = {
			{ Color(0, 0, 0), Player },
		};
		MobsMap = loadMap("mapPlayer.png", colorToTypeMobs);
		for (int x = 0; x < MobsMap.w; ++x)
		{
			for (int y = 0; y < MobsMap.h; ++y)
			{
				if (MobsMap[x][y] == Player)
				{
					if (load)
					{
						initializePlayer(x, y);
					}
					else
					{
						playerLayer.get(0).setPos(pixels({ x, y }, isZoomDown));
						
					}
					/*auto& parmy = player.army;
					Unit Unit;
					Unit.type = allTypes[0];
					Unit.n = 20;
					parmy.units.push_back()*/
				}
			}
		}
		field.setView(groundmap.w / 2 * 150 - 75, groundmap.h / 2 * 150 - 75);
	}

	int normalHeight, normalWidth;

	void load()
	{
		connect(Relog, nextStep);
		readNeutralsData();
		connect(heroMenu, openHeroMenu);
		connect(select0, changeHeroMenu, stats);
		connect(select1, changeHeroMenu, arts);
		connect(select2, changeHeroMenu, army);
		connect(fight_exit, fightExit, true);
		fight_exit.hide();
		connect(newDirection, clearSteps);
		connect(timer, hideLabel);

		connect(useMagicUnit, showMagicWindow);

		timer.repeat(2);

		selector.select(1);

		normalHeight = field.height();
		normalWidth = field.width();

		gold = 0;
		skill = 0;
		stepPoints = 4;
		days = 0;
		ore = 0;
		sera = 0;
		gems = 0;
		wood = 0;

		GOld << "gold: " << gold;
		SKill << "skill: " << skill;

		selector.select(1);

		connect(start_button, startGame);
		connect(quit_button, quitGame);

		map<Color, int> colorToTypeGround = {
			{ Color(165, 0, 255), grassDark },
		};
		groundmap = loadMap("Grassmap.png", colorToTypeGround);
		for (int x = 0; x < groundmap.w; ++x)
		{
			for (int y = 0; y < groundmap.h; ++y)
			{
				if (groundmap[x][y] == grassDark)
				{
					auto cell = ground.load("grass.json", x * 150, y * 150);
					ground.data(cell).cords.x = x;
					ground.data(cell).cords.y = y;
				}
			}
		}

		setStartPlayerPos(true);

		map<Color, int> colorToTypeNishtyak = {
			{ Color(255, 119, 0), Chest },
			{ Color(246, 255, 0), Gold },
		};
		Nishtyakmap = loadMap("Nishtyaki.png", colorToTypeNishtyak);
		for (int x = 0; x < Nishtyakmap.w; ++x)
		{
			for (int y = 0; y < Nishtyakmap.h; ++y)
			{
				if (Nishtyakmap[x][y] == Chest)
				{
					auto nishtyak = nishtyaki.load("Nishtyak.json", x * 150, y * 150);
					nishtyaki.data(nishtyak).type = chest;
					nishtyaki.data(nishtyak).cords.x = x;
					nishtyaki.data(nishtyak).cords.y = y;
				}
				if (Nishtyakmap[x][y] == Gold)
				{
					auto nishtyak = nishtyaki.load("Nishtyak.json", x * 150, y * 150);
					nishtyak.skin<Texture>().setImageName("gold.png");
					nishtyak.setScaleX(1.5);
					nishtyaki.data(nishtyak).type = golda;
				}
			}
		}
		field.setView(Nishtyakmap.w / 2 * 150 - 75, Nishtyakmap.h / 2 * 150 - 75);

		map<Color, int> colorToTypeRec = {
			{ Color(0, 0, 0), noneRec},
			{ Color(255, 238, 0), mine },
			{ Color(252, 255, 249), castle },
			{ Color(0, 4, 255), goldMine },
			{ Color(255, 0, 246), gem_mine },
			{ Color(127, 0, 19), sera_mine },
			{ Color(0, 250, 255), sawmill},
		};
		recmap = loadMap("recmap.png", colorToTypeRec);
		realrecmap = loadMap("recmap.png", colorToTypeRec);
		for (int x = 0; x < recmap.w; ++x)
		{
			for (int y = 0; y < recmap.h; ++y)
			{
				if (recmap[x][y] == mine)
				{
					auto Rec = rec.load("Rec.json", x * 150, y * 150);
					Rec.child<Texture>("mineTexture").setImageName("mine.png");
					rec.data(Rec).owner = neutral;
					rec.data(Rec).type = mine;
					rec.data(Rec).cords = { x,y };
				}
				if (recmap[x][y] == gem_mine)
				{
					auto Rec = rec.load("Rec.json", x * 150, y * 150);
					Rec.child<Texture>("mineTexture").setImageName("gem_mine.png");
					rec.data(Rec).owner = neutral;
					rec.data(Rec).type = gem_mine;
					rec.data(Rec).cords = { x,y };
				}
				if (recmap[x][y] == sera_mine)
				{
					auto Rec = rec.load("Rec.json", x * 150, y * 150);
					Rec.child<Texture>("mineTexture").setImageName("mine_sera.png");
					rec.data(Rec).owner = neutral;
					rec.data(Rec).type = sera_mine;
					rec.data(Rec).cords = { x,y };
				}
				if (recmap[x][y] == goldMine)
				{
					auto Rec = rec.load("Rec.json", x * 150, y * 150);
					rec.data(Rec).type = goldMine;
					rec.data(Rec).owner = neutral;
					rec.data(Rec).cords = { x,y };
				}
				if (recmap[x][y] == castle)
				{
					if (realrecmap[x][y] == castle)
					{
						realrecmap[x + 1][y] = castle;
						realrecmap[x][y + 1] = castle;
						realrecmap[x + 1][y + 1] = castle;
						realrecmap[x + 1][y - 1] = castle;
						realrecmap[x][y - 1] = castle;
						realrecmap[x - 1][y - 1] = castle_entry;
						realrecmap[x - 1][y] = castle;
						realrecmap[x - 1][y + 1] = castle;
					}
					auto Rec = rec.load("Rec.json", x * 150, y * 150);
					Rec.child<Texture>("mineTexture").setImageName("castle.png");
					rec.data(Rec).owner = neutral;
					Rec.setScaleX(1.5);
					Rec.setScaleY(1.5);
					rec.data(Rec).type = castle;
					rec.data(Rec).cords = { x - 1, y - 1 };
					rec.data(Rec).castleArmy.owner = neutral;
					rec.data(Rec).castleArmy = createArmy({
						{ unitType::myaso, 1000 },
						{ unitType::archer, 1000 },
						{ unitType::grifon, 1000 },
						{ unitType::chuvak, 1000 },
						{ unitType::horserider, 1000 },
						{ unitType::angel, 1000 } }, humanplayer);
					rec.data(Rec).protectCastleArmy = createArmy({
						{ unitType::myaso, 10 },
						{ unitType::archer, 10 },
						{ unitType::grifon, 10 },
						{ unitType::chuvak, 10 },
						{ unitType::horserider, 10 },
						{ unitType::angel, 10 } }, humanplayer);
				}
				if (recmap[x][y] == sawmill) 
				{
					auto Rec = rec.load("Rec.json", x * 150, y * 150);
					Rec.child<Texture>("mineTexture").setImageName("lesopilka.png");
					rec.data(Rec).type = sawmill;
					rec.data(Rec).owner = neutral;
					rec.data(Rec).cords = { x,y };
				}
			}
		}
		for (auto Neutral : neutrals.all())
		{
			auto& data = neutrals.data(Neutral);
			realrecmap[data.cords.x][data.cords.y] = fight;
		}



		map<Color, int> colorToTypeRock = {
			{ Color(111, 111, 111), rock },
		};
		rockmap = loadMap("rockpng.png", colorToTypeRock);
		for (int x = 0; x < rockmap.w; ++x)
		{
			for (int y = 0; y < rockmap.h; ++y)
			{
				if (rockmap[x][y] == rock)
				{
					auto r = rocks.load("hora.json", x * 150, y * 150);
					rocks.data(r).cords = { x,y };
					realrecmap[x][y] = rock;
				}
			}
		}


		field.setView(groundmap.w / 2 * 150 - 75, groundmap.h / 2 * 150 - 75);
		loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
		playerLayer.data(playerLayer.get(0)).exp = 0;
		playerLayer.data(playerLayer.get(0)).owner = humanplayer;
		updateHeroCastles();
		loadTextBank("textBank.json");
	}

	bool isZoomDown = false;

	void zoom(float scale)
	{
		for (auto g : ground.all())
		{
			auto cords = ground.data(g).cords;
			g.setScale(scale);
			g.setPos(cords.x * (150 * scale), cords.y * (150 * scale));
		}
		for (auto p : playerLayer.all())
		{
			p.setScale(scale);
			if (isZoomDown)
			{
				p.setPos(cell(p.pos()).x * (150 * scale), cell(p.pos()).y * (150 * scale));
			}
			else
			{
				p.setPos(cell_zoom(p.pos()).x * (150 * scale), cell_zoom(p.pos()).y * (150 * scale));
			}
		}
		for (auto n : nishtyaki.all())
		{
			auto cords = nishtyaki.data(n).cords;
			n.setScale(scale);
			n.setPos(cords.x * (150 * scale), cords.y * (150 * scale));

		}
		for (auto r : rec.all())
		{
			auto cords = rec.data(r).cords;
			r.setScale(scale);
			r.setPos(cords.x * (150 * scale), cords.y * (150 * scale));
			if (rec.data(r).type == castle || rec.data(r).type == castle_entry)
			{
				r.setScale(scale * 1.5);
				r.setPos((cords.x + 1) * (150 * scale), (cords.y + 1) * (150 * scale));
			}
		}
		for (auto e : SuperMegaPuperStepEgg.all())
		{
			auto cords = SuperMegaPuperStepEgg.data(e).cords;
			e.setScale(scale);
			e.setPos(cords.x * 150 * scale, cords.y * 150 * scale);
		}
		for (auto neutral : neutrals.all())
		{
			auto cords = neutrals.data(neutral).cords;
			neutral.setScale(scale);
			neutral.setPos(cords.x * (150 * scale), cords.y * (150 * scale));
		}
		for (auto r : rocks.all())
		{
			auto cords = rocks.data(r).cords;
			r.setScale(scale);
			r.setPos(cords.x * (150 * scale), cords.y * (150 * scale));
		}
		cout << field_container.size() << endl;
		field.setView(groundmap.w / 2 * (150 * scale) - (75 * scale), groundmap.h / 2 * (150 * scale) - (75 * scale));
		cout << field_container.size() << endl;
	} //edwaeawduhwaduwhahdauwhdiwauhduhiwahuhdwuahdhwhauhdiuhiwuahidhwuahduhwuahdhwuahudwhduhwudhwuhdhwduhwhwuhduwhduhwhdhdwudhwuhduwhduhdiuwhduwhduwdhwhdwdwdwdhuwhdwhdhwdwhudwhuhegfteffdtefdfertwrwtrwtdrtwddfgsfdgsdfgsdgsd
	
	void updateHeroCastles() {

		design.child<Layout>("castle_menu").clear();
		for (auto Build : rec.all())
		{
			if (rec.data(Build).type == castle)
			{
				if (rec.data(Build).owner == playerLayer.data(playerLayer.get(0)).owner)
				{
					auto Castle = design.child<Layout>("castle_menu").load("castle_icon.json");
					connect(Castle.child<Button>("castle_icon"), openCastleMenu, Build);
				}
			}
		}
	}

	void hideLabel()
	{
		emptyStep.hide();
	}

	void nextWeek()
	{
		for (auto Build : rec.all())
		{
			if (rec.data(Build).type == castle)
			{
				int i = 0;
				for (auto piu : rec.data(Build).castleRec.armyBranch)
				{
					if (piu.isBuild == IsBuild::build)
					{
						rec.data(Build).castleArmy.units[i].n += piu.prirost;
					}
					i++;
				}
			}
		}
		return;
	}

	void nextStep()
	{
		days++;
		if (days % 7 == 0)
		{
			nextWeek();
		}
		if (!playerLayer.get(0).anim.isEmpty() || !empty(direction) && stepPoints > 0)
		{
			return;
		}
		stepPoints = 4;
		for (auto Rec : rec.all())
		{
			if (rec.data(Rec).owner == playerLayer.data(playerLayer.get(0)).owner)
			{
				if (rec.data(Rec).type == goldMine)
				{
					gold += 1000;
					cout << "Gold: " << gold << endl;
				}
				else if (rec.data(Rec).type == mine)
				{
					ore += 2;
					cout << "Ore: " << ore << endl;
				}
				else if (rec.data(Rec).type == gem_mine)
				{
					gems++;
				}
				else if (rec.data(Rec).type == sera_mine)
				{
					sera++;
				}
				else if (rec.data(Rec).type == castle)
				{
					int castleGold = 0;
					int i = 0;
					for (auto Build : rec.data(Rec).castleRec.ecoBranch)
					{
						if (Build.isBuild == IsBuild::build)
						{
							if (i == 0)
							{
								castleGold = 250;
							}
							else if (i == 1)
							{
								castleGold = 1000;
							}
							else if (i == 2)
							{
								castleGold = 2000;
							}
							else if (i == 3)
							{
								castleGold = 4000;
							}
						}
						i++;
					}
					gold += castleGold;
				}
				else if (rec.data(Rec).type == sawmill)
				{
					wood += 2;
				}
			}
		}
	}

	void openHeroMenu()
	{
		if (!heroMenuOpen)
		{
			selector.select(2);
			heroMenuOpen = true;
		}
		else
		{
			selector.select(1);
			heroMenuOpen = false;
		}
	}

	enum heroMenuType
	{
		stats, army, arts
	};

	void changeHeroMenu(heroMenuType type)
	{
		if (type == stats)
		{
			heroMenuSelector.select(0);
		}
		if (type == arts)
		{
			heroMenuSelector.select(1);
		}
		if (type == army)
		{
			loadArmy(playerLayer.data(playerLayer.get(0)).army);
			heroMenuSelector.select(2);
		}
	}

	void clearSteps()
	{
		direction.clear();
		SuperMegaPuperStepEgg.clear();
	}

	void startGame()
	{
		selector.select(1);
	}

	void quitGame()
	{
		close();
	}

	float damage_count(int attack, int protect, int damage, int n)
	{
		float damage_per_unit = damage * ((attack - protect) * 0.05 + 1);
		if (damage_per_unit < 1)
		{
			return n;
		}
		else
		{
			return n * damage_per_unit;
		}

		/*if (attack > protect)
		{
			return (attack - protect) * damage * n * 0.02 * lvl;
		}
		else
		{
			return (n * damage * attack / float(protect)) * 0.02 * lvl;
		}*/
	}

	int round_up(float a)
	{
		return a + 0.999999999;
	}

	int die_count(float damage, float n, int hp, int lvl)
	{
		float new_n = n - damage / allTypes[lvl - 1].hp;

		if (new_n < 0)
		{
			return round_up(n);
		}
		else
		{
			return  round_up(n) - round_up(new_n);
		}
	}

	void close_fight_stats()
	{
		auto info = design.child<Layout>("fight_unit_stats");
		info.hide();
	}

    void process(Input input)
	{
        using namespace gamebase::InputKey;
		if (input.justPressed(Escape))
		{
			close();
		}
		
		if (isFighting)
		{
			if (!design.child<Layout>("forFightWindow").empty())
			{
				return;
			}
			if (animStart)
			{
				return;
			}
			if (input.justPressed(MouseRight))
			{
				if (getUnitOwner(fight_queue[0].second) != humanplayer)
				{
					return;
				}
				auto c = cell_fight(fight_field.mousePos());
				int die = 0;
				if (c.x >= fight_field_map.w || c.y >= fight_field_map.h || c.y < 0 || c.x < 0)
				{
					return;
				}
				if (fight_field_map[c.x][c.y] > -1)
				{
					auto unit1 = units.data(fight_queue[0].second).unit;
					auto unit2 = units.data(fight_field_map[c.x][c.y]).unit;
					int lvl1 = unit1.type.lvl;
					int lvl2 = unit2.type.lvl;
					auto info = design.child<Layout>("fight_unit_stats");
					info.show();
					info.child<Label>("attack") << unit2.type.attack;
					info.child<Label>("protect") << unit2.type.protect;
					auto damage = damage_count(unit1.type.attack, unit2.type.protect, unit1.type.damage, unit1.n);
					auto die = die_count(damage, unit2.n, unit2.type.hp, lvl2);
					info.child<Label>("enemy_die") << die;
					if (die < unit2.n)
					{
						damage = damage_count(unit1.type.attack, unit2.type.protect, unit1.type.damage, unit2.n - die);
						die = die_count(damage, unit1.n, unit1.type.hp, lvl1);
						info.child<Label>("your_die") << die;
					}
					else
					{
						info.child<Label>("enemy_die") << unit2.n;
						info.child<Label>("your_die") << 0;
					}
					connect(info.child<Button>("close_fight_stats"), close_fight_stats);
				}
			}
			if (input.justPressed(MouseLeft))
			{
				auto c = cell_fight(fight_field.mousePos());
				if (zone.count(c) == 0)
				{
					return;
				}					
				if (c.x >= fight_field_map.w || c.y >= fight_field_map.h || c.y < 0 || c.x < 0)
				{
					return;
				}
				int id = fight_queue[0].second;
				auto& cords = units.data(id).cords;
				auto obj = units.get(fight_queue[0].second);
				auto fight_direction = fight_route(units.data(id).cords, c);
				if (!units.data(obj).unit.type.strelyarcher || (fight_field_map[c] == -1 && get_owner(c.x,c.y) != units.data(id).owner))
				{
					if (!kastuyu)
					{
						for (auto dir : fight_direction)
						{
							IntVec2& NowPPos = units.data(id).cords;
							fight_field_map[cords.x][cords.y] = -1;
							if (dir.x > NowPPos.x)
							{
								cords.x++;
								obj.anim.play("right", 0);
							}
							else if (dir.x < NowPPos.x)
							{
								cords.x--;
								obj.anim.play("left", 0);
							}
							else if (dir.y > NowPPos.y)
							{
								cords.y++;
								obj.anim.play("top", 0);
							}
							else if (dir.y < NowPPos.y)
							{
								cords.y--;
								obj.anim.play("down", 0);
							}
							fight_field_map[cords.x][cords.y] = fight_queue[0].second;
						}

					}
					else
					{
						magicAttack();
					}
				}
				else
				{
					if (!kastuyu)
					{
						attack(units.data(obj).unit.type.strelyarcher);
					}
					else
					{
						magicAttack();
					}
				}
				animStart = true;
				moveUnitId = fight_queue[0].second;
			}			
		}
		else
		{
			if (input.justPressed(MouseRight))
			{
				cout << "MouseRight pressed" << endl;
				if (impl::isMouseOn(design.child<Button>("castleMagicBuild1").getImpl().get()))
				{
					auto& but = design.child<Button>("more_info");
					but.show();
					but.setPos(input.mousePos());
					cout << "Mouse on castleMagicBuild1" << endl;
				}
			}
			if (input.justPressed(MouseLeft)/* && !impl::isMouseOn(castle_menu.getImpl().get()) */&& forWindow.empty() && playerLayer.get(0).anim.isEmpty() && !impl::isMouseOn(Relog.getImpl().get()) && !impl::isMouseOn(newDirection.getImpl().get()) && !impl::isMouseOn(heroMenu.getImpl().get()) && !heroMenuOpen)
			{
				if (stepPoints <= 0)
				{
					emptyStep.show();
					timer.start();
					timer.repeat(2);
				}
				else
				{
					emptyStep.hide();
					if (!isZoomDown)
					{
						auto c = cell(field.mousePos());
						if (c.x < groundmap.w && c.y < groundmap.h && c.y >= 0 && c.x >= 0)
						{
							direction = route(cell(playerLayer.get(0).pos()), c);
							for (auto dir : direction)
							{

								auto egg = SuperMegaPuperStepEgg.load("SuperMegaPuperStepEgg.json", pixels(dir));
								SuperMegaPuperStepEgg.data(egg).cords = dir;
							}
						}
					}						
					else
					{
						auto c = cell_zoom(field.mousePos());
						if (c.x < groundmap.w && c.y < groundmap.h && c.y >= 0 && c.x >= 0)
						{
							direction = route(cell_zoom(playerLayer.get(0).pos()), c);
							for (auto dir : direction)
							{
								auto egg = SuperMegaPuperStepEgg.load("SuperMegaPuperStepEgg.json", pixels_zoom(dir));
								egg.setSize(18.5, 18.5);
							}
						}
					}
						
				}
			}
			if (input.justPressed(C))
			{
				clearSteps();
			}
			if (input.justPressed(Space))
			{
				nextStep();
			}
			if (isZoomDown && playerLayer.get(0).anim.isEmpty())
			{
				if (input.justPressed(Add))
				{
					isZoomDown = false;
					zoom(1);
				}
			}
			if (!isZoomDown && playerLayer.get(0).anim.isEmpty())
			{
				if (input.justPressed(Subtract))
				{
					isZoomDown = true;
					zoom(0.25);
				}
			}
		}

		
    }

	IntVec2 cell (Vec2 v)
	{
		v.x += 75;
		v.y += 75;
		v /= 150;
		return IntVec2(v.x, v.y);
	}

	Vec2 pixels(IntVec2 v)
	{
		v.x *= 150;
		v.y *= 150;
		return Vec2(v.x, v.y);
	}

	IntVec2 cell_fight(Vec2 v)
	{
		v.x += cellsize / 2;
		v.y += cellsize / 2;
		v /= cellsize;
		return IntVec2(v.x, v.y);
	}

	Vec2 pixels_fight(IntVec2 v)
	{
		v.x *= cellsize;
		v.y *= cellsize;
		return Vec2(v.x, v.y);
	}

	IntVec2 cell_zoom(Vec2 v)
	{
		v.x += 37.5 / 2;
		v.y += 37.5 / 2;
		v /= 37.5;
		return IntVec2(v.x, v.y);
	}

	IntVec2 cell(Vec2 v, bool isZoom)
	{
		return isZoom ? cell_zoom(v) : cell(v);
	}

	Vec2 pixels_zoom(IntVec2 v)
	{
		v.x *= 37.5;
		v.y *= 37.5;
		return Vec2(v.x, v.y);
	}

	Vec2 pixels(IntVec2 v, bool isZoom)
	{
		return isZoom ? pixels_zoom(v) : pixels(v);
	}

	enum class Target {
		none, castle, mine, fight
	};

	Target target;
	//rgerjgoierijgijeriojgiojreiogjejgriejgiorejgiorejojgoiejogieogijiejgioejgoogjeiogjgjrjgioejgjreijgerjogerjoigrejgjerioger
	deque <IntVec2> route(IntVec2 start, IntVec2 finish)
	{
		GameMap dmap;
		deque <IntVec2> queue;
		dmap = createMap(groundmap.w, groundmap.h);
		target = Target::none;
		if (realrecmap[finish] != noneRec)
		{
			target = Target::castle;
			if (realrecmap[finish] != castle && realrecmap[finish] != castle_entry)
			{
				if (realrecmap[finish] != fight)
				{
					if (realrecmap[finish] == rock)
					{
						target = Target::none;
					}
					else
					{
						target = Target::mine;
					}					
				}
				else
				{
					target = Target::fight;
				}
				
			}
		}
		for (int x = 0; x < dmap.w; x++)
		{
			for (int y = 0; y < dmap.h; y++)
			{
				dmap[x][y] = 2000000001;
			}
		}
		dmap[start] = 0;
		queue.push_front(start);
		IntVec2 cur = start;
		int w = 0;
		while (true)
		{
			w = dmap[cur.x][cur.y] + 1;
			if (dmap.w > cur.x + 1 && dmap[cur.x + 1][cur.y] > w && realrecmap[cur.x + 1][cur.y] == noneRec)
			{
				queue.emplace_front(cur.x + 1, cur.y);
				dmap[cur.x + 1][cur.y] = w;
			}
			if (cur.y > 0 && dmap[cur.x][cur.y-1] > dmap[cur.x][cur.y] && realrecmap[cur.x][cur.y - 1] == noneRec)
			{
				queue.emplace_front(cur.x, cur.y - 1);
				dmap[cur.x][cur.y - 1] = w;
			}
			if (cur.x > 0 && dmap[cur.x-1][cur.y] > dmap[cur.x][cur.y] && realrecmap[cur.x - 1][cur.y] == noneRec)
			{
				queue.emplace_front(cur.x - 1, cur.y);
				dmap[cur.x - 1][cur.y] = w;
			}			
			if (cur.y < dmap.h - 1 && dmap[cur.x][cur.y + 1] > dmap[cur.x][cur.y] && realrecmap[cur.x][cur.y + 1] == noneRec)
			{
				queue.emplace_front(cur.x, cur.y + 1);
				dmap[cur.x][cur.y + 1] = w;
			}
			queue.pop_back();
			if (queue.empty())
			{
				break;
			}	
			cur = queue.back();		
		}
		if (dmap[finish] == 2000000001)
		{
			if (target == Target::none)
			{
				return {};
			}
			else
			{
				finish = find_finish(start, finish, dmap);
			}
		}
		w = dmap[finish];
		cur = finish;
		if (finish == start)
		{
			return {};
		}
		deque <IntVec2> direction = {};
		while (true)
		{
			if (w == 0)
			{
				return direction;
			}
			/*if (w == 2000000001)
			{
				return direction;
			}*/
			direction.push_front(cur);
			if (cur.x > 0 && w > dmap[cur.x - 1][cur.y])
			{
				w--;
				cur.x--;
				continue;
			}
			if (cur.y < dmap.h - 1 && w > dmap[cur.x][cur.y + 1])
			{
				w--;
				cur.y++;
				continue;
			}
			if (dmap.w > cur.x + 1 && w > dmap[cur.x + 1][cur.y])
			{
				w--;
				cur.x++;
				continue;
			}
			if (cur.y > 0 && w > dmap[cur.x][cur.y - 1])
			{
				w--;
				cur.y--;
				continue;
			}
		}
	}

	enum class FightTarget
	{
		none, unit
	};

	FightTarget fightTarget;

	unitsData getUnitData(int x, int y)
	{
		unitsData data;
		if (fight_field_map[x][y] == -1)
		{
			return data;
		}
		data = units.data(fight_field_map[x][y]);
		return data;
	}

	ownerType get_owner(int x, int y)
	{
		ownerType data = noOwner;
		if (fight_field_map[x][y] == -1)
		{
			return data;
		}
		data = units.data(fight_field_map[x][y]).owner;
		return data;
	}

	set <IntVec2> get_fight_zone(IntVec2 start, int step)
	{
		GameMap dmap;
		deque <IntVec2> queue;
		dmap = createMap(fight_field_map.w, fight_field_map.h);
		for (int x = 0; x < dmap.w; x++)
		{
			for (int y = 0; y < dmap.h; y++)
			{
				dmap[x][y] = 2000000001;
			}
		}
		dmap[start] = 0;
		auto a = getUnitData(start.x, start.y);
		queue.push_front(start);
		IntVec2 cur = start;
		int w = 0;
		set <IntVec2> zone;
		while (true)
		{
			w = dmap[cur.x][cur.y] + 1;
			if (w > step)
			{
				break;
			}
			if (dmap.w > cur.x + 1 && dmap[cur.x + 1][cur.y] > w && get_owner(start.x, start.y) != get_owner(cur.x + 1, cur.y))
			{
				if (get_owner(cur.x + 1, cur.y) == noOwner || get_owner(cur.x + 1, cur.y) == neutral)
				{
					queue.emplace_front(cur.x + 1, cur.y);
					dmap[cur.x + 1][cur.y] = w;
				}
				zone.insert({ cur.x + 1, cur.y });
			}
			if (cur.x > 0 && dmap[cur.x - 1][cur.y] > dmap[cur.x][cur.y] && get_owner(start.x, start.y) != get_owner(cur.x - 1, cur.y))
			{
				if (get_owner(cur.x - 1, cur.y) == noOwner || get_owner(cur.x - 1, cur.y) == neutral)
				{
					queue.emplace_front(cur.x - 1, cur.y);
					dmap[cur.x - 1][cur.y] = w;
				}
				zone.insert({ cur.x - 1, cur.y });
			}
			if (cur.y > 0 && dmap[cur.x][cur.y - 1] > dmap[cur.x][cur.y] && get_owner(start.x, start.y) != get_owner(cur.x, cur.y - 1))
			{
				if (get_owner(cur.x, cur.y - 1) == noOwner || get_owner(cur.x, cur.y - 1) == neutral)
				{
					queue.emplace_front(cur.x, cur.y - 1);
					dmap[cur.x][cur.y - 1] = w;
				}
				zone.insert({ cur.x, cur.y - 1 });
			}
			if (cur.y < dmap.h - 1 && dmap[cur.x][cur.y + 1] > dmap[cur.x][cur.y] && get_owner(start.x, start.y) != get_owner(cur.x, cur.y + 1))
			{
				if (get_owner(cur.x, cur.y + 1) == noOwner || get_owner(cur.x, cur.y + 1) == neutral)
				{
					queue.emplace_front(cur.x, cur.y + 1);
					dmap[cur.x][cur.y + 1] = w;
				}
				zone.insert({ cur.x, cur.y + 1 });
			}
			queue.pop_back();
			if (queue.empty())
			{
				break;
			}
			cur = queue.back();
		}

		auto type = a.unit.type.strelyarcher;

		auto type2 = a.owner;

		if (type)
		{
			for (auto nit : units.all())
			{
				auto& data = units.data(nit);
				if (data.owner != type2)
				{
					zone.insert(data.cords);
				}
			}
		}

		return zone;
	}

	int attackUnitId, protectUnitId;
	bool isAttacking = false;

	int get_target()
	{
		for (auto nit : units.all())
		{
			
		}
	}

	bool isValidPoint(GameMap& map, IntVec2 point)
	{
		if (map.w > point.x && map.h > point.y && point.y >= 0 && point.x >= 0)
		{
			if (map[point] == 2000000001)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}

	}

	IntVec2 find_finish(IntVec2 start, IntVec2 finish, GameMap& dmap)
	{
		int n = 1;
		IntVec2 startOffset;
		IntVec2 dir;
		if (start.x > finish.x)
		{
			startOffset = { -1, 0 };
			dir = { 1, 1 };
		}
		if (start.x < finish.x)
		{
			startOffset = { 1, 0 };
			dir = { -1, -1 };
		}
		if (start.y > finish.y)
		{
			startOffset = { 0, 1 };
			dir = { 1, -1 };
		}
		if (start.y < finish.y)
		{
			startOffset = { 0, -1 };
			dir = { -1, 1 };
		}
		while (true)
		{
			/*

			if (start.x > finish.x && dmap[finish.x + 1][finish.y] == 2000000001)
			{
				finish = { finish.x + 1, finish.y };
			}
			if (start.x < finish.x && dmap[finish.x - 1][finish.y] == 2000000001)
			{
				finish = { finish.x - 1, finish.y };
			}
			if (start.y > finish.y && dmap[finish.x][finish.y + 1] == 2000000001)
			{
				finish = { finish.x, finish.y + 1 };
			}
			if (start.y < finish.y && dmap[finish.x][finish.y - 1] == 2000000001)
			{
				finish = { finish.x, finish.y - 1 };
			}
			if (dmap[finish] == 2000000001)
			{

			}*/
			IntVec2 cur = { finish.x - startOffset.x * n, finish.y + startOffset.y * n};
			for (int i = 0; i < n; i++)
			{
				if (isValidPoint(dmap, cur))
				{
					return cur;
				}
				cur += dir;
			}
			dir = rotate90CW(dir);
			for (int i = 0; i < n; i++)
			{
				if (isValidPoint(dmap, cur))
				{
					return cur;
				}
				cur += dir;
			}
			dir = rotate90CW(dir);
			for (int i = 0; i < n; i++)
			{
				if (isValidPoint(dmap, cur))
				{
					return cur;
				}
				cur += dir;
			}
			dir = rotate90CW(dir);
			for (int i = 0; i < n - 1; i++)
			{
				if (isValidPoint(dmap, cur))
				{
					return cur;
				}
				cur += dir;
			}
			dir = rotate90CW(dir);
			n++;
		}
	}

	deque <IntVec2> fight_route(IntVec2 start, IntVec2 finish)
	{
		GameMap dmap;
		deque <IntVec2> queue;
		dmap = createMap(fight_field_map.w, fight_field_map.h);
		ownerType owner = get_owner(start.x, start.y);
		fightTarget = FightTarget::none;
		if (fight_field_map[finish] != -1)
		{
			attackUnitId = fight_field_map[start.x][start.y];
			protectUnitId = fight_field_map[finish.x][finish.y];
			isAttacking = true;
			fightTarget = FightTarget::unit;
		}
		else
		{
			isAttacking = false;
			fightTarget = FightTarget::none;
		}
		for (int x = 0; x < dmap.w; x++)
		{
			for (int y = 0; y < dmap.h; y++)
			{
				dmap[x][y] = 2000000001;
			}
		}
		dmap[start] = 0;
		queue.push_front(start);
		IntVec2 cur = start;
		int w = 0;
		while (true)
		{
			w = dmap[cur.x][cur.y] + 1;
			if (dmap.w > cur.x + 1 && dmap[cur.x + 1][cur.y] > w && fight_field_map[cur.x + 1][cur.y] == -1)
			{
				queue.emplace_front(cur.x + 1, cur.y);
				dmap[cur.x + 1][cur.y] = w;
			}
			if (cur.y > 0 && dmap[cur.x][cur.y - 1] > w && fight_field_map[cur.x][cur.y - 1] == -1)
			{
				queue.emplace_front(cur.x, cur.y - 1);
				dmap[cur.x][cur.y - 1] = w;
			}
			if (cur.x > 0 && dmap[cur.x - 1][cur.y] > w && fight_field_map[cur.x - 1][cur.y] == -1)
			{
				queue.emplace_front(cur.x - 1, cur.y);
				dmap[cur.x - 1][cur.y] = w;
			}
			if (cur.y < dmap.h - 1 && dmap[cur.x][cur.y + 1] > w && fight_field_map[cur.x][cur.y + 1] == -1)
			{
				queue.emplace_front(cur.x, cur.y + 1);
				dmap[cur.x][cur.y + 1] = w;
			}
			queue.pop_back();
			if (queue.empty())
			{
				break;
			}
			cur = queue.back();
		}
		if (dmap[finish] == 2000000001)
		{
			if (fightTarget == FightTarget::none)
			{
				return {};
			}
			else
			{
				finish = find_finish(start, finish, dmap);
			}
		}
		w = dmap[finish];
		cur = finish;
		if (finish == start)
		{
			return {};
		}
		deque <IntVec2> direction = {};
		while (true)
		{
			if (w == 0)
			{
				return direction;
			}
			/*if (w == 2000000001)
			{
				return direction;
			}*/
			direction.push_front(cur);
			if (cur.x > 0 && w > dmap[cur.x - 1][cur.y])
			{
				w--;
				cur.x--;
				continue;
			}
			if (cur.y < dmap.h - 1 && w > dmap[cur.x][cur.y + 1])
			{
				w--;
				cur.y++;
				continue;
			}
			if (dmap.w > cur.x + 1 && w > dmap[cur.x + 1][cur.y])
			{
				w--;
				cur.x++;
				continue;
			}
			if (cur.y > 0 && w > dmap[cur.x][cur.y - 1])
			{
				w--;
				cur.y--;
				continue;
			}
		}
	}
	
	enum Babosiki
	{
		g, e
	};

	void getBabosiki(Babosiki b, int n)
	{
		if (b == g)
		{
			gold += n;
			forWindow.remove(1);
		}
		else
		{
			playerLayer.data(playerLayer.get(0)).exp += n;
			skill += n;
			forWindow.remove(1);
		}
	}

	void showChestMenu()
	{
		randomize();
		forWindow.load(1,"ChestMenu.json");
		auto getExp = forWindow.child<Button>("getExp");
		auto getGold = forWindow.child<Button>("getGold");
		auto expN = forWindow.child<Label>("expN");
		auto goldN = forWindow.child<Label>("goldN");
		int a = randomInt(1, 5);
		if (a >= 1 && a < 4)
		{
			connect(getExp, getBabosiki, e, 800);
			connect(getGold, getBabosiki, g, 800);
			expN << tr("get800exp");
			goldN << tr("get800gold");
		}
		if (a >= 4)
		{	
			connect(getGold, getBabosiki, g, 2000);
			connect(getExp, getBabosiki, e, 2000);
			expN << tr("get2000exp");
			goldN << tr("get2000gold");
		}
		design.update();
	}

	bool heroStay(GameObj hero)
	{
		if (hero.anim.isEmpty() && empty(direction))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool isMineInfoOpen = false;

	void closeMineInfo()
	{
		forWindow.remove(2);
		isMineInfoOpen = false;
	}

	void CloseCastleMenu(GameObj castle)
	{
		forWindow.remove(3);
		heroMenu.show();
		rec.data(castle).heroInCastle = false;
		loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
		design.update();
	}

	enum Place
	{
		main, getArmy, build
	};
	
	void more(int n, GameObj castle)
	{
		int unitN = 0;
		design.child<TextBox>("unitN") >> unitN;
		if (unitN < 0)
		{
			unitN = 0;
		}
		//int unitPrice;
		vector<Price> unitPrice = allTypes[n - 1].price;
		if (n != 6)
		{
			unitPrice[0].n = allTypes[n - 1].price[0].n;
			int price = unitPrice[0].n * unitN;
			int canBuy = gold - price;
			if (rec.data(castle).castleArmy.units[n - 1].n <= unitN || canBuy < unitPrice[0].n)
			{
				return;
			}
			unitN++;
			price += unitPrice[0].n;
			design.child<TextBox>("unitN") << unitN;
			design.child<Label>("allCost") << tr("allCost") << price;
		}
		else
		{
			vector<Price> price = allTypes[n - 1].price;
			unitPrice[0].n = allTypes[n - 1].price[0].n;
			unitPrice[1].n = allTypes[n - 1].price[1].n;
			price[0].n = unitPrice[0].n * unitN;
			price[1].n = unitPrice[1].n * unitN;
			int canBuyGold = gold - price[0].n;
			int canBuyGem = gems - price[1].n;
			if(rec.data(castle).castleArmy.units[n - 1].n <= unitN || canBuyGold < unitPrice[0].n || canBuyGem < unitPrice[1].n)
			{
				return;
			}
			unitN++; 
			price[0].n += unitPrice[0].n;
			price[1].n += unitPrice[1].n;
			design.child<TextBox>("unitN") << unitN;
			design.child<Label>("allCost") << tr("allCost") << tr("goldi") << price[0].n << tr("gemov") << price[1].n;
		}
		auto buyBut = design.child<Button>("buyArmyButton");
		connect(buyBut, buyUnit, n, unitN, castle);
		design.update();
	}	

	void min(int n, GameObj castle)
	{
		int unitN = 0;
		design.child<TextBox>("unitN") >> unitN;
		//int unitPrice;
		if (unitN < 0)
		{
			unitN = 0;
		}
		vector<Price> unitPrice = allTypes[n - 1].price;
		if (n != 6)
		{
			unitPrice[0].n = allTypes[n - 1].price[0].n;
			int price = unitPrice[0].n * unitN;
			int canBuy = gold - price;
			if (unitN < 1)
			{
				return;
			}
			unitN--;
			price -= unitPrice[0].n;
			design.child<TextBox>("unitN") << unitN;
			design.child<Label>("allCost") << tr("allCost") << price;
		}
		else
		{
			vector<Price> price = allTypes[n - 1].price;
			unitPrice[0].n = allTypes[n - 1].price[0].n;
			unitPrice[1].n = allTypes[n - 1].price[1].n;
			price[0].n = unitPrice[0].n * unitN;
			price[1].n = unitPrice[1].n * unitN;
			if (unitN < 1)
			{
				return;
			}
			unitN--;
			price[0].n -= unitPrice[0].n;
			price[1].n -= unitPrice[1].n;
			design.child<TextBox>("unitN") << unitN;
			design.child<Label>("allCost") << tr("allCost") << tr("goldi") << price[0].n << tr("gemov") << price[1].n;
		}
		auto buyBut = design.child<Button>("buyArmyButton");
		connect(buyBut, buyUnit, n, unitN, castle);
		design.update();
	}
	
	void changeTextBox (GameObj castle, int n)
	{
		int unitN = 0;
		design.child<TextBox>("unitN") >> unitN;
		if (unitN < 0)
		{
			unitN = 0;
		}
		if (rec.data(castle).castleArmy.units[n - 1].n <= unitN)
		{
			design.child<TextBox>("unitN") << rec.data(castle).castleArmy.units[n - 1].n;
			design.child<TextBox>("unitN") >> unitN;
		}
		
		if (n != 6)
		{
			int unitPrice;
			unitPrice = allTypes[n - 1].price[0].n;
			int price = unitPrice * unitN;

			if (gold < price)
			{
				unitN = gold / unitPrice;
			}

			price = unitN * unitPrice;
			design.child<Label>("allCost") << tr("allCost") << price;
		}
		else
		{

			vector <Price> unitPrice = allTypes[n - 1].price;
			vector <Price> price = allTypes[n - 1].price;
			price[0].n = unitPrice[0].n * unitN;
			price[1].n = unitPrice[1].n * unitN;
			if (gold < price[0].n || gems < price[1].n)
			{
				unitN = gold / unitPrice[0].n;
				price[1].n = unitPrice[1].n * unitN;
				if (gems < price[1].n)
				{
					unitN = gems / unitPrice[1].n;
				}
			}
			price[0].n = unitPrice[0].n * unitN;
			price[1].n = unitPrice[1].n * unitN;
			design.child<Label>("allCost") << tr("allCost") << tr("goldi") << price[0].n << tr("gemov") << price[1].n;
			design.child<TextBox>("unitN") << unitN;
		}
		auto buyBut = design.child<Button>("buyArmyButton");
		connect(buyBut, buyUnit, n, unitN, castle);
		//cout << "textBoxChanged" << endl;
		design.child<TextBox>("unitN") << unitN;
		design.update();
	}

	void toCastle(int n, int unitN, GameObj castle)
	{
		rec.data(castle).protectCastleArmy.units[n - 1].n += unitN;
		design.child<Layout>("buyPlace").hide();
	}

	void toHeroArmy(int n, int unitN)
	{
		playerLayer.data(playerLayer.get(0)).army.units[n - 1].n += unitN;
		design.child<Layout>("buyPlace").hide();
	}

	void buyUnit(int n, int unitN, GameObj castle)
	{
		if (n != 6)
		{
			int price = 0;
			int unitPrice = allTypes[n - 1].price[0].n;
			price = unitN * unitPrice;
			if (price > gold)
			{
				return;
			}
			else
			{
				gold -= price;
			}	
		}
		else
		{
			vector<Price> price = allTypes[n - 1].price;
			vector<Price> unitPrice = allTypes[n - 1].price;
			price[0].n = unitPrice[0].n * unitN;
			price[1].n = unitPrice[1].n * unitN;
			if (price[0].n > gold || price[1].n > gems)
			{
				return;
			}
			else
			{
				gold -= price[0].n;
				gems -= price[1].n;
			}
		}
		rec.data(castle).castleArmy.units[n - 1].n -= unitN;
		if (rec.data(castle).heroInCastle)
		{
			connect(design.child<Button>("toCastle"), toCastle, n, unitN, castle);
			connect(design.child<Button>("toHeroArmy"), toHeroArmy, n, unitN);
			design.child<Layout>("buyPlace").show();
		}
		else
		{
			toCastle(n, unitN, castle);
		}
		auto allCost = design.child<Label>("allCost");
		design.child<TextBox>("unitN") << "0";
		allCost << tr("allCost") << "0";
		design.child<Label>("canBuyUnitN") << tr("canBuy") << " " << rec.data(castle).castleArmy.units[n - 1].n;
		design.update();
	}

	void getUnit(int n, GameObj castle)
	{
		if (!design.child<ToggleButton>("castleunit" + toString(n)).isPressed())
		{
			return;
		}
		for (int i = 1; i < 7; i++)
		{
			if (i != n)
			{
				design.child<ToggleButton>("castleunit" + toString(i)).unpress();
			}			
		}
		auto sw = design.child<Layout>("shopWindow");
		sw.show();
		auto oneCost = sw.child<Label>("costForOne");
		auto allCost = sw.child<Label>("allCost");
		allCost << tr("allCost") << "0";
		auto buyBut = sw.child<Button>("buyArmyButton");
		design.child<TextBox>("unitN") << "0";
		sw.child<Label>("canBuyUnitN") << tr("canBuy") << " " << rec.data(castle).castleArmy.units[n - 1].n;
		connect(sw.child<Button>("more"), more, n, castle);
		connect(sw.child<Button>("min"), min, n, castle);
		connect(design.child<TextBox>("unitN"), changeTextBox, castle, n);
		if (n != 6)
		{
			oneCost << tr("oneCost") << allTypes[n - 1].price[0].n;
		}
		else 
		{
			oneCost << tr("oneCost") << tr("goldi") << allTypes[n - 1].price[0].n << " " << tr("gemov") << allTypes[n - 1].price[1].n;
		}
		design.update();
	}

	void closeBuildInfo()
	{
		design.child<Layout>("buildInfo").hide();
		design.update();
	}

	void viewMoreBuildInfo(int branch, int stage, GameObj castle) 
	{
		auto window = design.child<Layout>("buildInfo");
		window.show();
		connect(window.child<Button>("closeBuildInfo"), closeBuildInfo);
		auto name = window.child<Label>("name");
		auto cost = window.child<Label>("cost");
		auto description = window.child<Label>("description");
		vector <Price> price;
		switch (branch)
		{
		case 0:
			name << rec.data(castle).castleRec.magicBranch[stage - 1].name;
			description << rec.data(castle).castleRec.magicBranch[stage - 1].description;
			price = rec.data(castle).castleRec.magicBranch[stage - 1].price;
			break;
		case 1:
			name << rec.data(castle).castleRec.armyBranch[stage - 1].name;
			description << rec.data(castle).castleRec.armyBranch[stage - 1].description;
			price = rec.data(castle).castleRec.armyBranch[stage - 1].price;			
			break;
		case 2:
			name << rec.data(castle).castleRec.ecoBranch[stage - 1].name;
			description << rec.data(castle).castleRec.ecoBranch[stage - 1].description;
			price = rec.data(castle).castleRec.ecoBranch[stage - 1].price;
			break;
		}
		for (auto buildprice : price)
		{
			if (buildprice.n == 0)
			{
				continue;
			}
			else
			{
				if (buildprice.type == Price::gold)
				{
					cost << tr("gold") << buildprice.n;
				}
				if (buildprice.type == Price::ore)
				{
					cost << tr("ore") << buildprice.n;
				}
				if (buildprice.type == Price::wood)
				{
					cost << tr("wood") << buildprice.n;
				}
				if (buildprice.type == Price::crystal)
				{
					cost << tr("gems") << buildprice.n;
				}
				if (buildprice.type == Price::sera)
				{
					cost << tr("sera") << buildprice.n;
				}
			}
		}
		design.update();
	}

	void Build (int stage, int branch, GameObj castle)
	{
		auto win = design.child<Layout>("buildInfo");
		bool canBuy = false;
		int gold_, ore_, gems_, sera_, wood_;

		gold_ = gold;
		ore_ = ore;
		gems_ = gems;
		sera_ = sera;
		wood_ = wood;

		if (branch == 0)
		{
			for (auto Price : rec.data(castle).castleRec.magicBranch[stage].price)
			{
				if (Price.n != 0)
				{
					if (Price.type == Price::crystal)
					{
						if (Price.n < gems)
						{
							gems_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("gems") << " : " << Price.n;
					}
					else if (Price.type == Price::wood)
					{
						if (Price.n < wood)
						{
							wood_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("wood") << " : " << Price.n;
					}
					else if (Price.type == Price::ore)
					{
						if (Price.n < ore)
						{
							ore_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}

						//win.child<Label>("cost") << tr("ore") << " : " << Price.n;
					}
					else if (Price.type == Price::gold)
					{
						if (Price.n < gold)
						{
							gold_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("gold") << " : " << Price.n;
					}
					else if (Price.type == Price::sera)
					{
						if (Price.n < sera)
						{
							sera_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("sera") << " : " << Price.n;
					}

				}
			}
		}
		else if (branch == 1)
		{
			for (auto Price : rec.data(castle).castleRec.armyBranch[stage].price)
			{
				if (Price.n != 0)
				{
					if (Price.type == Price::crystal)
					{
						//cost += tr("gems") + " : " + toString(Price.n) + " , ";
						if (Price.n < gems)
						{
							gems_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("gems") << " : " << Price.n;
					}
					else if (Price.type == Price::wood)
					{
						//cost += tr("wood") + " : " + toString(Price.n) + " , ";
						if (Price.n < wood)
						{
							wood_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("wood") << " : " << Price.n;
					}
					else if (Price.type == Price::ore)
					{
						//cost += tr("ore") + " : " + toString(Price.n) + " , ";
						if (Price.n < ore)
						{
							ore_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}

						//win.child<Label>("cost") << tr("ore") << " : " << Price.n;
					}
					else if (Price.type == Price::gold)
					{
						//cost += tr("gold") + " : " + toString(Price.n) + " , ";
						if (Price.n < gold)
						{
							gold_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("gold") << " : " << Price.n;
					}
					else if (Price.type == Price::sera)
					{
						//cost += tr("sera") + " : " + toString(Price.n) + " , ";
						if (Price.n < sera)
						{
							sera_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("sera") << " : " << Price.n;
					}

				}
			}
		}
		else if (branch == 2)
		{
			for (auto Price : rec.data(castle).castleRec.ecoBranch[stage].price)
			{
				if (Price.n != 0)
				{
					if (Price.type == Price::crystal)
					{
						//cost += tr("gems") + " : " + toString(Price.n) + " , ";
						if (Price.n < gems)
						{
							gems_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("gems") << " : " << Price.n;
					}
					else if (Price.type == Price::wood)
					{
						//cost += tr("wood") + " : " + toString(Price.n) + " , ";
						if (Price.n < wood)
						{
							wood_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("wood") << " : " << Price.n;
					}
					else if (Price.type == Price::ore)
					{
						//cost += tr("ore") + " : " + toString(Price.n) + " , ";
						if (Price.n < ore)
						{
							ore_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}

						//win.child<Label>("cost") << tr("ore") << " : " << Price.n;
					}
					else if (Price.type == Price::gold)
					{
						//cost += tr("gold") + " : " + toString(Price.n) + " , ";
						if (Price.n < gold)
						{
							gold_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("gold") << " : " << Price.n;
					}
					else if (Price.type == Price::sera)
					{
						//cost += tr("sera") + " : " + toString(Price.n) + " , ";
						if (Price.n < sera)
						{
							sera_ -= Price.n;
							canBuy = true;
						}
						else
						{
							canBuy = false;
							break;
						}
						//win.child<Label>("cost") << tr("sera") << " : " << Price.n;
					}

				}
			}
		}
		if (canBuy)
		{
			gold = gold_;
			ore = ore_;
			gems = gems_;
			sera = sera_;
			wood = wood_;
			design.child<Button>("buyBuilding").hide();

			if (branch == 0)
			{
				rec.data(castle).castleRec.magicBranch[stage].isBuild = IsBuild::build;
			}
			else if (branch == 1)
			{
				rec.data(castle).castleRec.armyBranch[stage].isBuild = IsBuild::build;
			}
			else if (branch == 2)
			{
				rec.data(castle).castleRec.ecoBranch[stage].isBuild = IsBuild::build;
			}

			int i = 1;
			for (auto recShow : rec.data(castle).castleRec.armyBranch)
			{
				if (recShow.isBuild == IsBuild::build)
				{
					design.child<ToggleButton>("castleunit" + toString(i)).show();
				}
				i++;
			}

			auto& magicbranch = rec.data(castle).castleRec.magicBranch;
			auto& armybranch = rec.data(castle).castleRec.armyBranch;
			auto& ecobranch = rec.data(castle).castleRec.ecoBranch;
			design.child<Layout>("MagicBranch").clear();
			design.child<Layout>("ArmyBranch").clear();
			design.child<Layout>("EcoBranch").clear();
			i = 0;
			for (auto Build : magicbranch)
			{
				auto& branch = design.child<Layout>("MagicBranch");
				auto& build = branch.load<Selector>("buildButton.json");
				string Icon_gray = Build.icon_name;
				string Icon_notgray = Build.icon_name;
				if (Build.isBuild == IsBuild::build)
				{
					build.select(2);
				}
				Icon_gray += "_gray.png";
				Icon_notgray += ".png";
				build.child<Button>("gray").child<Texture>("icon").setImageName(Icon_gray);
				build.child<Button>("not_gray").child<Texture>("icon").setImageName(Icon_notgray);
				connect(build.child<Button>("gray"), castleBuild, i, 0, castle);
				connect(build.child<Button>("not_gray"), castleBuild, i, 0, castle);
				i++;
			}
			i = 0;
			for (auto Build : armybranch)
			{
				auto& branch = design.child<Layout>("ArmyBranch");
				auto& build = branch.load<Selector>("buildButton.json");
				string Icon_gray = Build.icon_name;
				string Icon_notgray = Build.icon_name;
				Icon_gray += "_gray.png";
				Icon_notgray += ".png";
				build.child<Button>("gray").child<Texture>("icon").setImageName(Icon_gray);
				build.child<Button>("not_gray").child<Texture>("icon").setImageName(Icon_notgray);
				if (Build.isBuild == IsBuild::build)
				{
					build.select(2);
				}
				connect(build.child<Button>("gray"), castleBuild, i, 1, castle);
				connect(build.child<Button>("not_gray"), castleBuild, i, 1, castle);
				i++;
			}
			i = 0;
			for (auto Build : ecobranch)
			{
				auto& branch = design.child<Layout>("EcoBranch");
				auto& build = branch.load<Selector>("buildButton.json");
				string Icon_gray = Build.icon_name;
				string Icon_notgray = Build.icon_name;
				Icon_gray += "_gray.png";
				Icon_notgray += ".png";
				if (Build.isBuild == IsBuild::build)
				{
					build.select(2);
				}
				build.child<Button>("gray").child<Texture>("icon").setImageName(Icon_gray);
				build.child<Button>("not_gray").child<Texture>("icon").setImageName(Icon_notgray);
				connect(build.child<Button>("gray"), castleBuild, i, 2, castle);
				connect(build.child<Button>("not_gray"), castleBuild, i, 2, castle);
				i++;
			}
			connect(design.child<Button>("knopka_vihod"), changeCastleMenuSelector, design.child<Selector>("castleMenuSelector"), main, castle);
			design.update();
		}
	}

	void castleBuild(int stage, int branch, GameObj castle)
	{
		loadTextBank("textBankBranch.json");
		if (branch == 0) // magic
		{
			cout << "Button was pressed" << endl;
			auto win = design.child<Layout>("buildInfo");
			win.show();
			win.child<Label>("name") << tr(toString(branch) + toString(stage + 1) + "_name");
			win.child<Label>("description") << tr(toString(branch) + toString(stage + 1));
			string cost;
			loadTextBank("textBank.json");
			if (rec.data(castle).castleRec.magicBranch[stage].isBuild == IsBuild::build)
			{
				win.child<Button>("buyBuilding").hide();
			}
			else
			{
				if (rec.data(castle).castleRec.magicBranch[stage - 1].isBuild == IsBuild::build)
				{
					win.child<Button>("buyBuilding").show();
				}
				else
				{
					win.child<Button>("buyBuilding").hide();
				}
				connect(win.child<Button>("buyBuilding"), Build ,stage, branch, castle);
			}
			for (auto Price : rec.data(castle).castleRec.magicBranch[stage].price)
			{
				if (Price.n != 0)
				{
					if (Price.type == Price::crystal)
					{
						cost += tr("gems") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("gems") << " : " << Price.n;
					}
					else if (Price.type == Price::wood)
					{
						cost += tr("wood") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("wood") << " : " << Price.n;
					}
					else if (Price.type == Price::ore)
					{
						cost += tr("ore") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("ore") << " : " << Price.n;
					}
					else if (Price.type == Price::gold)
					{
						cost += tr("gold") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("gold") << " : " << Price.n;
					}
					else if (Price.type == Price::sera)
					{
						cost += tr("sera") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("sera") << " : " << Price.n;
					}
				}
			}
			win.child<Label>("cost") << cost;
			//cout << "Stage: " << stage << " branch: " << branch << endl;
		}
		else if (branch == 1) // army
		{
			cout << "Button was pressed" << endl;
			auto win = design.child<Layout>("buildInfo");
			win.show();
			win.child<Label>("name") << tr(toString(branch) + toString(stage + 1) + "_name");
			win.child<Label>("description") << tr(toString(branch) + toString(stage + 1));
			string cost = "";
			loadTextBank("textBank.json");
			if (rec.data(castle).castleRec.armyBranch[stage].isBuild == IsBuild::build)
			{
				win.child<Button>("buyBuilding").hide();
			}
			else
			{
				if (rec.data(castle).castleRec.armyBranch[stage - 1].isBuild == IsBuild::build)
				{
					win.child<Button>("buyBuilding").show();
				}
				else
				{
					win.child<Button>("buyBuilding").hide();
				}
				connect(win.child<Button>("buyBuilding"), Build, stage, branch, castle);
			}
			for (auto Price : rec.data(castle).castleRec.armyBranch[stage].price)
			{
				if (Price.n != 0)
				{
					if (Price.type == Price::crystal)
					{
						cost += tr("gems") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("gems") << " : " << Price.n;
					}
					else if (Price.type == Price::wood)
					{
						cost += tr("wood") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("wood") << " : " << Price.n;
					}
					else if (Price.type == Price::ore)
					{
						cost += tr("ore") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("ore") << " : " << Price.n;
					}
					else if (Price.type == Price::gold)
					{
						cost += tr("gold") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("gold") << " : " << Price.n;
					}
					else if (Price.type == Price::sera)
					{
						cost += tr("sera") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("sera") << " : " << Price.n;
					}
				}
			}
			win.child<Label>("cost") << cost;
			//cout << "Stage: " << stage << " branch: " << branch << endl;
		}
		else if (branch == 2) // eco
		{
			cout << "Button was pressed" << endl;
			auto win = design.child<Layout>("buildInfo");
			win.show();
			win.child<Label>("name") << tr(toString(branch) + toString(stage) + "_name");
			win.child<Label>("description") << tr(toString(branch) + toString(stage));
			string cost;
			loadTextBank("textBank.json");
			if (rec.data(castle).castleRec.ecoBranch[stage].isBuild == IsBuild::build)
			{
				win.child<Button>("buyBuilding").hide();
			}
			else
			{

				if (rec.data(castle).castleRec.ecoBranch[stage - 1].isBuild == IsBuild::build)
				{
					win.child<Button>("buyBuilding").show();
				}
				else
				{
					win.child<Button>("buyBuilding").hide();
				}
				connect(win.child<Button>("buyBuilding"), Build, stage, branch, castle);
			}
			for (auto Price : rec.data(castle).castleRec.ecoBranch[stage].price)
			{
				if (Price.n != 0)
				{
					if (Price.type == Price::crystal)
					{
						cost += tr("gems") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("gems") << " : " << Price.n;
					}
					else if (Price.type == Price::wood)
					{
						cost += tr("wood") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("wood") << " : " << Price.n;
					}
					else if (Price.type == Price::ore)
					{
						cost += tr("ore") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("ore") << " : " << Price.n;
					}
					else if (Price.type == Price::gold)
					{
						cost += tr("gold") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("gold") << " : " << Price.n;
					}
					else if (Price.type == Price::sera)
					{
						cost += tr("sera") + " : " + toString(Price.n) + " , ";
						win.child<Label>("cost") << tr("sera") << " : " << Price.n;
					}
				}
			}
			win.child<Label>("cost") << cost;
			//cout << "Stage: " << stage << " branch: " << branch << endl;
		}

		connect(design.child<Button>("closeBuildInfo"), closeBuildInfo);
		connect(design.child<Button>("closeBuildInfo"), closeBuildInfo);
		design.update();
	}

	void changeCastleMenuSelector(Selector selector, Place place, GameObj castle)
	{
		if (place == main)
		{
			selector.select(0);
			loadArmyCastle(rec.data(castle).protectCastleArmy, castle);
			loadArmyHeroCastle(playerLayer.data(playerLayer.get(0)).army, castle);
			design.update();
		}
		else if (place == getArmy)
		{
			selector.select(1);
			design.update();
			int i = 1;
			for (auto recShow : rec.data(castle).castleRec.armyBranch)
			{
				if (recShow.isBuild == IsBuild::build)
				{
					design.child<ToggleButton>("castleunit" + toString(i)).show();
				}
				i++;
			}
			for (int i = 1; i < 7; i++)
			{
				connect(design.child<ToggleButton>("castleunit" + toString(i)), getUnit, i, castle);
			}
		}
		else if (place == build)
		{
			auto& magicbranch = rec.data(castle).castleRec.magicBranch;
			auto& armybranch = rec.data(castle).castleRec.armyBranch;
			auto& ecobranch = rec.data(castle).castleRec.ecoBranch;
			selector.select(2);
			int i = 0;
			int j = 0;
			design.child<Layout>("MagicBranch").clear();
			design.child<Layout>("ArmyBranch").clear();
			design.child<Layout>("EcoBranch").clear();
			//design.child<Layout>("MagicBranchTitle").clear();
			//design.child<Layout>("ArmyBranchTitle").clear();
			//design.child<Layout>("EcoBranchGGWPTitle").clear();
			for (auto Build : magicbranch)
			{
				//if ()
				auto& branch = design.child<Layout>("MagicBranch");
				auto& build = branch.load<Selector>("buildButton.json");
				string Icon_gray = Build.icon_name;
				string Icon_notgray = Build.icon_name;
				if (Build.isBuild == IsBuild::build)
				{
					build.select(2);
					j++;
				}
				else
				{
					if (rec.data(castle).castleRec.armyBranch[i - 1].isBuild == IsBuild::build)
					{
						design.child<Button>("buyBuilding").hide();
					}
				}
				Icon_gray += "_gray.png";
				Icon_notgray += ".png";
				build.child<Button>("gray").child<Texture>("icon").setImageName(Icon_gray);
				build.child<Button>("not_gray").child<Texture>("icon").setImageName(Icon_notgray);
				connect(build.child<Button>("gray"), castleBuild, i, 0, castle);
				connect(build.child<Button>("not_gray"), castleBuild, i, 0, castle);
				i++;
			}
			i = 0;
			j = 0;
			for (auto Build : armybranch)
			{
				auto& branch = design.child<Layout>("ArmyBranch");
				auto& build = branch.load<Selector>("buildButton.json");
				string Icon_gray = Build.icon_name;
				string Icon_notgray = Build.icon_name;
				Icon_gray += "_gray.png";
				Icon_notgray += ".png";
				build.child<Button>("gray").child<Texture>("icon").setImageName(Icon_gray);
				build.child<Button>("not_gray").child<Texture>("icon").setImageName(Icon_notgray);
				if (Build.isBuild == IsBuild::build)
				{
					build.select(2);
					j++;
				}
				else
				{
					if (rec.data(castle).castleRec.armyBranch[i - 1].isBuild == IsBuild::build)
					{
						design.child<Button>("buyBuilding").hide();
					}
				}
				connect(build.child<Button>("gray"), castleBuild, i, 1, castle);
				connect(build.child<Button>("not_gray"), castleBuild, i, 1, castle);
				i++;
			}
			i = 0;
			j = 0;
			for (auto Build : ecobranch)
			{
				auto& branch = design.child<Layout>("EcoBranch");
				auto& build = branch.load<Selector>("buildButton.json");
				string Icon_gray = Build.icon_name;
				string Icon_notgray = Build.icon_name;
				Icon_gray += "_gray.png";
				Icon_notgray += ".png";
				if (Build.isBuild == IsBuild::build)
				{
					build.select(2);
					j++;
				}
				else
				{
					if (rec.data(castle).castleRec.armyBranch[i - 1].isBuild == IsBuild::build)
					{
						design.child<Button>("buyBuilding").hide();
					}
				}
				build.child<Button>("gray").child<Texture>("icon").setImageName(Icon_gray);
				build.child<Button>("not_gray").child<Texture>("icon").setImageName(Icon_notgray);
				connect(build.child<Button>("gray"), castleBuild, i, 2, castle);
				connect(build.child<Button>("not_gray"), castleBuild, i, 2, castle);
				i++;
			}
			connect(design.child<Button>("knopka_vihod"), changeCastleMenuSelector, design.child<Selector>("castleMenuSelector"), main, castle);
			design.update();
		}
	}

	void openCastleMenu(GameObj castle)
	{
		auto cm = forWindow.load(3, "castleMenu.json");
		//design.child<Layout>("heroArmyCastle").clear();
		if (rec.data(castle).heroInCastle)
		{
			loadArmyHeroCastle(playerLayer.data(playerLayer.get(0)).army, castle);
			design.child<Layout>("heroIcon").child<Texture>("icon").setImageName("player_icon.png");
		}
		else
		{
			design.child<Layout>("heroArmyCastle").hide();
		}
		connect(cm.child<Button>("closeCastleMenu"), CloseCastleMenu, castle);
		loadArmyCastle(rec.data(castle).protectCastleArmy, castle);
		auto cmSelector = cm.child<Selector>("castleMenuSelector");
		connect(cm.child<Button>("backToMain"), changeCastleMenuSelector, cmSelector, main, castle);
		connect(cm.child<Button>("getArmyButton"), changeCastleMenuSelector, cmSelector, getArmy, castle);
		connect(cm.child<Button>("castleBuildings"), changeCastleMenuSelector, cmSelector, build, castle);
		heroMenu.hide();
	}

	void placeArmy(Army army, bool isLeft)
	{
		int y = 2;
		for (auto unit : army.units)
		{
			if (unit.n != 0)
			{
				int x = fight_field_map.w - 1;
				if (isLeft)
				{
					x = 0;
				}
				auto& uunitObj = units.load("unit.json", x * cellsize, y * cellsize);
				if (isLeft)
				{
					uunitObj.child<GameObj>("fightUnitImg").setScaleX(-1);
				}
				uunitObj.child<GameObj>("fightUnitImg").skin<Texture>().setImageName(allTypes[y - 2].img_name);
				auto& udata = units.data(uunitObj);
				auto a = army.owner;
				udata.owner = a;
				udata.unit.n = unit.n;
				uunitObj.child<Label>("fightUnitNum") << udata.unit.n;
				udata.cords.x = x;
				udata.cords.y = y;
				udata.unit.type = unit.type;
				fight_field_map[x][y] = uunitObj.id();
			}
			y++;
		}
	}

	deque <pair <int, int>> fight_queue; //fewkolfewoiewfhiwefiuhfewhufiuewfuhewihuerihufrcdiuhreihuihureihurfiuhrfiuhfrihurfikhufvikhu

	deque <pair <int, int>> make_fight_queue ()
	{
		deque <pair <int, int>> queue;
		for (auto nit : units.all())
		{
			auto& unit = units.data(nit);
			pair <int, int> p;
			p.first = unit.unit.type.initiative;
			p.second = nit.id();
			queue.push_back(p);
			cout << "owner: " << unit.owner;
		}
		sort(queue.begin(), queue.end());
		reverse(queue.begin(), queue.end());
		return queue;
	}

	set <IntVec2> zone;

	void renderUnitStats(int id)
	{
		units.get(id).child<Label>("fightUnitNum") << units.data(id).unit.n;
	}

	void draw_get_step_fight_zone(int id)
	{
		IntVec2 pos = units.data(id).cords;
		int step = units.data(id).unit.type.stepPoint;
		zone = get_fight_zone(pos, step);
		step_zone.clear();
		auto a = step_zone.load("zone_item.json", pos.x * cellsize, pos.y * cellsize);
		a.skin<Texture>().setColor(0, 0, 255, 100);
		for (auto zitem : zone)
		{
			auto item = step_zone.load("zone_item.json", zitem.x * cellsize, zitem.y * cellsize);
			if (get_owner(zitem.x, zitem.y) == noOwner)
			{
				item.skin<Texture>().setColor(0, 255, 0, 100);
			}
		}
	}

	deque <IntVec2> enemy_fight_direction;

	void make_computer_step()
	{
		int id = target_queue[0].second; // кого бьём
		int id2 = id;
		IntVec2 pos1 = units.data(id).cords;
		id = fight_queue[0].second; // кто бьёт
		IntVec2 pos2 = units.data(id).cords;
		enemy_fight_direction = fight_route(pos2, pos1);
		int steps = units.data(fight_queue[0].second).unit.type.stepPoint;
		if (units.data(id).unit.type.strelyarcher)
		{
			if (!empty(units.data(id).unit.type.availibleMagic))
			{
				kastanut_malexa(1);
				magicAttack();
			}
			else
			{
				attack(units.data(id2).unit.type.strelyarcher);
			}
		}
		else
		{
			if (!empty(units.data(id).unit.type.availibleMagic))
			{
				kastanut_malexa(1);
				magicAttack();
			}
			else
			{
				for (auto dir : enemy_fight_direction)
				{
					IntVec2& NowPPos = units.data(units.get(fight_queue[0].second)).cords;
					fight_field_map[units.data(units.get(fight_queue[0].second)).cords.x][units.data(units.get(fight_queue[0].second)).cords.y] = -1;
					if (dir.x > NowPPos.x)
					{
						units.data(units.get(fight_queue[0].second)).cords.x++;
						units.get(fight_queue[0].second).anim.play("right", 0);
					}
					else if (dir.x < NowPPos.x)
					{
						units.data(units.get(fight_queue[0].second)).cords.x--;
						units.get(fight_queue[0].second).anim.play("left", 0);
					}
					else if (dir.y > NowPPos.y)
					{
						units.data(units.get(fight_queue[0].second)).cords.y++;
						units.get(fight_queue[0].second).anim.play("top", 0);
					}
					else if (dir.y < NowPPos.y)
					{
						units.data(units.get(fight_queue[0].second)).cords.y--;
						units.get(fight_queue[0].second).anim.play("down", 0);
					}
					fight_field_map[units.data(units.get(fight_queue[0].second)).cords.x][units.data(units.get(fight_queue[0].second)).cords.y] = fight_queue[0].second;

					steps--;
					if (steps < 1)
					{
						break;
					}
				}
			}
		}
		enemy_fight_direction.clear();
		animStart = true;
		moveUnitId = fight_queue[0].second;
	}

	ownerType getUnitOwner(int id)
	{
		return units.data(id).owner;
	}

	deque <pair <float, int>> target_queue;

	deque <pair <float, int>> makeTargetQueue (ownerType attackOwner)
	{
		deque <pair <float, int>> queue;
		for (auto nit : units.all())
		{
			auto& unit = units.data(nit);
			pair <float, int> p;
			p.first = unit.unit.type.k;
			p.second = nit.id();
			if (attackOwner == units.data(nit).owner)
			{
				continue;
			}
			queue.push_back(p);
		}
		sort(queue.begin(), queue.end());
		reverse(queue.begin(), queue.end());
		return queue;
	}

	void next_fight_step()
	{
		if (animStart)
		{
			return;
		}
		auto first = fight_queue[0];
		fight_queue.pop_front();
		fight_queue.push_back(first);
		auto first2 = fight_queue[0];
		if (!empty(units.data(first2.second).unit.type.availibleMagic))
		{
			useMagicUnit.show();
		}
		else
		{
			useMagicUnit.hide();
		}
		if (getUnitOwner(first2.second) != humanplayer)
		{
			IntVec2 pos = units.data(first2.second).cords;
			int step = units.data(first2.second).unit.type.stepPoint;
			zone = get_fight_zone(pos, step);
			step_zone.clear();
			make_computer_step();
		}
		else
		{
			draw_get_step_fight_zone(first2.second);
		}
	}

	bool kastuyu = false;

	int kastId = -1;

	void kastanut_malexa(int kast_id)
	{
		kastuyu = true;
		kastId = kast_id; 
	    closeMagicWindow();
	}

	void loadMagic(vector<int> availibleMagic)
	{
		auto win = design.child<Layout>("magicWin");
		for (auto m : availibleMagic)
		{
			string stroka = "magicIcon" + toString(m);
			cout << stroka << endl;
			auto mag = win.child<Layout>(stroka);
			auto but = mag.child<Button>("button");
			but.child<Texture>("icon").setImageName("magic_icons/fireball_icon.png");
			design.update();
			connect(but, kastanut_malexa, m);
		}
		win.show();
		design.child<Layout>("forFightWindow").show();
	}

	void closeMagicWindow()
	{
		auto win = design.child<Layout>("forFightWindow");
		win.clear();
		win.hide();
	}

	void showMagicWindow()
	{
		if (animStart)
		{
			return;
		}
		auto win = design.child<Layout>("forFightWindow");
		win.load("magic_window.json");
		connect(win.child<Button>("closeMagicWindow"), closeMagicWindow);
		loadMagic(units.data(fight_queue[0].second).unit.type.availibleMagic);
	}

	bool check_win(ownerType owner)
	{
		bool win = true;
		for (auto nit : units.all())
		{
			auto owner2 = units.data(nit).owner;
			cout << owner2 << endl;
			if (owner2 != owner)
			{
				win = false;
			}
		}
		return win;
	}

	bool animStart = false;
	int moveUnitId = -1;
	const int cellsize = 75;
	
	void delete_from_queue(int id)
	{
		int i = 0;
		for (auto item : fight_queue)
		{
			if (item.second == id)
			{
				fight_queue.erase(fight_queue.begin() + i);
				break;
			}
			i++;
		}
	}

	int neutral_fight_id;

	bool is_mi_ryadom(IntVec2 pos1, IntVec2 pos2)
	{
		bool mi_ryadom = false;
		/*pos1.x + 1 == pos2.x
			|| pos1.x - 1 == pos2.x
			|| pos1.y + 1 == pos2.y
			|| pos1.y - 1 == pos2.y
			|| (pos1.x + 1 == pos2.x && pos1.y + 1 == pos2.y)
			|| (pos1.x + 1 == pos2.x && pos1.y - 1 == pos2.y)
			|| (pos1.x - 1 == pos2.x && pos1.y - 1 == pos2.y)
			|| (pos1.x - 1 == pos2.x && pos1.y + 1 == pos2.y)*/
		if (abs(pos1.x - pos2.x) <= 1 && abs(pos1.y - pos2.y) <= 1)
		{
			mi_ryadom = true;
		}
		return mi_ryadom;
	}

	Army getOstatochki(ownerType owner)
	{
		Army army = createEmptyArmy(owner);
		for (auto nit : units.all())
		{
			if (units.data(nit).owner == owner)
				army.units[units.data(nit).unit.type.lvl - 1] = units.data(nit).unit;
		}
		return army;
	}

	Army getPoteri(Army startArmy)
	{
		ownerType owner = startArmy.owner;
		Army army = createEmptyArmy(owner);
		Army finishArmy = getOstatochki(owner);
		int index = 0;
		for (auto nit : finishArmy.units)
		{
			army.units[index].n = startArmy.units[index].n - nit.n;
			index++;
		}
		/*for (auto nit : units.all())
		{
			int index = units.data(nit).unit.type.lvl - 1;
			if (units.data(nit).owner == owner)
			{
				int newCount = startArmy.units[index].n - finishArmy.units[index].n;
				army.units[index] = units.data(nit).unit;
				army.units[index].n = newCount;
			}
		}*/
		return army;
	}

	Army startArmy1, startArmy2;

	int countfightReward(Army startArmy)
	{
		int points = 0;
		for (auto nit : getPoteri(startArmy).units)
		{
			points += nit.n * nit.type.skill;
		}
		return points;
	}

	void selectFight(bool lose)
	{
		selector.select(1);
		isFighting = false;
		fightExit(lose);
		design.child<Layout>("forFightWindow").clear();
		design.child<Layout>("forFightWindow").hide();
	}

	int magicDamageCount(int kast_damage, int n)
	{
		return kast_damage * n;
	}

	void magicAttack()
	{
		isAttacking = false;
		kastuyu = false;
		useMagicUnit.hide();
		auto& unit1 = units.data(attackUnitId).unit;
		auto& unit2 = units.data(protectUnitId).unit;
		int n1 = unit1.n;
		int n2 = unit2.n;
		auto& oneHp2 = allTypes[unit2.type.lvl - 1].hp;
		auto& allHp2 = unit2.type.hp;
		auto damage = magicDamageCount(AllMagic[kastId - 1].damage, n1);
		n2 -= die_count(damage, unit2.n, unit2.type.hp, unit2.type.lvl);
		allHp2 -= damage;
		unit2.n = n2;
		renderUnitStats(protectUnitId); 
		if (n2 <= 0)
		{
			fight_field_map[units.data(protectUnitId).cords] = -1;
			delete_from_queue(protectUnitId);
			units.get(protectUnitId).kill();
			moveUnitId = -1;

			if (check_win(humanplayer) || check_win(neutral))
			{
				auto forwindow = design.child<Layout>("forFightWindow");
				forwindow.show();
				auto window = forwindow.load("finishFight.json");
				window.child<Label>("fightReward") << countfightReward(startArmy2);
				fight_exit.hide();
				if (check_win(humanplayer))
				{
					realrecmap[neutrals.data(neutral_fight_id).cords.x][neutrals.data(neutral_fight_id).cords.y] = noneRec;
					neutrals.get(neutral_fight_id).kill();
					playerLayer.data(playerLayer.get(0)).army = getOstatochki(humanplayer);
					loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
					loadfinishstats(getPoteri(startArmy1), getPoteri(startArmy2));
					window.child<Label>("fightReward") << countfightReward(startArmy2) << tr("skillpoints");
					skill += countfightReward(startArmy2);
					connect(window.child<Button>("closeFinishStats"), selectFight, false);
				}
				else
				{
					clearArmy(playerLayer.data(playerLayer.get(0)).army, humanplayer);
					loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
					loadfinishstats(getPoteri(startArmy1), getPoteri(startArmy2));
					window.child<Label>("fightReward") << countfightReward(startArmy2) << tr("skillpoints");
					connect(window.child<Button>("closeFinishStats"), selectFight, true);
				}
				design.child<Layout>("statusBar").show();
				//selector.select(1);
				units.clear();
				zone.clear();
			}
			else
			{
				target_queue = makeTargetQueue(neutral);
			}
		}
	}

	void attack(bool archer)
	{
		isAttacking = false;
		auto& unit1 = units.data(attackUnitId).unit;
		auto& unit2 = units.data(protectUnitId).unit;
		if (!is_mi_ryadom(units.data(attackUnitId).cords, units.data(protectUnitId).cords) && !archer)
		{
			isAttacking = true;
			return;
		}
		auto damage1 = damage_count(unit1.type.attack, unit2.type.protect, unit1.type.damage, unit1.n);
		auto& oneHp1 = allTypes[unit1.type.lvl - 1].hp;
		auto& oneHp2 = allTypes[unit2.type.lvl - 1].hp;
		auto& allHp1 = unit1.type.hp;
		auto& allHp2 = unit2.type.hp;
		int n1 = unit1.n;
		int n2 = unit2.n;
		n2 -= die_count(damage1, unit2.n, unit2.type.hp, unit2.type.lvl);
		float damage2 = damage_count(unit2.type.attack, unit1.type.protect, unit2.type.damage, n2);
		allHp2 -= damage1;
		unit2.type.hp = allHp2;
		if (unit2.type.strelyarcher)
		{
			n1 -= die_count(damage2, unit1.n, unit1.type.hp, unit1.type.lvl);
			unit1.n = n1;
			allHp1 -= damage2;
			unit1.type.hp = allHp1;
		}
		unit2.n = n2;
		renderUnitStats(attackUnitId);
		renderUnitStats(protectUnitId);
		if (n1 == 0)
		{
			fight_field_map[units.data(attackUnitId).cords] = -1;
			delete_from_queue(attackUnitId);
			units.get(attackUnitId).kill();
			moveUnitId = -1;
			if (check_win(humanplayer) || check_win(neutral))
			{
				auto forwindow = design.child<Layout>("forFightWindow");
				forwindow.show();
				auto window = forwindow.load("finishFight.json");
				loadfinishstats(getPoteri(startArmy1), getPoteri(startArmy2));
				fight_exit.hide();
				if (check_win(humanplayer))
				{
					realrecmap[neutrals.data(neutral_fight_id).cords.x][neutrals.data(neutral_fight_id).cords.y] = noneRec;
					neutrals.get(neutral_fight_id).kill();
					playerLayer.data(playerLayer.get(0)).army = getOstatochki(humanplayer);
					loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
					window.child<Label>("fightReward") << countfightReward(startArmy2) << tr("skillpoints");
					skill += countfightReward(startArmy2);
					connect(window.child<Button>("closeFinishStats"), selectFight, false);
				}
				else
				{
					clearArmy(playerLayer.data(playerLayer.get(0)).army, humanplayer);
					loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
					loadfinishstats(getPoteri(startArmy1), getPoteri(startArmy2));
					window.child<Label>("fightReward") << countfightReward(startArmy2) << tr("skillpoints");
					connect(window.child<Button>("closeFinishStats"), selectFight, true);
				}
				units.clear();
				design.child<Layout>("statusBar").show();
			}
			else
			{
				target_queue = makeTargetQueue(neutral);
			}
		}
		if (n2 == 0)
		{
			fight_field_map[units.data(protectUnitId).cords] = -1;
			delete_from_queue(protectUnitId);
			units.get(protectUnitId).kill();
			moveUnitId = -1;

			if (check_win(humanplayer) || check_win(neutral))
			{
				auto forwindow = design.child<Layout>("forFightWindow");
				forwindow.show();
				auto window = forwindow.load("finishFight.json");
				window.child<Label>("fightReward") << countfightReward(startArmy2);
				fight_exit.hide();
				if (check_win(humanplayer))
				{
					realrecmap[neutrals.data(neutral_fight_id).cords.x][neutrals.data(neutral_fight_id).cords.y] = noneRec;
					neutrals.get(neutral_fight_id).kill();
					playerLayer.data(playerLayer.get(0)).army = getOstatochki(humanplayer);
					loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
					loadfinishstats(getPoteri(startArmy1), getPoteri(startArmy2));
					window.child<Label>("fightReward") << countfightReward(startArmy2) << tr("skillpoints");
					skill += countfightReward(startArmy2);
					connect(window.child<Button>("closeFinishStats"), selectFight, false);
				}
				else
				{
					clearArmy(playerLayer.data(playerLayer.get(0)).army, humanplayer);
					loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
					loadfinishstats(getPoteri(startArmy1), getPoteri(startArmy2));
					window.child<Label>("fightReward") << countfightReward(startArmy2) << tr("skillpoints");
					connect(window.child<Button>("closeFinishStats"), selectFight, true);
				}
				design.child<Layout>("statusBar").show();
				//selector.select(1);
				units.clear();
				zone.clear();
			}
			else
			{
				target_queue = makeTargetQueue(neutral);
			}
		}
	}

	void startFight(Army army1, Army army2, int neutralFightId = -1)
	{

		selector.select(3);
		design.child<Layout>("statusBar").hide();
		map<Color, int> colorToTypeGround = {
			{ Color(165, 0, 255), grassDark },
		};
		fight_field_map = loadMap("Grassmap_fight.png", colorToTypeGround);
		for (int x = 0; x < fight_field_map.w; ++x)
		{
			for (int y = 0; y < fight_field_map.h; ++y)
			{
				if (fight_field_map[x][y] == grassDark)
				{
					fight_ground.load("grass.json", x * cellsize, y * cellsize);
					fight_field_map[x][y] = -1;
				}
			}
		}
		fight_field.setView(fight_field_map.w * 0.5 * cellsize - cellsize * 0.5, fight_field_map.h * 0.5 * cellsize - cellsize * 0.5);
		placeArmy(army1, true);
		placeArmy(army2, false);
		startArmy1 = army1;
		startArmy2 = army2;
		fight_queue = make_fight_queue();
		target_queue = makeTargetQueue(neutral);
		if (units.data(fight_queue[0].second).owner != humanplayer)
		{
			make_computer_step();
		}
		else
		{
			draw_get_step_fight_zone(fight_queue[0].second);
		}
		connect(new_fight_step, next_fight_step);
		if (neutralFightId != -1)
		{
			neutral_fight_id = neutralFightId;
		}
		fight_exit.show();
		isFighting = true;
	}

	int isNeutralsRyadom(IntVec2 cords)
	{
		for (auto neutral : neutrals.all())
		{
			auto ncords = neutrals.data(neutral).cords;
			if (is_mi_ryadom(cords, ncords))
			{
				return neutral.id();
			}
		}
		return -1;
	}

    void move()
     {
		GOld << " : " << gold;
		SKill << " : " << skill;
		Ore << " : " << ore;
		Gems << " : " << gems;
		Sera << " : " << sera;
		WOOD << " : " << wood;
		heroStepsLabel << " : " << stepPoints;
		field.setView(playerLayer.get(0).pos());
		//updateHeroCastles();
		

		if (isFighting)
		{
			if (moveUnitId != -1 && animStart)
			{
				if (units.get(moveUnitId).anim.isEmpty())
				{
					if (fightTarget == FightTarget::unit && isAttacking)
					{
						attack(false);
					}
					animStart = false;
					moveUnitId = -1;
					next_fight_step();
				}
			}
		}
		else {
			if (playerLayer.get(0).anim.isEmpty())
			{
				IntVec2 NowPPos = cell(playerLayer.get(0).pos(), isZoomDown);
				if (isNeutralsRyadom(NowPPos) != -1)
				{
					startFight(playerLayer.data(playerLayer.get(0)).army, neutrals.data(isNeutralsRyadom(NowPPos)).army, isNeutralsRyadom(NowPPos));
				}
			}
			if (heroStay(playerLayer.get(0)))
			{
				for (auto n : nishtyaki.find(playerLayer.get(0).pos()))
				{
					if (nishtyaki.data(n).type == golda)
					{
						randomize();
						int a = randomInt(1, 5);
						if (a >= 1 && a < 4)
						{
							gold += 200;
						}
						if (a >= 4)
						{
							gold += 400;
						}

					}
					else
					{
						showChestMenu();
					}
					n.kill();
				}
			}

			if (stepPoints > 0)
			{
				emptyStep.hide();
			}



			if (!empty(direction) && playerLayer.get(0).anim.isEmpty() && stepPoints > 0)
			{
				IntVec2 NowPPos = cell(playerLayer.get(0).pos(), isZoomDown);
				auto dir = direction.front();
				stepPoints--;
				string suffix = isZoomDown ? "_min" : "";
				if (dir.x > NowPPos.x)
				{
					playerLayer.get(0).anim.play("right" + suffix, 0);
				}
				else if (dir.x < NowPPos.x)
				{
					playerLayer.get(0).anim.play("left" + suffix, 0);
				}
				else if (dir.y > NowPPos.y)
				{
					playerLayer.get(0).anim.play("up" + suffix, 0);
				}
				else if (dir.y < NowPPos.y)
				{
					playerLayer.get(0).anim.play("down" + suffix, 0);
				}
				NowPPos = dir;
				direction.pop_front();
			}

			for (auto egg : SuperMegaPuperStepEgg.find(playerLayer.get(0).pos()))
			{
				egg.kill();
			}

			if (target != Target::none && heroStay(playerLayer.get(0)))
			{
				auto& ppos = playerLayer.get(0).pos();
				auto cppos = cell(ppos);
				if (isZoomDown)
				{
					cppos = cell_zoom(ppos);
				}
				for (auto Neutral : neutrals.all())
				{
					auto& rpos = neutrals.data(Neutral).cords;
					if ((rpos.x + 1 == cppos.x && rpos.y == cppos.y) || (rpos.x - 1 == cppos.x  && rpos.y == cppos.y) || (rpos.y + 1 == cppos.y && rpos.x == cppos.x) || (rpos.y - 1 == cppos.y && rpos.x == cppos.x) || (rpos.x + 1 == cppos.x && rpos.y + 1 == cppos.y) || (rpos.x + 1 == cppos.x && rpos.y - 1 == cppos.y) || (rpos.y - 1 == cppos.y && rpos.x - 1 == cppos.x) || (rpos.x - 1 == cppos.x && rpos.y + 1 == cppos.y))
					{

						if (target == Target::fight)
						{
							startFight(playerLayer.data(playerLayer.get(0)).army, neutrals.data(Neutral).army, Neutral.id());
							target = Target::none;
						}
					}
				}
				for (auto Rec : rec.all())
				{
					auto& rpos = rec.data(Rec).cords;
					if ((rpos.x + 1 == cppos.x && rpos.y == cppos.y) || (rpos.x - 1 == cppos.x  && rpos.y == cppos.y) || (rpos.y + 1 == cppos.y && rpos.x == cppos.x) || (rpos.y - 1 == cppos.y && rpos.x == cppos.x) || (rpos.x + 1 == cppos.x && rpos.y + 1 == cppos.y) || (rpos.x + 1 == cppos.x && rpos.y - 1 == cppos.y) || (rpos.y - 1 == cppos.y && rpos.x - 1 == cppos.x) || (rpos.x - 1 == cppos.x && rpos.y + 1 == cppos.y))
					{
						if (target == Target::castle)
						{
							if (rec.data(Rec).type == castle || rec.data(Rec).type == castle_entry)
							{
								target = Target::none;
								rec.data(Rec).heroInCastle = true;
								Rec.child<Texture>("flag").setColor(255, 0, 0, 255);
								rec.data(Rec).owner = playerLayer.data(playerLayer.get(0)).owner;
								openCastleMenu(Rec);
								updateHeroCastles();
								//cm.child
								break;
							}
							else
							{
								continue;
							}
						}
						else if (target == Target::mine)
						{
							if (rec.data(Rec).type == castle || rec.data(Rec).type == rock)
								continue;
							//GOld << "allright!!";
							rec.data(Rec).owner = playerLayer.data(playerLayer.get(0)).owner;
							forWindow.load(2, "MineInfo.json");
							auto owner = forWindow.child<Label>("rOwner");
							if (rec.data(Rec).owner == humanplayer)
							{
								owner << tr("you");
								Rec.child<Texture>("flag").setColor(255, 0, 0, 255);
							}
							else
							{
								owner << tr("neutral");
							}

							auto type = forWindow.child<Label>("rType");

							auto n = forWindow.child<Label>("rN");
							if (rec.data(Rec).type == goldMine)
							{
								type << tr("gold");
								n << "1000";
								Rec.child<Texture>("flag").setColor(255, 0, 0, 255);
							}
							else if (rec.data(Rec).type == mine)
							{
								type << tr("ore");
								n << "2";
								Rec.child<Texture>("flag").setColor(255, 0, 0, 255);
							}
							else if (rec.data(Rec).type == sera_mine)
							{
								type << tr("sera");
								n << "1";
								Rec.child<Texture>("flag").setColor(255, 0, 0, 255);
							}
							else if (rec.data(Rec).type == gem_mine)
							{
								type << tr("gems");
								n << "1";
								Rec.child<Texture>("flag").setColor(255, 0, 0, 255);
							}
							else if (rec.data(Rec).type == sawmill)
							{
								type << tr("wood");
								n << "2";
								Rec.child<Texture>("flag").setColor(255, 0, 0, 255);
							}
							else if (rec.data(Rec).type == rock)
							{
								closeMineInfo();
							}
							auto closeBut = forWindow.child<Button>("closeMineInfoBut");
							connect(closeBut, closeMineInfo);
							target = Target::none;
						}
					}
				}
			}
		}
    }

	GameMap groundmap, GroundObjMap, MobsMap, Nishtyakmap, rockmap, recmap, realrecmap, fight_field_map;
	FromDesign(Button, start_button);
	FromDesign(Button, quit_button);
	FromDesign(Button, load_button);
	FromDesign(Button, Relog);
	FromDesign(Button, useMagicUnit);
	FromDesign(Button, newDirection);
	FromDesign(Label, test_);
	FromDesign(Selector, selector);
	FromDesign(Layout, castle_menu);
	FromDesign(Layout, field_container);
	FromDesign(Button, new_fight_step);
	FromDesign(Button, fight_exit);
	FromDesign(Label, WOOD);
	//FromDesign(HorizontalLayout, armyMenu);

	FromDesign(Label, heroStepsLabel);
	
	FromDesign(Layout, forWindow);

	FromDesign(Selector, heroMenuSelector);
	FromDesign(Button, select0);
	FromDesign(Button, select1);
	FromDesign(Button, select2);

	FromDesign(Button, heroMenu);
	FromDesign(GameView, field);
	FromDesign(GameView, fight_field);

	FromDesign(Label, GOld);
	FromDesign(Label, SKill);
	FromDesign(Label, Ore);
	FromDesign(Label, Gems);
	FromDesign(Label, Sera);

	FromDesign(Label, emptyStep);

	LayerFromDesign(void, mobs);
	LayerFromDesign(groundData, ground);
	LayerFromDesign(void, fight_ground);
	LayerFromDesign(void, step_zone);
	LayerFromDesign(void, forest);
	LayerFromDesign(eggData, SuperMegaPuperStepEgg);

	LayerFromDesign(playerData, playerLayer);
	LayerFromDesign(unitsData, units);
	LayerFromDesign(recData, rec);
	LayerFromDesign(nishData, nishtyaki);
	LayerFromDesign(neutralData, neutrals);
	LayerFromDesign(rockData, rocks);
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("HeroesConfig.json");
    app.setDesign("Design.json");
	if (!app.init(&argc, argv))
	{
		return 1;
	}        
    app.run();
    return 0;
}
