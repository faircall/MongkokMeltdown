/*******************************************************************************************
*Press f6 to run
********************************************************************************************/
#include <math.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"


// TODO: think about how to do walk cycles for horizontal
// in an appropriate amount of frames lol

// TODO: a run animation cycle

// TODO: an enemy on screen

// TODO: Options- manual sprite ordering, OR draw stuff orthographically

// TODO: remake game in Unity and Godot for comparison

//Gonna want some data structure holding everything to draw, which in the manual case needs to be sorted
//and we'll also want some way of identifying which draw function to use

// Try both

//start manually

#define RAYMATH_IMPLEMENTATION

#define min(a,b) (a < b? a : b)

typedef enum {
    RUDYSTATIONARY,
    RUDYUP,
    RUDYDOWN,
    RUDYLEFT,
    RUDYRIGHT
} RudyDirection;

typedef enum {
    PLAYER,
    ENEMY
} EntityType;

typedef struct {
    Vector2 position;
    Vector2 destination;
    Vector2 velocity;
    Vector2 heading;
    EntityType type;
} Entity;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    float gameTimer = 0.0f;
    
    SetWindowState(FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Mongkok Meltdown");
    HideCursor();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    struct Vector2 newGameLoc = {.x = 300.0f, .y = 300.0f};
    struct Rectangle newGameRect = {.x = newGameLoc.x - 4, .y = newGameLoc.y - 2, .width = 102, .height = 24};
    
   
    
    int max_entities = 256;
    struct Entity *entities = malloc(max_entities * sizeof(Entity)); // flat array of enemies?
    
    
    Vector2 playerPosition = {.x = screenWidth/2.0f, .y = screenHeight/2.0f};
    Vector2 playerDestination = {.x = playerPosition.x, .y = playerPosition.y};
    Vector2 playerVelocity = {.x = 0.0f, .y = 0.0f};
    Vector2 playerHeading= {.x = 0.0f, .y = 0.0f};

    Vector2 enemyPosition = {.x = 400.0f, .y = 500.0f};
    Vector2 enemyDestination = {.x = 200.0f, .y = 100.0f};
    Vector2 enemyHeading;
    float enemyMoveTimer = 0.0f;
    float enemyMoveTimerMax = 2.0f;
    

    // graphics stuff
    Texture2D boloTexture = LoadTexture("art/bolo.png");
    Texture2D blankBGTexture = LoadTexture("art/blankbg.png");
    Texture2D rudyTexture = LoadTexture("art/rudy.png");
    Texture2D groundTexture = LoadTexture("art/ground.png");
    Texture2D pillarTexture = LoadTexture("art/pillars.png");
    int rudyTextureWidth = 64;
    int rudyTextureHeight = 64;
    // constants pulled from the spritesheet positions,
    // basically needs to be hardcoded
    int rudyStationaryStart = 0;
    int rudyDownStart = 2;
    int rudyUpStart = 8;
    int rudyLeftStart = 24;
    int rudyRightStart = 14;

    
    Shader shadowShader = LoadShader("base.vs", "shadow.fs");
    int shadowLoc = GetShaderLocation(shadowShader, "tex");
    int shadowTimeOfDayLoc = GetShaderLocation(shadowShader, "timeOfDay");
    int shadowTimeOfDayMaxLoc = GetShaderLocation(shadowShader, "timeOfDayMax");
    //SetShaderValueTexture(shadowShader, shadowLoc, rudyTexture);

    Shader lightingShader = LoadShader("base.vs", "lighting.fs");
    int lightingLoc = GetShaderLocation(lightingShader, "tex");
    int lightingTimeOfDayLoc = GetShaderLocation(lightingShader, "timeOfDay");
    int lightingTimeOfDayMaxLoc = GetShaderLocation(lightingShader, "timeOfDayMax");
    //SetShaderValueTexture(lightingShader, lightingLoc, pillarTexture);

    Shader horizonShader = LoadShader("base.vs", "horizon.fs");
    int horizonTimeOfDayLoc = GetShaderLocation(horizonShader, "timeOfDay");
    int horizonTimeOfDayMaxLoc = GetShaderLocation(horizonShader, "timeOfDayMax");
    int horizonSunPosLoc = GetShaderLocation(horizonShader, "sunPos");
    int horizonPosLoc = GetShaderLocation(horizonShader, "horizonPos");
    
    int horizonScreenWidthLoc = GetShaderLocation(horizonShader, "screenWidth");
    int horizonScreenHeightLoc = GetShaderLocation(horizonShader, "screenHeight");

    float timeOfDay = 0.0f;
    float timeOfDayMax = 100.0f;
    Vector2 sunPos = {.x = screenWidth/2.0f, .y = screenHeight/2.0f};
    Vector2 horizonPos = {.x = screenWidth/2.0f, .y = screenHeight/2.0f};

    float horizonScreenWidth = 1280.0f;
    float horizonScreenHeight = 720.0f;
    
    SetShaderValue(horizonShader, horizonTimeOfDayLoc, &timeOfDay, SHADER_UNIFORM_FLOAT);
    SetShaderValue(horizonShader, horizonTimeOfDayMaxLoc, &timeOfDayMax, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shadowShader, shadowTimeOfDayLoc, &timeOfDay, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shadowShader, shadowTimeOfDayMaxLoc, &timeOfDayMax, SHADER_UNIFORM_FLOAT);
    SetShaderValue(lightingShader, lightingTimeOfDayLoc, &timeOfDay, SHADER_UNIFORM_FLOAT);
    SetShaderValue(lightingShader, lightingTimeOfDayMaxLoc, &timeOfDayMax, SHADER_UNIFORM_FLOAT);
    SetShaderValue(horizonShader, horizonScreenWidthLoc, &horizonScreenWidth, SHADER_UNIFORM_FLOAT);
    SetShaderValue(horizonShader, horizonScreenHeightLoc, &horizonScreenHeight, SHADER_UNIFORM_FLOAT);
    SetShaderValueV(horizonShader, horizonSunPosLoc, &sunPos, SHADER_UNIFORM_VEC2, 1);
    SetShaderValueV(horizonShader, horizonPosLoc, &horizonPos, SHADER_UNIFORM_VEC2, 1);
    
    float targetMsPerFrame = 1.0f/60.0f;
    
    
    

    int rudyAnimationFrame = 0;
    int rudyCycleFrame = 0;
    int rudyCycleMax = 2;
    float rudyAnimationTimer = 0.0f;
    float rudyAnimationFrameTime = 25.0f;


    
    
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        float dt = GetFrameTime();
	

	timeOfDay += dt;
	SetShaderValue(horizonShader, horizonTimeOfDayLoc, &timeOfDay, SHADER_UNIFORM_FLOAT);
	SetShaderValue(shadowShader, shadowTimeOfDayLoc, &timeOfDay, SHADER_UNIFORM_FLOAT);
	SetShaderValue(lightingShader, lightingTimeOfDayLoc, &timeOfDay, SHADER_UNIFORM_FLOAT);
	if (timeOfDay > timeOfDayMax) {
	    timeOfDay = 0.0f;
	    SetShaderValue(horizonShader, horizonTimeOfDayLoc, &timeOfDay, SHADER_UNIFORM_FLOAT);
	    SetShaderValue(shadowShader, shadowTimeOfDayLoc, &timeOfDay, SHADER_UNIFORM_FLOAT);
	    SetShaderValue(lightingShader, lightingTimeOfDayLoc, &timeOfDay, SHADER_UNIFORM_FLOAT);
	}

	dt = min(dt, targetMsPerFrame);
        
        // handle keyboard input
        playerHeading= (Vector2){.x = 0.0f, .y = 0.0f};
        if (IsKeyDown(KEY_W)) {
	    playerHeading.y = -1.0f;
        }
        if (IsKeyDown(KEY_S)) {
	    playerHeading.y = 1.0f;
        }
        if (IsKeyDown(KEY_A)) {
	    playerHeading.x = -1.0f;
        }
        if (IsKeyDown(KEY_D)) {
	    playerHeading.x = 1.0f;
        }
	if (Vector2Length(playerHeading) > 0.0f) {
	    playerHeading = Vector2Normalize(playerHeading);
	}



	// some float constants for tuning
	float playerSpeed = 40.0f;
        float maxSpeed = 250.0f;

	float playerFrictionAmount = -0.1f;
	float velocityEpsilon = 20.0f;

	// basic physics integration
	playerVelocity = Vector2Add(playerVelocity, Vector2Scale(playerHeading, playerSpeed ));
	Vector2 playerFriction = Vector2Scale(playerVelocity, playerFrictionAmount);
	playerVelocity = Vector2Add(playerVelocity, playerFriction);
	if (Vector2Length(playerVelocity) > maxSpeed) {
	    playerVelocity = Vector2Normalize(playerVelocity);
	    playerVelocity = Vector2Scale(playerVelocity, maxSpeed);
	}
	if (Vector2Length(playerVelocity) < velocityEpsilon && Vector2Length(playerHeading) < 1.0f) {
	    playerVelocity = (Vector2){.x = 0.0f, .y = 0.0f};
	}
	
        playerPosition = Vector2Add(playerPosition, Vector2Scale(playerVelocity, dt));

	RudyDirection rudyDirection = RUDYSTATIONARY;


	
	// actually do abs val tests here
	if (playerVelocity.x * playerVelocity.x >= playerVelocity.y * playerVelocity.y) {
	    if (playerVelocity.x < 0.0f) {
		rudyDirection = RUDYLEFT;
	    }
	    if (playerVelocity.x > 0.0f) {
		rudyDirection = RUDYRIGHT;
	    }
	}
	if (playerVelocity.y * playerVelocity.y >= playerVelocity.x * playerVelocity.x) {
	    if (playerVelocity.y < 0.0f) {
		rudyDirection = RUDYUP;
	    }
	    if (playerVelocity.y > 0.0f) {
		rudyDirection = RUDYDOWN;
	    }
	}

	float rudyAnimationFrameTimeMultiplier = 0.5f;
	if (rudyDirection == RUDYSTATIONARY) {
	    rudyAnimationFrame = rudyStationaryStart;
	    rudyCycleMax = 2;
	}

	if (rudyDirection == RUDYLEFT) {
	    rudyAnimationFrame = rudyLeftStart;
	    rudyCycleMax = 8;
	    rudyAnimationFrameTimeMultiplier = 1.0f;
	}
	if (rudyDirection == RUDYRIGHT) {
	    rudyAnimationFrame = rudyRightStart;
	    rudyCycleMax = 8;
	    rudyAnimationFrameTimeMultiplier = 1.0f;
	}
	
	if (rudyDirection == RUDYUP) {
	    rudyAnimationFrame = rudyUpStart;
	    rudyCycleMax = 4;
	}
	if (rudyDirection == RUDYDOWN) {
	    rudyAnimationFrame = rudyDownStart;
	    rudyCycleMax = 4;
	}

	if (Vector2Length(playerHeading) != 0.0f) {
	    rudyAnimationTimer += (dt * Vector2Length(playerVelocity)*1.15*rudyAnimationFrameTimeMultiplier);
	} else if (rudyDirection == RUDYSTATIONARY) {
	    rudyAnimationTimer += (dt * 100.0f);
	} else {
	    rudyAnimationTimer += (dt * maxSpeed*1.5);
	}
	if (rudyAnimationTimer >= rudyAnimationFrameTime) {
	    rudyCycleFrame++;
	    rudyAnimationTimer = 0.0f;
	}
	if (rudyCycleFrame >= rudyCycleMax) {
	    rudyCycleFrame = 0;
	}

	// ENEMY MOVEMENT STUFF (to be wrapped later)
	if (Vector2Distance(enemyDestination, enemyPosition) > 5.0f) {
	    //get heading
	    enemyHeading = Vector2Subtract(enemyDestination, enemyPosition);
	    enemyHeading = Vector2Normalize(enemyHeading);
	    //then move

	    //temp basic movement
	    enemyPosition = Vector2Add(enemyPosition, Vector2Scale(enemyHeading, dt * 100.0f));
	    
	} else {
	    //wait
	    enemyMoveTimer += dt;
	    //find a new destination
	    if (enemyMoveTimer >= enemyMoveTimerMax) {
		enemyMoveTimer = 0.0f;
		enemyDestination.x = 5 + rand() % (screenWidth - 30);
		enemyDestination.y = 5 + rand() % (screenHeight - 30);
	    }
	}
    
        
        
       
        struct Vector2 mousePos = {.x = GetMouseX(), .y = GetMouseY()};
       
    
       
        
        float dtToUse = min(dt, targetMsPerFrame);
    

	
            
        //dt -= dtToUse;
        //gameTimer += dtToUse;
            
        
        
        
        
       

        // Draw
        //----------------------------------------------------------------------------------
     
        BeginDrawing();
        
        
     
        ClearBackground(RED);
        
        
        
        // Rectangle playerToDraw = {.x = 0.0f, .y = 0.0f, .width = playerTexture.width, .height = playerTexture.height};
        
       
        

        








	//think about draw order here
	//apply a sort

        
     
        
        DrawFPS(30, 30);
        
        
        //DrawCircleLines(playerPosition.x, playerPosition.y, 20, RAYWHITE);
	Rectangle rudyFrameRec = {.x = (rudyAnimationFrame + rudyCycleFrame) * rudyTextureWidth, .y = 0, .width = rudyTextureWidth, .height = rudyTextureHeight};
	Rectangle rudyFrameShadowRec = {.x = (rudyAnimationFrame + rudyCycleFrame) * rudyTextureWidth, .y = 0, .width = rudyTextureWidth, .height = rudyTextureHeight};
	rudyFrameShadowRec.width *= -1.0f;


	BeginShaderMode(horizonShader);
	//DrawRectangle(0, 0, screenWidth, screenHeight, RAYWHITE);
	DrawTexture(blankBGTexture, 0, 0, RAYWHITE);
	EndShaderMode();

	
	BeginShaderMode(lightingShader);
	SetShaderValueTexture(lightingShader, lightingLoc, pillarTexture); //bugs out later shader code if we do this
	//maybe its cache doesn't clear out fast enough or something...?

	//I think we should use a different draw texture call
	DrawTextureEx(pillarTexture, (Vector2){.x = 0.0f, .y = 0.0f}, 0.0f, 10.0f, RAYWHITE);
	EndShaderMode();
	//EndShaderMode();
	//BeginShaderMode(lightingShader);
	//SetShaderValueTexture(lightingShader, lightingLoc, groundTexture);
	//DrawTextureEx(groundTexture, (Vector2){.x = 0.0f, .y = 0.0f}, 0.0f, 10.0f, RAYWHITE);
	//EndShaderMode();

	BeginShaderMode(shadowShader);
	SetShaderValueTexture(shadowShader, shadowLoc, rudyTexture);
	Vector2 playerShadowPosition = {.x = playerPosition.x, .y = playerPosition.y - 50.0f};
	Rectangle playerShadowRect = {.x = playerPosition.x, .y = playerPosition.y + 60.0f, .width = rudyTextureWidth, .height = rudyTextureHeight};
	//DrawTextureRec(rudyTexture, rudyFrameRec, playerShadowPosition, RAYWHITE);
	//DrawTextureEx(rudyTexture, rudyFrameRec, playerShadowPosition, RAYWHITE);
	//DrawTextureTiled(rudyTexture, rudyFrameRec, playerShadowRect, (Vector2){.x = rudyTextureWidth, .y = rudyTextureHeight}, 2.0f, 1.0f, RAYWHITE);
	DrawTexturePro(rudyTexture, rudyFrameShadowRec, playerShadowRect, (Vector2){.x = rudyTextureWidth, .y = rudyTextureHeight}, 180.0f, RAYWHITE);
	EndShaderMode();
	BeginShaderMode(lightingShader);
	SetShaderValueTexture(lightingShader, lightingLoc, rudyTexture); //bugs out later shader code if we do this
	DrawTextureRec(rudyTexture, rudyFrameRec, playerPosition, RAYWHITE);

	//DrawTextureV(boloTexture, enemyPosition, RAYWHITE);
        DrawCircleLines(mousePos.x, mousePos.y, 5, LIGHTGRAY);
     
            
	
            
	
                
               
	
      

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    //free everything
    free(entities);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
