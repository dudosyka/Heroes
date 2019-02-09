#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
	int gold, skill;

	deque <IntVec2> direction;

	bool heroMenuOpen = false;

	Timer timer;

	int stepPoints;

	enum ownerType
	{
		neutral, humanplayer, others
	};

	enum nishType
	{
		golda,
		chest
	};

	enum recType
	{
		noneRec, mine, goldMine, castle
	};

	struct recData
	{
		recType type;
		ownerType owner;
	};

	struct nishData
	{
		nishType type;
	};

	struct Price
	{
		enum Enum
		{
			gold, ore, wood, crystal
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
		vector <Price> price;
		Enum type;
	};

	vector<unitType> allTypes = {
		{
			16,
			4,
			8,
			3,
			4,
			8,
			{{Price::gold, 85}},
			unitType::myaso
		},
		{
			7,
			4,
			3,
			3,
			4,
			9,
			{{Price::gold, 50}},
			unitType::archer
		},
		{
			30,
			7,
			5,
			8,
			7,
			15,
			{{Price::gold, 260}},
			unitType::grifon
		},
		{
			54,
			12,
			12,
			10,
			5,
			10,
			{{Price::gold, 600}},
			unitType::chuvak
		},
		{
			90,
			23,
			21,
			25,
			7,
			11,
			{{Price::gold, 1300}},
			unitType::horserider
		},
		{
			16,
			4,
			8,
			3,
			4,
			8,
			{{Price::gold, 2800}, {Price::crystal, 1}},
			unitType::myaso
		},
	};

	struct unit
	{
		int n;
		unitType type;
	};

	struct Army
	{
		ownerType owner;
		vector <unit> units;
	};

	struct playerData
	{
		Army army;
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
			unit Unit;
			Unit.type = allTypes[armyUnit.first];
			Unit.n = armyUnit.second;
			army.units.push_back(Unit);
		}
		return army;
	}

	void load()
	{
		stepPoints = 4;
		connect(Relog, nextStep);

		connect(heroMenu, openHeroMenu);
		connect(select0, changeHeroMenu, stats);
		connect(select1, changeHeroMenu, arts);
		connect(select2, changeHeroMenu, army);

		connect(newDirection, clearSteps);
		connect(timer, hideLabel);
		timer.repeat(2);
		gold = 0;
		skill = 0;
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
					player.army = createArmy({
						{ unitType::myaso, 20 },
						{ unitType::archer, 5 },
						{ unitType::grifon, 50 },
						{ unitType::chuvak, 100 },
						{ unitType::horserider, 1 },
						{ unitType::angel, 1000 } }
					);
					/*auto& parmy = player.army;
					unit Unit;
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
			{ Color(0, 4, 255), goldMine }
		};
		recmap = loadMap("recmap.png", colorToTypeRec);
		for (int x = 0; x < recmap.w; ++x)
		{
			for (int y = 0; y < recmap.h; ++y)
			{
				if (recmap[x][y] == mine)
				{
					auto Rec = rec.load("Rec.json", x * 150, y * 150);
					Rec.skin<Texture>().setImageName("mine.png");
					rec.data(Rec).type = mine;
				}
				if (recmap[x][y] == goldMine)
				{
					auto Rec = rec.load("Rec.json", x * 150, y * 150);
					rec.data(Rec).type = goldMine;
				}
				if (recmap[x][y] == castle)
				{
					auto Rec = rec.load("Rec.json", x * 150, y * 150);
					Rec.skin<Texture>().setImageName("castle.png");
					rec.data(Rec).owner = humanplayer;
					Rec.setScaleX(1.5);
					Rec.setScaleY(1.5);
					rec.data(Rec).type = castle;
				}
			}
		}
		field.setView(groundmap.w / 2 * 150 - 75, groundmap.h / 2 * 150 - 75);
	}

	void hideLabel()
	{
		emptyStep.hide();
	}

	void nextStep()
	{
		stepPoints = 4;
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
			heroMenuSelector.select(2);
		}
	}

	void clearSteps ()
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

    void process(Input input)
	{
        using namespace gamebase::InputKey;
		if (input.justPressed(Escape))
		{
			close();
		}

		if (input.justPressed(MouseLeft) && playerLayer.get(0).anim.isEmpty() && !impl::isMouseOn(Relog.getImpl().get()) && !impl::isMouseOn(newDirection.getImpl().get()) && !impl::isMouseOn(heroMenu.getImpl().get()) && !heroMenuOpen)
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

	deque <IntVec2> route (IntVec2 start, IntVec2 finish)
	{
		GameMap dmap;
		deque <IntVec2> queue;
		dmap = createMap(groundmap.w, groundmap.h);
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
			if (dmap.w > cur.x + 1 && dmap[cur.x + 1][cur.y] > w && recmap[cur.x + 1][cur.y] == noneRec)
			{
				queue.emplace_front(cur.x + 1, cur.y);
				dmap[cur.x + 1][cur.y] = w;
			}
			if (cur.y > 0 && dmap[cur.x][cur.y-1] > dmap[cur.x][cur.y] && recmap[cur.x][cur.y - 1] == noneRec)
			{
				queue.emplace_front(cur.x, cur.y - 1);
				dmap[cur.x][cur.y - 1] = w;
			}			
			if (cur.x > 0 && dmap[cur.x-1][cur.y] > dmap[cur.x][cur.y] && recmap[cur.x - 1][cur.y] == noneRec)
			{
				queue.emplace_front(cur.x - 1, cur.y);
				dmap[cur.x - 1][cur.y] = w;
			}			
			if (cur.y < dmap.h - 1 && dmap[cur.x][cur.y + 1] > dmap[cur.x][cur.y] && recmap[cur.x][cur.y + 1] == noneRec)
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

    void move()
    {
		GOld << "gold: " << gold;
		SKill << "skill: " << skill;
		field.setView(playerLayer.get(0).pos());
		if (empty(direction) && playerLayer.get(0).anim.isEmpty())
		{
			for (auto n : nishtyaki.find(playerLayer.get(0).pos()))
			{
				if (nishtyaki.data(n).type == golda)
				{
					gold += 100;
				}
				else
				{
					skill += 100;
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
    }

	GameMap groundmap, GroundObjMap, MobsMap, Nishtyakmap, recmap;
	FromDesign(Button, start_button);
	FromDesign(Button, quit_button);
	FromDesign(Button, load_button);
	FromDesign(Button, Relog);
	FromDesign(Button, newDirection);
	FromDesign(Label, test_);
	FromDesign(Selector, selector);

	FromDesign(Selector, heroMenuSelector);
	FromDesign(Button, select0);
	FromDesign(Button, select1);
	FromDesign(Button, select2);
	FromDesign(Button, heroMenu);

	FromDesign(GameView, field);
	FromDesign(Label, GOld);
	FromDesign(Label, SKill);
	FromDesign(Label, emptyStep);
	LayerFromDesign(void, mobs);
	LayerFromDesign(void, ground);
	LayerFromDesign(void, forest);
	LayerFromDesign(playerData, playerLayer);
	LayerFromDesign(recData, rec);
	LayerFromDesign(nishData, nishtyaki);
	LayerFromDesign(void, SuperMegaPuperStepEgg);	
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