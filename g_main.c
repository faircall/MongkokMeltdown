/*******************************************************************************************
*Press f6 to run
********************************************************************************************/
#include <math.h>
#include "raylib.h"
#include "raymath.h"


// TODO: think about how to do walk cycles for horizontal
// in an appropriate amount of frames lol

#define RAYMATH_IMPLEMENTATION

#define min(a,b) (a < b? a : b)

typedef enum {
    RUDYSTATIONARY,
    RUDYUP,
    RUDYDOWN,
    RUDYLEFT,
    RUDYRIGHT
} RudyDirection;

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
    
   
    
   
    
    
    Vector2 playerPosition = {.x = screenWidth/2.0f, .y = screenHeight/2.0f};
    Vector2 playerDestination = {.x = playerPosition.x, .y = playerPosition.y};
    Vector2 playerVelocity = {.x = 0.0f, .y = 0.0f};
    Vector2 playerHeading= {.x = 0.0f, .y = 0.0f};
    

    // graphics stuff
    Texture2D rudyTexture = LoadTexture("art/rudy.png");
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
	DrawTextureRec(rudyTexture, rudyFrameRec, playerPosition, RAYWHITE);
        DrawCircleLines(mousePos.x, mousePos.y, 5, LIGHTGRAY);
     
            
            
            
           
                
               
             
      

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
