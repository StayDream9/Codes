#include "Control.h"


const QPointF scoreTextPos = QPointF(600, 420); //分数标签位置
const QPointF LifeBarPos = QPointF(600,450); //血条标签位置
const QPointF SkillBarPos = QPointF(600, 470); //技能条位置

const int myBulletShootTimerItv = 300; //我的子弹发射速度
const int enemyBulletShootTimerItv = 1000; //敌军子弹发射速度
const int allBulletMoveTimerItv = 10; //所有子弹的移动速度

const int myPlaneMoveTimerItv = 30; //我的飞机移动速度
const int enemyPlaneMoveTimerItv = 50; //敌军飞机移动速度
const int enemyPlaneGenerateTimerItv = 3000; //敌军创建普通新飞机的速度
const int bossGenerateTimeItv = 5000; //敌军创建boss新飞机的速度

Control::Control()
{
    setSceneRect(0,0,512,768);

    this->myPlaneImageFile = ":/images/myplane.png";
    this->myLife = 50;
    this->mySkill = 5;

    this->myBulletImageFile = ":/images/mybullet.png";
    this->myBulletPower = 1;

    this->enemyPlaneImageFile = ":/images/enemyplane.png";
    this->enemyLife = 1;

    this->enemyBulletImageFile = ":/images/enemybullet.png";
    this->enemyBulletPower = 1;

    this->bossImageFile = ":/images/boss.png";
    this->bossLife = 10;

    this->bossBulletImageFile = ":/images/bossbullet.png";
    this->bossBulletPower = 2;

    this->lifeSupplyImageFile = ":/images/lifesupply.png";

    /* 生命值 */
    lifeFrameBar = new QGraphicsRectItem(LifeBarPos.x(), LifeBarPos.y(), myLife*2,5);
    lifeFrameBar->setPen(QPen(Qt::red));
    addItem(lifeFrameBar); //将这个Item加入场景中
    lifeFrameBar->hide();
    lifeBar = new QGraphicsRectItem(LifeBarPos.x(), LifeBarPos.y(), myLife*2, 5);
    lifeBar->setBrush(QBrush(Qt::green));
    addItem(lifeBar);
    lifeBar->hide();

    /* 技能值 */
    skillFrameBar = new QGraphicsRectItem(SkillBarPos.x(),SkillBarPos.y(), myLife*2,5);
    skillFrameBar->setPen(QPen(Qt::white));
    addItem(skillFrameBar);
    skillFrameBar->hide();//
    skillBar = new QGraphicsRectItem(SkillBarPos.x(), SkillBarPos.y(), mySkill*2, 5);
    skillBar->setBrush(QBrush(Qt::blue));
    addItem(skillBar);
    skillBar->hide();//

    /* 初始化场景，播放背景音乐 */
    playList = new QMediaPlaylist;
    playList->addMedia(QUrl("qrc:///music/starwars.mp3"));
    playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop); //单曲循环
    player = new QMediaPlayer(this);
    player->setPlaylist(playList);
    player->play();

    /* 遮罩面板 */
    QPixmap pixmap(QPixmap(QString::fromStdString(myPlaneImageFile)));
    QWidget *mask = new QWidget;
    mask->setAutoFillBackground(true);
    //QColor(int r,int g,int b,int a = ...)
    //a:透明度
    mask->setPalette(QPalette(QColor(0, 0, 0, 80)));
    mask->resize(width(),height());
    maskWidget = addWidget(mask);
    maskWidget->setPos(0 + pixmap.width(),0);
    maskWidget->setZValue(1); //设置处在z值为0的图形项上方
    maskWidget->hide();

    /* 得分提示 */
    myBulletType = 0; //子弹类型初始化
    score = 0; //得分清零
    scoreText = new QGraphicsTextItem;
    scoreText->setPos(scoreTextPos);
    scoreText->setHtml(tr("<font color=white>SCORE: %1</font>").arg(score));
    scoreText->setFont(QFont("Courier"));
    addItem(scoreText);
    scoreText->hide();

    /* 游戏开始按钮 */
    QPushButton *startGameBtn = new QPushButton("Start Game");
    startGameBtn->setFont(QFont("Algerian",18));
    startGameBtn->setStyleSheet("QPushButton{background: transparent; color:white; }"
                                "QPushButton:hover{color:red;}");
    connect(startGameBtn,SIGNAL(clicked()),this,SLOT(startGame()));
    startGameButton = addWidget(startGameBtn);
    startGameButton->setPos(300,250);
    startGameButton->setZValue(2);
    startGameButton->hide();

    /* 游戏帮助 */
    QPushButton *helpGameBtn = new QPushButton(tr("Help"));
    helpGameBtn->setFont(QFont("Algerian",18));
    helpGameBtn->setStyleSheet("QPushButton{background: transparent; color:white; }"
                                   "QPushButton:hover{color:red;}");
    connect(helpGameBtn,SIGNAL(clicked()),this,SLOT(showHelpMessage()));
    helpGameButton = addWidget(helpGameBtn);
    helpGameButton->setPos(350,300);
    helpGameButton->setZValue(2); //
    helpGameButton->hide();

    /* 退出游戏 */
    QPushButton *quitGameBtn = new QPushButton(tr("Quit Game"));
    quitGameBtn->setFont(QFont("Algerian",18));
    quitGameBtn->setStyleSheet("QPushButton{background: transparent; color:white; }"
                               "QPushButton:hover{color:red;}");
    connect(quitGameBtn,SIGNAL(clicked()),this,SLOT(quitGame()));
    quitGameButton = addWidget(quitGameBtn);
    quitGameButton->setPos(310, 350);
    quitGameButton->setZValue(2);
    quitGameButton->hide();

    /* 游戏暂停提示 */
    isPause = false;//未暂停
    QPushButton *continueGameBtn = new QPushButton(tr("Resume"));
    continueGameBtn->setFont(QFont("Algerian",18));
    continueGameBtn->setStyleSheet("QPushButton{background: transparent; color:white; }"
                                   "QPushButton:hover{color:red;}");
    connect(continueGameBtn,SIGNAL(clicked()),this,SLOT(pauseGame()));
    continueGameButton = addWidget(continueGameBtn);
    continueGameButton->setPos(330,250);
    continueGameButton->setZValue(2);
    continueGameButton->hide();

    /* 游戏标题 */
    titleText = new QGraphicsTextItem;
    addItem(titleText);
    titleText->setHtml(tr("<font color=white>WAR OF PLANES</font>"));
    titleText->setFont(QFont("Algerian", 30));
    titleText->setPos(200,100);
    titleText->setZValue(2); 
    titleText->hide();

    /* 作者信息 */
    authorText = new QGraphicsTextItem;
    addItem(authorText);
    authorText->setHtml(tr("<font color=white>Copyright © 2017,Zhang Shuai. All Rights Reserved."));
    authorText->setFont(QFont("Courier"));
    authorText->setPos(100, 500);
    authorText->setZValue(2);
    authorText->hide();

    /* 游戏终止提示 */
    gameLostText = new QGraphicsTextItem;
    addItem(gameLostText);
    gameLostText->setHtml(tr("<font color=white>Game Over</font>"));
    gameLostText->setFont(QFont("Algerian", 22));
    gameLostText->setPos(150, 150);
    gameLostText->setZValue(2);
    gameLostText->hide();

    /* 重试 */
    QPushButton *retryGameBtn = new QPushButton(tr("Retry"));
    retryGameBtn->setFont(QFont("Algerian",18));
    retryGameBtn->setStyleSheet("QPushButton{background: transparent; color:white; }"
                                "QPushButton:hover{color:red;}");
    connect(retryGameBtn,SIGNAL(clicked()),this,SLOT(retryGame()));
    retryGameButton = addWidget(retryGameBtn);
    retryGameButton->setPos(345,250);
    retryGameButton->setZValue(2);
    retryGameButton->hide();

    /* 进入欢迎界面，之后按Enter开始游戏 */
    welcomeGame();
}

void Control::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==myPlaneMoveTimerId)
        changePlanePosition(myplane, myplane->x()+myPlaneMove.x(), myplane->y()+myPlaneMove.y());
    if(event->timerId()==enemyBulletShootTimerId)
        shootEnemyBullets();
    else if(event->timerId()==myBulletShootTimerId)
        shootBullet();
    else if(event->timerId()==allBulletMoveTimerId)
    {
        updateMyBullets();
        updateEnemyBullets();
    }
    else if(event->timerId()==enemyPlaneMoveTimerId)
        updateEnemyPlanes();
    else if(event->timerId()==enemyPlaneGenerateTimerId)
    {
        for(int i=0;i<2;i++)
            generateEnemyPlane();
    }
    else if(event->timerId()==bossGenerateTimeId)
        generateBoss();
    else if(event->timerId()==skillQTimerId)
        myBulletType = 0;
}

//键盘按下事件
void Control::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_W && !event->isAutoRepeat())
    {
        if(myPlaneMove==QPointF(0,0))
            myPlaneMoveTimerId = startTimer(myPlaneMoveTimerItv);
        myPlaneMove = QPointF(0, -10);
    }
    else if(event->key()==Qt::Key_S && !event->isAutoRepeat())
    {
        if(myPlaneMove==QPointF(0,0))
            myPlaneMoveTimerId = startTimer(myPlaneMoveTimerItv);
        myPlaneMove = QPointF(0, 10);
    }
    else if(event->key()==Qt::Key_A && !event->isAutoRepeat())
    {
        if(myPlaneMove==QPointF(0,0))
            myPlaneMoveTimerId = startTimer(myPlaneMoveTimerItv);
        myPlaneMove = QPointF(-10, 0);
    }
    else if(event->key()==Qt::Key_D && !event->isAutoRepeat())
    {
        if(myPlaneMove==QPointF(0,0))
            myPlaneMoveTimerId = startTimer(myPlaneMoveTimerItv);
        myPlaneMove = QPointF(10, 0);
    }
    else if(event->key()==Qt::Key_J && myplane->skill>=5)
    {
        //按J的技能可以一次发射3个子弹，但是会消耗5点技能
        myBulletType = 1;
        myplane->skill -= 5;
        updateBar(skillBar, skillFrameBar, SkillBarPos, -10, QBrush(Qt::blue));
        skillQTimerId = startTimer(5000); //5秒使用时间
    }
    else if(event->key()==Qt::Key_K && myplane->skill>=3)
    {
        //按K的技能可以打掉所有飞机，消耗3点技能值
        for(vector<EnemyPlane*>::iterator iter=enemyplanes.begin(); iter!=enemyplanes.end(); iter++)
        {
            score++;
            scoreText->setHtml(tr("<font color=white>SCORE: %1</font>").arg(score));
            removeItem(*iter);
            delete *iter;
        }
        enemyplanes.clear();

        myplane->skill -= 3; //消耗3点技能值
        updateBar(skillBar, skillFrameBar, SkillBarPos, -6, QBrush(Qt::blue));
    }
    else if(event->key()==Qt::Key_L && myplane->skill>=7)
    {
        //按L可以消掉所有敌机子弹，消耗7点技能值
        for(vector<Bullet*>::iterator it = enemybullets.begin(); it!= enemybullets.end(); it++)
        {
            removeItem(*it);
            delete *it;
        }
        enemybullets.clear();

        myplane->skill -= 7;
        updateBar(skillBar, skillFrameBar, SkillBarPos, -14, QBrush(Qt::blue));
    }
    else if(event->key()==Qt::Key_Space)
        pauseGame();
}

//键盘释放事件
void Control::keyReleaseEvent(QKeyEvent *event)
{
    if((event->key()==Qt::Key_W || event->key()==Qt::Key_S || event->key()==Qt::Key_A || event->key()==Qt::Key_D)
            && !event->isAutoRepeat())
    {
        myPlaneMove = QPointF(0, 0);
        killTimer(myPlaneMoveTimerId);
    }
}

//生成敌机
bool Control::generateEnemyPlane()
{
	/* 随机在第一行生成敌机 */
    srand(time(NULL));//初始化时间种子
    QPixmap pixmap(QPixmap(QString::fromStdString(enemyPlaneImageFile)));
    int cnt = 0;
    int x = rand() % static_cast<int>(width()); //敌机最左端位置
    for(cnt=0;cnt<100;cnt++)
    {
        bool flag = true; //此位置是否合法
        for(auto iter:enemyplanes)
        {
            //碰撞检测
            if(iter->sceneBoundingRect().intersects(QRectF(x, 0, pixmap.width(), pixmap.height())))
            {
                flag = false;
                break;
            }
        }

        if(flag)
            break;
        else
            x = rand() % static_cast<int>(width());
    }

    /* 若生成100次随机都未能找到合适的位置则退出 */
    if(cnt>=100)
        return false;

    /* 新增敌机 */
    EnemyPlane *enemy = new EnemyPlane(x, 0, enemyPlaneImageFile, this, ORD, enemyLife);
    enemyplanes.push_back(enemy);
    return true;
}

//生成boss敌机
bool Control::generateBoss()
{
    /* 随机在第一行生成敌机 */
    srand(time(NULL));//初始化时间种子
    QPixmap pixmap(QPixmap(QString::fromStdString(bossImageFile)));
    int cnt = 0;
    int x = rand() % (int)width(); //敌机最左端位置
    for(cnt=0;cnt<100;cnt++)
    {
        bool flag = true; //此位置是否合法
        for(auto iter:enemyplanes)
        {
            if(iter->sceneBoundingRect().intersects(QRectF(x, 0, pixmap.width(), pixmap.height())))
            {
                flag = false;
                break;
            }
        }

        if(flag)
            break;
        else
            x = rand() % static_cast<int>(width());
    }

    /* 若生成100次随机都未能找到合适的位置则退出 */
    if(cnt>=100)
        return false;

    /* 新增敌机 */
    EnemyPlane *enemy = new EnemyPlane(x, 0, bossImageFile, this, BOSS, bossLife);
    enemyplanes.push_back(enemy);
    return true;
}

//改变飞机位置
bool Control::changePlanePosition(Plane *plane, int newX, int newY)
{
    QPixmap pixmap(QPixmap(QString::fromStdString(myPlaneImageFile)));
    /* 检查位置是否有变化，无变化则直接返回 */
    if (plane->x() == newX && plane->y() == newY)
		return true;

    /* 检查新位置是否合法，不合法则直接返回 */
    if (newX<pixmap.width() || newX>width() || newY<0 || newY>height())
	{
        if (plane->part == ME) //玩家飞机不允许出界
			return true;
        else if (plane->part == ENEMY) //敌机不允许出上界、左界、右界，但出下界之后将被删除
		{
            if (newY>height())
			{
                plane->delScreen(this);
				return false;
			}
			else
				return true;
		}
	}

    /* 若为玩家飞机，则首先检查是否与补给碰撞，遇到生命补给则+5 */
    if(plane->part==ME)
        for(vector<Object*>::iterator it=lifesupplys.begin();it!=lifesupplys.end(); )
        {
            if(plane->collidesWithItem(*it))//边界检测
            {
                plane->life = min(plane->life+10, myLife); //避免血条超出框架
                updateBar(lifeBar,lifeFrameBar,LifeBarPos,+20,Qt::green);
                removeItem(*it);
                delete *it;
                it = lifesupplys.erase(it);
            }
            else
                it++;
        }

	/* 检查新位置是否与某一飞机位置冲突 */
	/* 首先检查是否与敌机碰撞 */
	for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); ) //遍历敌机
	{
		if (plane == *it) //跳过自己
		{
			it++;
			continue;
		}

		bool alive = true; //it所指向的飞机是否还有生命值
        if (plane->collidesWithItem(*it))
		{
			/* 若为玩家飞机，两架飞机均crash，生命值都减1 */
            if (plane->part == ME)
			{
                plane->crash(this);
                alive = (*it)->crash(this);
                updateBar(lifeBar, lifeFrameBar, LifeBarPos, -2, QBrush(Qt::green));
			}
            if (plane->part == ENEMY) //若同为敌机，则不允许改变位置，NOCHANGE
				return true;
		}

		if (alive)
			it++;
		else //若飞机坠毁，则将此飞机去掉
		{
			delete *it;
			it = enemyplanes.erase(it);
		}
	}

    /* 若此飞机为敌机，且与玩家飞机发生了碰撞 */
    if (plane->part == ENEMY && plane->collidesWithItem(myplane))
    {
        myplane->crash(this);
        plane->crash(this);
        updateBar(lifeBar, lifeFrameBar, LifeBarPos, -2, QBrush(Qt::green));
    }

	/* 若plane存活，则更改坐标并同步屏幕 */
	if (plane->life > 0)
	{
        plane->moveBy(newX-plane->x(), newY-plane->y());
        plane->update();
	}

	return plane->life > 0;
}

//更新敌机
bool Control::updateEnemyPlanes()
{
    /* 若当前敌机少于1，则自动生成敌机，数目随机但小于3 */
    if (enemyplanes.size() < 1)
	{
        int genNum = rand() % 3;
		for (int i = 0; i < genNum; i++)
			generateEnemyPlane();
    }

	/* 所有敌机移动位置 */
	for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); )
	{
        //qDebug() << it-enemyplanes.begin() << "  (" << (*it)->x() << "," << (*it)->y() << ")";
		pair<int, int> loc = (*it)->updatePosition();
		if (changePlanePosition(*it, loc.first, loc.second))
			it++;
        else
            it = enemyplanes.erase(it);
	}
    return myplane->life > 0;
}

//子弹位置变化
bool Control::changeBulletPosition(Bullet * bullet, int newX, int newY)
{
	/* 检查位置是否有变化，无变化则返回true */
    if (bullet->x() == newX && bullet->y() == newY)
		return true;

	/* 检查子弹是否击中某一飞机 */
	/* 首先检查玩家飞机 */
    if (bullet->part==ENEMY && bullet->collidesWithItem(myplane))
	{
        bullet->hit(this);
        myplane->crash(this);
        updateBar(lifeBar, lifeFrameBar, LifeBarPos, -2, QBrush(Qt::green));
        if(myplane->life<=0)
            loseGame();
	}
    else if(bullet->part==ME)
    {
        /* 然后检查敌机：若敌机已经没有生命值，就从enemyplanes中删去 */
        for (vector<EnemyPlane*>::iterator it = enemyplanes.begin(); it != enemyplanes.end(); )
        {
            bool alive = true;
            if (bullet->collidesWithItem(*it))
            {
                bullet->hit(this);
                alive = (*it)->crash(this);
                myplane->skill++; //技能点增加
                updateBar(skillBar, skillFrameBar, SkillBarPos, +2, QBrush(Qt::blue));
                score++;
                scoreText->setHtml(tr("<font color=white>SCORE: %1</font>").arg(score));
            }

            if(alive)
                it++;
            else
            {
                delete *it;
                it = enemyplanes.erase(it);
                /* 25%的概率掉落生命补给 */
                srand(time(NULL));
                if(rand()%4==0)
                {
                    Object *lifeSupply = new Object(LIFESUPPLY, lifeSupplyImageFile);
                    lifeSupply->setPos(bullet->pos());
                    lifesupplys.push_back(lifeSupply);
                    addItem(lifeSupply);
                }
            }
        }
    }

	/* 若子弹还具有杀伤力则更新位置并同步屏幕 */
	if (bullet->power > 0)
	{
		/* 若此时子弹试图打出边界，则销毁子弹 */
        if (newX <= 0 || newX >= width() || newY <= 0 || newY >= height())
		{
            bullet->delScreen(this);
            return false;
		}
        bullet->synScreen(this);
        bullet->moveBy(newX-bullet->x(), newY-bullet->y());
        bullet->update();
	}

	return bullet->power>0;
}

void Control::updateEnemyBullets()
{
    for(vector<Bullet*>::iterator it = enemybullets.begin(); it!=enemybullets.end(); )
    {
        pair<int ,int> loc = (*it)->updatePosition();
        if(changeBulletPosition(*it, loc.first, loc.second))
            it++;
        else
        {
            delete *it;
            it = enemybullets.erase(it);
        }
    }
}

//敌军子弹的发射
void Control::shootEnemyBullets()
{
    /* 仍存活的敌机发出新子弹，新子弹在敌机炮管外的位置 */
    for (vector<EnemyPlane*>::iterator iter = enemyplanes.begin(); iter != enemyplanes.end(); iter++)
		if ((*iter)->life > 0)
        {
            if((*iter)->type==ORD)
            {//子弹的图片和发射位置
                Bullet *bullet = new Bullet(ENEMY,
                                            (*iter)->x()+(*iter)->pixmap().width()/2-8,
                                            (*iter)->y()+(*iter)->pixmap().height()-15,
                                            enemyBulletImageFile, QPointF(0,1),
                                            enemyBulletPower);
                enemybullets.push_back(bullet);
                addItem(bullet);
            }
            else if((*iter)->type==BOSS)
            {
                Bullet *bullet0 = new Bullet(ENEMY,
                                             (*iter)->x()+(*iter)->pixmap().width()/2-8,
                                             (*iter)->y()+(*iter)->pixmap().height()-15,
                                             bossBulletImageFile, QPointF(0,1),
                                             bossBulletPower);
                enemybullets.push_back(bullet0);
                addItem(bullet0);

                Bullet *bullet1 = new Bullet(ENEMY,
                                             (*iter)->x()+(*iter)->pixmap().width()/2-8,
                                             (*iter)->y()+(*iter)->pixmap().height()-15,
                                             bossBulletImageFile, QPointF(-1,1),
                                             bossBulletPower);
                bullet1->setRotation(45); //旋转45度
                enemybullets.push_back(bullet1);
                addItem(bullet1);

                Bullet *bullet2 = new Bullet(ENEMY,
                                             (*iter)->x()+(*iter)->pixmap().width()/2-8,
                                             (*iter)->y()+(*iter)->pixmap().height()-15,
                                             bossBulletImageFile, QPointF(1,1),
                                             bossBulletPower);
                bullet2->setRotation(-45);
                enemybullets.push_back(bullet2);
                addItem(bullet2);
            }
        }
}

//更新子弹位置
void Control::updateMyBullets()
{
    for (vector<Bullet*>::iterator it = mybullets.begin(); it != mybullets.end(); )
	{
		pair<int, int> loc = (*it)->updatePosition();
		if (changeBulletPosition(*it, loc.first, loc.second))
			it++;
		else
		{
			delete *it;
            it = mybullets.erase(it);
        }
    }
}

//玩家子弹的发射
void Control::shootBullet()
{
    /* 玩家飞机发出新子弹，新子弹在玩家飞机炮管外的位置 */
    if(myBulletType==0) //技能类型
    {
        Bullet *bullet = new Bullet(ME, myplane->x()+40, myplane->y()-38,
                                    myBulletImageFile, QPointF(0,-3), myBulletPower);
        mybullets.push_back(bullet);
        addItem(bullet);
    }
    else if(myBulletType==1)
    {
        Bullet *bullet1 = new Bullet(ME, myplane->x()+40, myplane->y()-38,
                                    myBulletImageFile, QPointF(-3,-3), myBulletPower);
        mybullets.push_back(bullet1);
        bullet1->setRotation(-45);
        addItem(bullet1);

        Bullet *bullet2 = new Bullet(ME, myplane->x()+40, myplane->y()-38,
                                    myBulletImageFile, QPointF(0,-3), myBulletPower);
        mybullets.push_back(bullet2);
        addItem(bullet2);

        Bullet *bullet3 = new Bullet(ME, myplane->x()+40, myplane->y()-38,
                                    myBulletImageFile, QPointF(3,-3), myBulletPower);
        mybullets.push_back(bullet3);
        bullet3->setRotation(45);
        addItem(bullet3);
    }
}

//更新血条或技能条的状态
void Control::updateBar(QGraphicsRectItem *bar, QGraphicsRectItem *frameBar, const QPointF &pos, qreal var, const QBrush &brush)
{
    qreal wid = bar->rect().width();
    if(var<0)
        wid = max(static_cast<qreal>(0), wid+var); //保证血条或技能条不会超出框架
    else
        wid = min(frameBar->rect().width(), wid+var); //保证血条或技能条不会超出框架
    bar->setRect(pos.x(), pos.y(), wid, bar->rect().height());
    bar->setBrush(brush);
    bar->update();
}

//暂停游戏
void Control::pauseGame()
{
    if(!isPause)
    {
        isPause = true;
        killTimer(myBulletShootTimerId);
        killTimer(enemyBulletShootTimerId);
        killTimer(allBulletMoveTimerId);
        killTimer(enemyPlaneMoveTimerId);
        killTimer(enemyPlaneGenerateTimerId);
        killTimer(bossGenerateTimeId);
        maskWidget->show();
        continueGameButton->show();
        helpGameButton->show();
        quitGameButton->show();
    }
    else
    {
        isPause = false;
        myBulletShootTimerId = startTimer(myBulletShootTimerItv);
        enemyBulletShootTimerId = startTimer(enemyBulletShootTimerItv);
        allBulletMoveTimerId = startTimer(allBulletMoveTimerItv);
        enemyPlaneMoveTimerId = startTimer(enemyPlaneMoveTimerItv);
        enemyPlaneGenerateTimerId = startTimer(enemyPlaneGenerateTimerItv);
        bossGenerateTimeId = startTimer(bossGenerateTimeItv);
        maskWidget->hide();
        continueGameButton->hide();
        helpGameButton->hide();
        quitGameButton->hide();
    }
}

//游戏结束
void Control::loseGame()
{
    killTimer(myBulletShootTimerId);
    killTimer(enemyBulletShootTimerId);
    killTimer(allBulletMoveTimerId);
    killTimer(enemyPlaneMoveTimerId);
    killTimer(enemyPlaneGenerateTimerId);
    killTimer(bossGenerateTimeId);
    maskWidget->show();
    gameLostText->setHtml(tr("<font color=white>Game Over, Your Score: %1</font>").arg(score));
    gameLostText->show();
    retryGameButton->show();
    helpGameButton->show();
    quitGameButton->show();
}

//重玩游戏
void Control::retryGame()
{
    //玩家飞机已毁，故不需要再次删除

    for(auto iter:enemyplanes)
    {
        removeItem(iter);
        delete iter;
    }
    enemyplanes.clear();

    for(auto it:mybullets)
    {
        removeItem(it);
        delete it;
    }
    mybullets.clear();

    for(auto it:enemybullets)
    {
        removeItem(it);
        delete it;
    }
    enemybullets.clear();

    for(auto it:lifesupplys)
    {
        removeItem(it);
        delete it;
    }
    lifesupplys.clear();

    gameLostText->hide();
    retryGameButton->hide();
    helpGameButton->hide();
    quitGameButton->hide();
    startGame();
}

//退出游戏
void Control::quitGame()
{
    exit(0);
}

//欢迎界面
void Control::welcomeGame()
{
    hasStarted = false;
    maskWidget->show();
    titleText->show();
    startGameButton->show();
    helpGameButton->show();
    quitGameButton->show();
    authorText->show();
}

//开始游戏
void Control::startGame()
{
    score = 0;
    scoreText->setHtml(tr("<font color=white>SCORE: %1</font>").arg(score));

    hasStarted = true; //是否已经开始了游戏
    titleText->hide();
    startGameButton->hide();
    helpGameButton->hide();
    quitGameButton->hide();
    authorText->hide();
    maskWidget->hide();

    scoreText->show();
    lifeFrameBar->show();
    lifeBar->setRect(LifeBarPos.x(), LifeBarPos.y(), myLife*2, lifeBar->rect().height());
    lifeBar->setBrush(Qt::green);
    lifeBar->update();
    lifeBar->show();

    skillFrameBar->show();
    skillBar->setRect(SkillBarPos.x(), SkillBarPos.y(), mySkill*2, skillBar->rect().height());
    skillBar->setBrush(Qt::blue);
    skillBar->update();
    skillBar->show();

    /* 设置各动作更新时钟 */
    myPlaneMove = QPointF(0, 0);
    myBulletShootTimerId = startTimer(myBulletShootTimerItv);
    enemyBulletShootTimerId = startTimer(enemyBulletShootTimerItv);
    allBulletMoveTimerId = startTimer(allBulletMoveTimerItv);
    enemyPlaneMoveTimerId = startTimer(enemyPlaneMoveTimerItv);
    enemyPlaneGenerateTimerId = startTimer(enemyPlaneGenerateTimerItv);
    bossGenerateTimeId = startTimer(bossGenerateTimeItv);

    /* 添加玩家飞机 */
    myplane = new MyPlane(width() / 2, height() / 2, myPlaneImageFile, this, myLife, mySkill);
    myplane->synScreen(this);

    /* 添加敌机 */
    for (int i = 0; i < 3; i++)
        generateEnemyPlane();
}

//展示帮助信息
void Control::showHelpMessage()
{
    QString helpText;
    helpText += tr("欢迎来到飞机大战\n");
    helpText += tr("\n当你击落敌机时，你的得分和技能值都会相应地增加\n");
    helpText += tr("若你被敌机击中或撞到，你的生命值会相应减少\n");
    helpText += tr("当你的技能值达到一定量的时候，便可释放技能\n");
    helpText += tr("\n进度条：\n");
    helpText += tr("绿色进度条：你的生命值\n");
    helpText += tr("蓝色进度条：你的技能值\n");
    helpText += tr("\n操作：\n");
    helpText += tr("W：上； S：下； A：左； D：右\n");
    helpText += tr("J：超级子弹技能，一次发射三个方向的子弹，消耗5点技能\n");
    helpText += tr("K：轰炸技能，一次性炸掉所有敌机，消耗3点技能\n");
    helpText += tr("L：子弹拦截技能，一次性拦截掉所有子弹，消耗7点技能\n");
    helpText += tr("空格键：暂停游戏\n");
    helpText += tr("\n补给：\n");
    helpText += tr("被击毁的敌机随机生成补给，即绿色的加号，每次可补充10点生命值");
    QMessageBox::information(NULL,tr("游戏帮助"),helpText);
}
