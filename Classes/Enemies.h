#ifndef __ENEMIES_H__
#define __ENEMIES_H__

#include "cocos2d.h"
#include <vector>

using namespace cocos2d;

class Enemies : public Node
{

public:

	Enemies(Sprite *character, TMXTiledMap *map, TMXLayer *wall, cocos2d::Vector<cocos2d::Sprite *> *_projectiles, int *ammoplasmagun, cocos2d::Vector<cocos2d::Sprite *> *_projTurret, int *hp, int *priorityGun, TMXLayer *ammo);

	void spawnEnemy();

	void enemyOnScreen();

	void projCollisionEnemy();

	void spawnerTimer();

	void enemyCollisionCharacter();

	void gameOverEnemy();

	void turretProjReady();
private:
	
	int *ammoplasmagun;
	int *hp;
	int *priorityGun;

	Sprite *character;
	Sprite *enemy;
	Sprite *turret;
	Sprite *explosion;

	TMXTiledMap *map;

	TMXLayer *wall;
	TMXLayer *ammo;

	TMXObjectGroup *objectGroup;

	cocos2d::Vector<cocos2d::Sprite *> _visibleEnemies;
	cocos2d::Vector<cocos2d::Sprite *> _enemies;
	cocos2d::Vector<cocos2d::Sprite *> _turrets;
	cocos2d::Vector<cocos2d::Sprite *> *_projectiles;
	cocos2d::Vector<cocos2d::Sprite *> *_projTurret;

	std::vector<cocos2d::Point> _spawnCoord;

	void enemyMoveFinished(Node *pSender);

	void turretMoveFinished(Node *pSender);

	void animateEnemy(Sprite *enemy);

	void animateTurret(Sprite *turret);

	void addEnemyAtPos(Point position);

	void addTurretAtPos(Point position);

	void spawner();

	void projExplosion(Sprite *projectile);

	void turretProjectile(Sprite *target);

	void turretProjDone();

	void projectileMoveFinished(cocos2d::Node *pSender);

	void delayAttackEnemy(float dt);

	double slowDownEnemy(Sprite *enemy);

	Point tileCoordForPosition(Point position);
};

#endif // __ENEMIES_H__