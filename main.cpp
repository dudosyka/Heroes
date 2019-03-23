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
		noneRec, mine, goldMine, castle, castle_entry, gem_mine, sera_mine, fight
	};

	struct nishData
	{
		nishType type;
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
		vector <Price> price;
		Enum type;
		string icon_name;
		string img_name;
	};

	vector<unitType> allTypes = {
		{
			3,
			4,
			8,
			3,
			4,
			8,
			1,
			{{Price::gold, 85}},
			unitType::myaso,
			"myaso_icon.png",
			"myaso.png"
		},
		{
			7,
			4,
			3,
			3,
			4,
			9,
			2,
			{{Price::gold, 50}},
			unitType::archer,
			"archer_icon.png",
			"Archer.png"
		},
		{
			30,
			7,
			5,
			8,
			7,
			15,
			3,
			{{Price::gold, 260}},
			unitType::grifon,
			"griffin_icon.png",
			"griffin.png"
		},
		{
			54,
			12,
			12,
			10,
			5,
			10,
			4,
			{{Price::gold, 600}},
			unitType::chuvak,
			"chuvak_icon.png",
			"chuvak.png"
		},
		{
			90,
			23,
			21,
			25,
			7,
			11,
			5,
			{{Price::gold, 1300}},
			unitType::horserider,
			"horserider_icon.png",
			"horserider.png"
		},
		{
			180,
			27,
			27,
			45,
			6,
			12,
			6,
			{{Price::gold, 2800}, {Price::crystal, 1}},
			unitType::angel,
			"Angel_icon.png",
			"Angel.png"
		},
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

	struct unitsData
	{
		Unit unit;
		ownerType owner;
		IntVec2 cords;
	};

	enum grass
	{
		grassDark,
		Player,
		Chest,
		Gold
	};

	Army createArmy(vector<pair<unitType::Enum, int>> newArmy)
	{
		Army army;
		for (auto armyUnit : newArmy)
		{
			Unit unit;
			unit.type = allTypes[armyUnit.first];
			unit.n = armyUnit.second;
			army.units.push_back(unit);
		}
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

	//void chestMenu

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
			neutrals.data(Neutral).army = createArmy({
				{ unitType::myaso, 0 },
				{ unitType::archer, 0 },
				{ unitType::grifon, 0 },
				{ unitType::chuvak, 0 },
				{ unitType::horserider, 0 },
				{ unitType::angel, 0 }
				});
			Neutral.skin<Texture>().setImageName(allTypes[n].img_name);
			neutrals.data(Neutral).army.units[n].n = unitNum;
			neutrals.data(Neutral).owner = neutral;
			neutrals.data(Neutral).cords = cords;
		}

	}

	void load()
	{
		connect(Relog, nextStep);
		readNeutralsData();
		connect(heroMenu, openHeroMenu);
		connect(select0, changeHeroMenu, stats);
		connect(select1, changeHeroMenu, arts);
		connect(select2, changeHeroMenu, army);

		connect(newDirection, clearSteps);
		connect(timer, hideLabel);

		timer.repeat(2);

		gold = 100000;
		skill = 9000;
		stepPoints = 4;
		days = 0;
		ore = 100000;
		sera = 50000;
		gems = 100000;
		wood = 10000;

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
					ground.load("grass.json", x * 150, y * 150);
				}
			}
		}

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
					playerLayer.load("Heroes.json", x * 150, y * 150);
					auto& player = playerLayer.data(playerLayer.get(0));
					player.army.owner = humanplayer;
					player.owner = humanplayer;
					player.army = createArmy({
						{ unitType::myaso, 20 },
						{ unitType::archer, 5 },
						{ unitType::grifon, 50 },
						{ unitType::chuvak, 100 },
						{ unitType::horserider, 1 },
						{ unitType::angel, 1000 } }
					);
					/*auto& parmy = player.army;
					Unit Unit;
					Unit.type = allTypes[0];
					Unit.n = 20;
					parmy.units.push_back()*/
				}
			}
		}
		field.setView(groundmap.w / 2 * 150 - 75, groundmap.h / 2 * 150 - 75);

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
						{ unitType::angel, 1000 } });
					rec.data(Rec).protectCastleArmy = createArmy({
						{ unitType::myaso, 10 },
						{ unitType::archer, 10 },
						{ unitType::grifon, 10 },
						{ unitType::chuvak, 10 },
						{ unitType::horserider, 10 },
						{ unitType::angel, 10 } });
				}
			}
		}
		for (auto Neutral : neutrals.all())
		{
			auto& data = neutrals.data(Neutral);
			realrecmap[data.cords.x][data.cords.y] = fight;
		}
		field.setView(groundmap.w / 2 * 150 - 75, groundmap.h / 2 * 150 - 75);
		loadMainArmy(playerLayer.data(playerLayer.get(0)).army);
		playerLayer.data(playerLayer.get(0)).exp = 0;
		playerLayer.data(playerLayer.get(0)).owner = humanplayer;
		updateHeroCastles();
		loadTextBank("textBank.json");
	}

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

	float damage_count(int attack, int protect, int damage, int n, int n2, int lvl)
	{
		if (attack > protect)
		{
			return (attack - protect) * damage * n * 0.02 * lvl;
		}
		else
		{
			return n * damage - (protect - attack) * n2 * 0.02 * lvl;
		}
	}

	int round_up(float a)
	{
		return a + 0.999999999;
	}

	int die_count(float damage, float n, int hp)
	{
		float new_n = n - damage / hp;

		if (new_n < 0)
		{
			return round_up(n);
		}
		else
		{
			return  round_up(n) - round_up(new_n);
		}
	}

	deque <IntVec2> fight_direction;
	//iowei09giofriojwijewf9uwfer4i93f4jt340t35jt90958t5f0834jt8j03j8t4f8t30948fj8430jtf8t094jf09t4380934jf8t0j8j08j80c8cjfwe

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
		//fkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigjfkjosdgjiodfgjdgjdjgodjgsjsgoigjdijojgdjgodjigj
		if (isFighting)
		{
			if (input.justPressed(MouseRight))
			{
				auto c = cell_fight(fight_field.mousePos());
				int die = 0;
				if (fight_field_map[c.x][c.y] > -1)
				{
					auto unit1 = units.data(fight_field_map[c.x][c.y]).unit;
					auto unit2 = units.data(fight_queue[0].second).unit;
					auto info = design.child<Layout>("fight_unit_stats");
					info.show();
					info.child<Label>("attack") << unit1.type.attack;
					info.child<Label>("protect") << unit1.type.protect;
					auto damage = damage_count(unit2.type.attack, unit1.type.protect, unit2.type.damage, unit2.n, unit1.n, unit1.type.lvl);
					auto die = die_count(damage, unit1.n, unit1.type.hp);
					info.child<Label>("enemy_die") << die;
					damage = damage_count(unit1.type.attack, unit2.type.protect, unit1.type.damage, unit1.n, unit2.n, unit2.type.lvl);
					die = die_count(damage, unit2.n, unit2.type.hp);
					info.child<Label>("your_die") << die;
					connect(info.child<Button>("close_fight_stats"), close_fight_stats);
				}
			}
			if (input.justPressed(MouseLeft))
			{
				auto c = cell_fight(fight_field.mousePos());
				if (zone.count(c) == 0)
					return;
				if (c.x >= fight_field_map.w || c.y >= fight_field_map.h || c.y < 0 || c.x < 0)
				{
					return;
				}
				int id = fight_queue[0].second;
				for (auto nit : units.all())
				{
					if (nit.id() == id)
					{
						fight_direction = fight_route(units.data(nit).cords, c);
						//fdfdf
					}
				}
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
				auto c = cell(field.mousePos());
				if (c.x < groundmap.w && c.y < groundmap.h && c.y >= 0 && c.x >= 0)
				{
					direction = route(cell(playerLayer.get(0).pos()), c);
					for (auto dir : direction)
					{
						SuperMegaPuperStepEgg.load("SuperMegaPuperStepEgg.json", pixels(dir));
					}
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
					target = Target::mine;
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
				while (dmap[finish] == 2000000001)
				{
					if (start.x > finish.x)
					{
						IntVec2 newFinish;
						newFinish.x = finish.x + 1;
						newFinish.y = finish.y;
						finish = newFinish;
					}
					if (start.x < finish.x)
					{
						finish = { finish.x-1, finish.y };
					}
					if (start.y > finish.y)
					{
						finish = { finish.x, finish.y+1 };
					}
					if (start.y < finish.y)
					{
						finish = { finish.x, finish.y-1 };
					}
				}
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

	ownerType get_owner(int x, int y)
	{
		ownerType owner = noOwner;
		if (fight_field_map[x][y] == -1)
		{
			return owner;
		}
		owner = units.data(fight_field_map[x][y]).owner;
		return owner;
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
				if (get_owner(cur.x + 1, cur.y) == noOwner)
				{
					queue.emplace_front(cur.x + 1, cur.y);
					dmap[cur.x + 1][cur.y] = w;
				}
				zone.insert({ cur.x + 1, cur.y });
			}
			if (cur.x > 0 && dmap[cur.x - 1][cur.y] > dmap[cur.x][cur.y] && get_owner(start.x, start.y) != get_owner(cur.x - 1, cur.y))
			{
				if (get_owner(cur.x - 1, cur.y) == noOwner)
				{
					queue.emplace_front(cur.x - 1, cur.y);
					dmap[cur.x - 1][cur.y] = w;
				}
				zone.insert({ cur.x - 1, cur.y });
			}
			if (cur.y > 0 && dmap[cur.x][cur.y - 1] > dmap[cur.x][cur.y] && get_owner(start.x, start.y) != get_owner(cur.x, cur.y - 1))
			{
				if (get_owner(cur.x, cur.y - 1) == noOwner)
				{
					queue.emplace_front(cur.x, cur.y - 1);
					dmap[cur.x][cur.y - 1] = w;
				}
				zone.insert({ cur.x, cur.y - 1 });
			}
			if (cur.y < dmap.h - 1 && dmap[cur.x][cur.y + 1] > dmap[cur.x][cur.y] && get_owner(start.x, start.y) != get_owner(cur.x, cur.y + 1))
			{
				if (get_owner(cur.x, cur.y + 1) == noOwner)
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
		return zone;
	}

	deque <IntVec2> fight_route(IntVec2 start, IntVec2 finish)
	{
		GameMap dmap;
		deque <IntVec2> queue;
		dmap = createMap(fight_field_map.w, fight_field_map.h);
		fightTarget = FightTarget::none;
		if (fight_field_map[finish] != -1)
		{
			fightTarget = FightTarget::unit;
		}
		else
		{
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
			if (cur.y > 0 && dmap[cur.x][cur.y - 1] > dmap[cur.x][cur.y] && fight_field_map[cur.x][cur.y - 1] == -1)
			{
				queue.emplace_front(cur.x, cur.y - 1);
				dmap[cur.x][cur.y - 1] = w;
			}
			if (cur.x > 0 && dmap[cur.x - 1][cur.y] > dmap[cur.x][cur.y] && fight_field_map[cur.x - 1][cur.y] == -1)
			{
				queue.emplace_front(cur.x - 1, cur.y);
				dmap[cur.x - 1][cur.y] = w;
			}
			if (cur.y < dmap.h - 1 && dmap[cur.x][cur.y + 1] > dmap[cur.x][cur.y] && fight_field_map[cur.x][cur.y + 1] == -1)
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
				while (dmap[finish] == 2000000001)
				{
					if (start.x > finish.x)
					{
						IntVec2 newFinish;
						newFinish.x = finish.x + 1;
						newFinish.y = finish.y;
						finish = newFinish;
					}
					if (start.x < finish.x)
					{
						finish = { finish.x - 1, finish.y };
					}
					if (start.y > finish.y)
					{
						finish = { finish.x, finish.y + 1 };
					}
					if (start.y < finish.y)
					{
						finish = { finish.x, finish.y - 1 };
					}
				}
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
				auto uunitObj = units.load("unit.json", x * cellsize, y * cellsize);
				if (isLeft)
				{
					uunitObj.child<GameObj>("fightUnitImg").setScaleX(-1);
				}
				uunitObj.child<GameObj>("fightUnitImg").skin<Texture>().setImageName(allTypes[y - 2].img_name);
				auto& udata = units.data(uunitObj);
				udata.owner = army.owner;
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
		}
		sort(queue.begin(), queue.end());
		reverse(queue.begin(), queue.end());
		return queue;
	}

	set <IntVec2> zone;

	void draw_get_step_fight_zone(int id)
	{
		IntVec2 pos = units.data(id).cords;
		int step = units.data(id).unit.type.stepPoint;
		zone = get_fight_zone(pos, step);
		step_zone.clear();
		step_zone.load("zone_item.json", pos.x * cellsize, pos.y * cellsize);
		for (auto zitem : zone)
		{
			auto item = step_zone.load("zone_item.json", zitem.x * cellsize, zitem.y * cellsize);
			if (get_owner(zitem.x, zitem.y) == noOwner)
			{
				item.skin<Texture>().setColor(0, 255, 0, 100);
			}
		}
	}

	void next_fight_step()
	{
		auto first = fight_queue[0];
		fight_queue.pop_front();
		fight_queue.push_back(first);
		auto first2 = fight_queue[0];
		draw_get_step_fight_zone(first2.second);
	}

	bool animStart = false;
	int moveUnitId = -1;
	const int cellsize = 75;
    void move()
     {
		GOld << " : " << gold;
		SKill << " : " << skill;
		Ore << " : " << ore;
		Gems << " : " << gems;
		Sera << " : " << sera;
		heroStepsLabel << " : " << stepPoints;
		field.setView(playerLayer.get(0).pos());
		//updateHeroCastles();

		if (moveUnitId != -1 && animStart)
		{
			if (units.get(moveUnitId).anim.isEmpty())
			{
				animStart = false;
				next_fight_step();
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
		
		if (!empty(fight_direction))
		{
			int steps;
			for (auto dir : fight_direction)
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
				
				animStart = true;
				moveUnitId = fight_queue[0].second;
			}
			fight_direction.clear();
		}

		if (!empty(direction) && playerLayer.get(0).anim.isEmpty() && stepPoints > 0)
		{
			IntVec2 NowPPos = cell(playerLayer.get(0).pos());
			auto dir = direction.front();
			stepPoints--;
			if (dir.x > NowPPos.x)
			{
				playerLayer.get(0).anim.play("right", 0);
			}
			else if (dir.x < NowPPos.x)
			{
				playerLayer.get(0).anim.play("left", 0);
			}
			else if (dir.y > NowPPos.y)
			{
				playerLayer.get(0).anim.play("up", 0);
			}
			else if (dir.y < NowPPos.y)
			{
				playerLayer.get(0).anim.play("down", 0);
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
			for (auto Neutral : neutrals.all())
			{
				auto& rpos = neutrals.data(Neutral).cords;
				if ((rpos.x + 1 == cppos.x && rpos.y == cppos.y) || (rpos.x - 1 == cppos.x  && rpos.y == cppos.y) || (rpos.y + 1 == cppos.y && rpos.x == cppos.x) || (rpos.y - 1 == cppos.y && rpos.x == cppos.x) || (rpos.x + 1 == cppos.x && rpos.y + 1 == cppos.y) || (rpos.x + 1 == cppos.x && rpos.y - 1 == cppos.y) || (rpos.y - 1 == cppos.y && rpos.x - 1 == cppos.x) || (rpos.x - 1 == cppos.x && rpos.y + 1 == cppos.y))
				{

					if (target == Target::fight)
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
						placeArmy(playerLayer.data(playerLayer.get(0)).army, true);
						placeArmy(neutrals.data(Neutral).army, false);
						fight_queue = make_fight_queue();
						draw_get_step_fight_zone(fight_queue[0].second);
						connect(new_fight_step, next_fight_step);
						isFighting = true;
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
						if (rec.data(Rec).type == castle)
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
						if (rec.data(Rec).type == castle)
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
						auto closeBut = forWindow.child<Button>("closeMineInfoBut");
						connect(closeBut, closeMineInfo);
						target = Target::none;
					}
				}
			}
		}
    }

	GameMap groundmap, GroundObjMap, MobsMap, Nishtyakmap, recmap, realrecmap, fight_field_map;
	FromDesign(Button, start_button);
	FromDesign(Button, quit_button);
	FromDesign(Button, load_button);
	FromDesign(Button, Relog);
	FromDesign(Button, newDirection);
	FromDesign(Label, test_);
	FromDesign(Selector, selector);
	FromDesign(Layout, castle_menu);
	FromDesign(Button, new_fight_step);
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
	LayerFromDesign(void, ground);
	LayerFromDesign(void, fight_ground);
	LayerFromDesign(unitsData, units);
	LayerFromDesign(void, step_zone);
	LayerFromDesign(void, forest);
	LayerFromDesign(playerData, playerLayer);
	LayerFromDesign(recData, rec);
	LayerFromDesign(nishData, nishtyaki);
	LayerFromDesign(void, SuperMegaPuperStepEgg);
	LayerFromDesign(neutralData, neutrals);
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("HeroesConfig.json");
    app.setDesign("Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}