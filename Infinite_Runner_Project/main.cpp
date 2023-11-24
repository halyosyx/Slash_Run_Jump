#include <cstdio>
#include <math.h>
#include "raylib.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 397;
const char* TITLE_NAME = "Warrior Runner";

const float gravity = 980.0;
const float jumpForce = -300.0;
const float updateTime = 1.0 / 12.0;
const float enemyUpdateTime = 1.0/12.0;
const float startSpawnTime = 3.0;
const float startIFrames = 4.0;

float velocity = 0;
float scale = 2;
float runTime = 0;
float enemyRuntime = 0;
float spawnTime = 0.0;
bool isGrounded;
bool peak;

float enemyVel = -200;
float iFrames = 0;

int enemyDeadCount = 0;
int health = 3;
bool isHit = false;
bool isDead = false;

struct CharacterData
{
    Texture2D texture;
    Rectangle characterRect;
    Vector2 position;
    int frame;
    float runningTime;
    float updateTime;
};

struct BackGroundData
{
    Texture2D background;
    Vector2 position;
    float bgSpeed;
};

struct PlatformData
{
    Texture2D texture;
    Rectangle rec;
    Vector2 position;
    float speed;
};


void DrawCharacterTexture(Texture2D texture, Rectangle source, Vector2 position, float textureScale, Color tint);
void SlimeEnemy(Texture2D texture, Rectangle rect, Vector2 position, int frame, float deltaTime);
bool GroundCheck(CharacterData characterData, PlatformData platformData);
CharacterData UpdateData(CharacterData data,int maxFrame ,float deltaTime);
BackGroundData UpdateBackground(BackGroundData data, float deltaTime);

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE_NAME);
    SetTargetFPS(60);

    CharacterData enemy {
        {LoadTexture("textures/Slime.png")}, 
        {0,0, (enemy.texture.width / 4), enemy.texture.height}, 
        {(SCREEN_WIDTH - enemy.characterRect.width / 2),SCREEN_HEIGHT - (enemy.characterRect.height * scale)}
        , 0, 0, 1.0 / 12.0
    };
 
    BackGroundData FarBackground {
    {LoadTexture("textures/FarTrees.png")}, 
    {0.0, -FarBackground.background.height / 2}, 
    {90.0}};

    BackGroundData BackTreesBackground {
    {LoadTexture("textures/BackTrees.png")}, 
    {0.0, -BackTreesBackground.background.height / 2}, 
    {200.0}};

    BackGroundData Foreground {
    {LoadTexture("textures/Trees.png")}, 
    {0.0, -Foreground.background.height / 2}, 
    {400.0}};

    BackGroundData TopTrees {
    {LoadTexture("textures/TopTrees.png")}, 
    {0.0, -TopTrees.background.height / 2}, 
    {400.0}};

    BackGroundData FarLights {
    {LoadTexture("textures/FarLights.png")}, 
    {0.0, -FarLights.background.height / 2}, 
    {150.0}};

    BackGroundData CloseLights {
    {LoadTexture("textures/CloseLights.png")}, 
    {0.0, -CloseLights.background.height / 2}, 
    {300.0}};

    PlatformData Ground{
        {LoadTexture("textures/GroundGrass.png")},
        {Ground.texture.width,Ground.texture.height},
        {0.0, SCREEN_HEIGHT - Ground.texture.height},
        {400.0}
    };
    
    // Player Animations
    CharacterData player {
        {LoadTexture("textures/WarriorRun.png")},
        {0,0,player.texture.width / 8, player.texture.height},
        {(SCREEN_WIDTH / 2 - player.characterRect.width / 2) / 6, (Ground.position.y + 7.5) - (player.characterRect.height * scale)},
        0, 0, 1.0 / 12.0
    };

    CharacterData playerJump {
    {LoadTexture("textures/WarriorJump.png")},
    {0,0,playerJump.texture.width / 3, playerJump.texture.height},
    {(SCREEN_WIDTH / 2 - playerJump.characterRect.width / 2) / 6, (Ground.position.y + 7.5) - (playerJump.characterRect.height * scale)},
    0, 0, 1.0 / 12.0
    };

    CharacterData playerFall {
        {LoadTexture("textures/WarriorFall.png")},
        {0,0,playerFall.texture.width / 3, playerFall.texture.height},
        {(SCREEN_WIDTH / 2 - playerFall.characterRect.width / 2) / 6, (Ground.position.y + 7.5) - (playerFall.characterRect.height * scale)},
        0, 0, 1.0 / 12.0
    };

    CharacterData playerHit{
        {LoadTexture("textures/WarriorHurt.png")},
        {0,0,playerHit.texture.width / 4, playerHit.texture.height},
        {(SCREEN_WIDTH / 2 - playerHit.characterRect.width / 2) / 6, (Ground.position.y + 7.5) - (playerHit.characterRect.height * scale)},
        0, 0, 1.0 / 12.0
    };

    CharacterData playerDeath{
        {LoadTexture("textures/WarriorDeath.png")},
        {0,0,playerDeath.texture.width / 11, playerDeath.texture.height},
        {(SCREEN_WIDTH / 2 - playerDeath.characterRect.width / 2) / 6, (Ground.position.y + 7.5) - (playerDeath.characterRect.height * scale)},
        0, 0, 1.0 / 12.0
    };

    int count = 0;
    const int enemySize = 4;
    CharacterData enemies[enemySize] {};

    for (int i = 0; i < enemySize; i++)
    {
        enemies[i].texture = {LoadTexture("textures/Slime.png")};
        enemies[i].characterRect.x = 0;
        enemies[i].characterRect.y = 0;
        enemies[i].characterRect.width = (enemy.texture.width / 4);
        enemies[i].characterRect.height = enemy.texture.height;
        enemies[i].position.y = (Ground.position.y + 7.5) - (enemy.characterRect.height * scale);
        enemies[i].frame = 0;

        enemies[i].runningTime = 0;
        enemies[i].updateTime = 1.0 / 16.0;

        enemies[i].position.x = SCREEN_WIDTH;
    }

    BackGroundData listOfBackgrounds[6] {{FarBackground}, {FarLights}, {BackTreesBackground}, {Foreground}, {CloseLights}, {TopTrees}};

    while (!WindowShouldClose())
    {
        const float deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(GRAY);

        for (int i = 0; i < 6; i++)
        {
            Vector2 bgPosSpawnPosition = {listOfBackgrounds[i].background.width, 0.0};
            if (!isDead)
            {
                listOfBackgrounds[i] = UpdateBackground(listOfBackgrounds[i], deltaTime);
            }
            

            if (listOfBackgrounds[i].position.x <= -bgPosSpawnPosition.x)
            {
                listOfBackgrounds[i].position.x = 0.0;
            }
                
            DrawTextureEx(listOfBackgrounds[i].background, listOfBackgrounds[i].position, 0.0, 1.0, WHITE);
            DrawTextureEx(listOfBackgrounds[i].background, {bgPosSpawnPosition.x + listOfBackgrounds[i].position.x, listOfBackgrounds[i].position.y}, 0.0, 1.0, WHITE);
        }

        if (!isDead)
        {
            Ground.position.x -= Ground.speed * deltaTime;
        }
        

        if (Ground.position.x <= -Ground.texture.width && !isDead)
        {
            Ground.position.x = 0.0;
        }
            

        DrawTextureEx(Ground.texture, Ground.position, 0.0, 1.0, GRAY);
        DrawTextureEx(Ground.texture, {Ground.position.x + Ground.texture.width, Ground.position.y}, 0.0, 1.0, GRAY);

            if (count < enemySize)
            {
                spawnTime -= deltaTime;

                if (spawnTime <= 0.0)
                {
                    spawnTime = GetRandomValue(1,startSpawnTime);
                    count++;
                }
            }
            

            isGrounded = GroundCheck(player, Ground);
            velocity = isGrounded ? 0 : velocity + (gravity * deltaTime);

            if (health <= 0)
            {
                DrawText("Game Over. . .", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 40, RED);
                DrawCharacterTexture(playerDeath.texture, playerDeath.characterRect, player.position, scale, WHITE);
            }
            else
            {
                for(CharacterData enemy : enemies)
                {

                    Rectangle enemyRec{
                        enemy.position.x,
                        enemy.position.y,
                        enemy.characterRect.width,
                        enemy.characterRect.height
                    };

                    Rectangle playerRec
                    {
                        player.position.x,
                        player.position.y,
                        player.characterRect.width,
                        player.characterRect.height,
                    };

                    iFrames -= deltaTime;
                    if (CheckCollisionRecs(playerRec, enemyRec) && !isHit && iFrames <= 0.0)
                    {
                        isHit = true;
                    }
                }

                if (isHit)
                {
                    if (health <= 1)
                    {
                        DrawCharacterTexture(playerDeath.texture, playerDeath.characterRect, player.position, scale, WHITE);
                        playerDeath.runningTime += deltaTime;
                        isDead = true;
                        if (playerDeath.runningTime >= playerDeath.updateTime)
                        {
                            if (playerDeath.frame < 10)
                            {
                                playerDeath.runningTime = 0.0;
                                playerDeath.characterRect.x = playerDeath.frame * playerDeath.characterRect.width;
                                playerDeath.frame++;
                            }
                            else
                            {
                                health--;
                            }
                        }
                    }
                    else
                    {
                        DrawCharacterTexture(playerHit.texture, playerHit.characterRect, player.position, scale, WHITE);
                        playerHit.runningTime += deltaTime;

                        if (playerHit.runningTime >= playerHit.updateTime)
                        {   
                            if (playerHit.frame > 3)
                            {
                                health--;
                                playerHit.frame = 0;
                                isHit = false;
                                iFrames = startIFrames;
                            }
                            else
                            {
                                playerHit.runningTime = 0.0;
                                playerHit.characterRect.x = playerHit.frame * playerHit.characterRect.width;
                                playerHit.frame++;
                            }
                        }
                    }

                }
                else
                {
                    if (IsKeyPressed(KEY_SPACE) && isGrounded)
                    {
                        velocity += jumpForce;    
                    }
                    
                    if (isGrounded)
                    {
                        player = UpdateData(player, 7, deltaTime);
                        DrawCharacterTexture(player.texture, player.characterRect, player.position, scale, WHITE);
                    }
                    else
                    {
                        playerJump = UpdateData(playerJump, 2, deltaTime);
                        DrawCharacterTexture(playerJump.texture, playerJump.characterRect, player.position, scale, WHITE);
                    }
                    
                }
            }

            for (int i = 0; i < count; i++)
            {
                SlimeEnemy(enemies[i].texture, enemies[i].characterRect,enemies[i].position, enemies[i].frame, deltaTime);
                enemies[i].position.x += enemyVel * deltaTime;
                enemies[i] = UpdateData(enemies[i], 3, deltaTime);
            }
            
            player.position.y += (velocity * deltaTime);
            //End Game State

            if (enemies[enemySize - 1].position.x + enemies[enemySize - 1].texture.width <= 0.0)
            {
                DrawText("Winner!!!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 40, RED);
            }      
        EndDrawing();
    }

    UnloadTexture(player.texture); 

    for (int i = 0; i < enemySize; i++)
    {
        UnloadTexture(enemies[i].texture);
    }
    
    for (int i = 0; i < 4; i++)
    {
        UnloadTexture(listOfBackgrounds[i].background);
    }
    
    UnloadTexture(Ground.texture);

    CloseWindow();   
}

void DrawCharacterTexture(Texture2D texture, Rectangle source, Vector2 position, float textureScale, Color tint)
{
    Rectangle dest = { position.x, position.y, fabsf(source.width * textureScale), fabsf(source.height * textureScale) };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(texture, source, dest, origin, 0.0f, tint);
}

void SlimeEnemy(Texture2D texture, Rectangle rect, Vector2 position,int frame, float deltaTime)
{
    DrawCharacterTexture(texture, rect, position, scale, WHITE);
}

bool GroundCheck(CharacterData characterData, PlatformData platformData)
{    
    return characterData.position.y >= ((platformData.position.y + 7.5) - (characterData.characterRect.height * scale));
}

BackGroundData UpdateBackground(BackGroundData data, float deltaTime)
{
    data.position.x -= data.bgSpeed * deltaTime;    

    return data;
}



CharacterData UpdateData(CharacterData data,int maxFrame ,float deltaTime)
{
    data.runningTime += deltaTime;

    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        data.characterRect.x = data.frame * data.characterRect.width;
        data.frame++;
        
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }

    return data;
}