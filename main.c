#include <GPU.h>
#include <time.h>

#define TILE_SIZE 16
#define BLOCK_SIZE (TILE_SIZE*2)
#define COLUMNS 30
#define ROWS 30
#define VELOCITY 150

typedef struct _Piece{
    Vector2 direction;
    Vector2 current_position;
    Vector2 next_position;
    Vector2 previous_position;

    struct _Piece *next;
    struct _Piece *previous;
}Piece;

typedef struct _Snake{
    bool moving;
    Vector2 buffer_direction;
    Piece *body;
    Piece *head;
    Piece *tail;
}Snake;

void GetPointPosition(Vector2 *point){
    point->x = (rand() % COLUMNS) * TILE_SIZE;
    point->y = (rand() % ROWS) * TILE_SIZE;
}

Vector2 GetIsometricPosition(float x,float y){
    return (Vector2){x - y,(y + x) * 0.5f};
}

Piece* AddPiece(Piece **body,Piece source){
    Piece *piece = malloc(sizeof(Piece));
    *piece = source;

    if(*body == NULL){
        *body = piece;
        piece->next = NULL;
        piece->previous = NULL;
    }
    else{
        Piece *buffer = *body;
        while(buffer->next != NULL){buffer = buffer->next;}
        buffer->next = piece;
        piece->previous = buffer;
        piece->next = NULL;
    }

    return piece;
}

Snake* CreateSnake(){
    
    Snake *snake = malloc(sizeof(Snake));
    snake->moving = false;
    snake->buffer_direction = (Vector2){0.0f,0.0f};

    Piece piece1 = {{0.0f},{2.0f*TILE_SIZE,0.0f},{2.0f*TILE_SIZE,0.0f},{2.0f*TILE_SIZE,0.0f},NULL,NULL};
    Piece piece2 = {{0.0f},{2.0f*TILE_SIZE,0.0f},{2.0f*TILE_SIZE,0.0f},{2.0f*TILE_SIZE,0.0f},NULL,NULL};
    Piece piece3 = {{0.0f},{1.0f*TILE_SIZE,0.0f},{1.0f*TILE_SIZE,0.0f},{1.0f*TILE_SIZE,0.0f},NULL,NULL};;
    Piece piece4 = {{0.0f},{0.0f},{0.0f},{0.0f},NULL,NULL};;

    snake->body = NULL;

    snake->head = AddPiece(&snake->body,piece1);

    AddPiece(&snake->body,piece2);
    AddPiece(&snake->body,piece3);

    snake->tail = AddPiece(&snake->body,piece4);

    return snake;
}

void SnakeFree(Snake *snake){
    Piece *piece = snake->body;
    Piece *buffer;
    while(piece != NULL){
        buffer = piece->next;
        free(piece);
        piece = buffer;
    }
    free(snake);
}

void DrawBlock(Renderer *renderer,Vector2 position,Color border_color,Color fill_color){

    Vector2 top_face[4] = {
        {position.x,position.y},
        {position.x+TILE_SIZE,position.y-TILE_SIZE*0.5f},
        {position.x,position.y-TILE_SIZE},
        {position.x-TILE_SIZE,position.y-TILE_SIZE*0.5f}
    };

    Vector2 left_face[4] = {
        {position.x,position.y},
        {position.x,position.y+TILE_SIZE},
        {position.x-TILE_SIZE,position.y+TILE_SIZE*0.5f},
        {position.x-TILE_SIZE,position.y-TILE_SIZE*0.5f}
    };

    Vector2 right_face[4] = {
        {position.x,position.y},
        {position.x+TILE_SIZE,position.y-TILE_SIZE*0.5f},
        {position.x+TILE_SIZE,position.y+TILE_SIZE*0.5f},
        {position.x,position.y+TILE_SIZE}
    };

    DrawFilledPolygon(renderer,top_face,4,fill_color);
    DrawFilledPolygon(renderer,left_face,4,fill_color);
    DrawFilledPolygon(renderer,right_face,4,fill_color);

    DrawPolygon(renderer,top_face,4,border_color);
    DrawPolygon(renderer,left_face,4,border_color);
    DrawPolygon(renderer,right_face,4,border_color);
}

void DrawFloor(Renderer *renderer,Vector2 translate,Texture *floor_texture){
    Vector2 position;
    for(int y=0; y<ROWS; ++y){
        for(int x=0; x<COLUMNS; ++x){
            position = GetIsometricPosition(x * TILE_SIZE,y * TILE_SIZE);
            position.x += translate.x;
            position.y += translate.y + TILE_SIZE;
            Blit(renderer,floor_texture,NULL,&(Rect){position.x,position.y,BLOCK_SIZE,BLOCK_SIZE});
        }
    }
}

void DrawSnake(Renderer *renderer,Snake *snake,Vector2 translate,Texture *piece_texture){
    Vector2 position;
    Piece *buffer = snake->body;
    while(buffer != NULL){
        position = GetIsometricPosition(buffer->current_position.x,buffer->current_position.y);
        position.x += translate.x;
        position.y += translate.y;
        Blit(renderer,piece_texture,NULL,&(Rect){position.x,position.y,BLOCK_SIZE,BLOCK_SIZE});
        buffer = buffer->next;
    }
}

Vector2 GetTailDirection(Piece *tail){
    Vector2 direction = {0.0f};
    if(tail->previous != NULL){
        direction.x = (tail->previous->current_position.x - tail->current_position.x) / TILE_SIZE;
        direction.y = (tail->previous->current_position.y - tail->current_position.y) / TILE_SIZE;

        if(direction.x >= (COLUMNS-1)){
            direction.x = -1.0f;
        }
        else if(direction.x <= -(COLUMNS-1)){
            direction.x = 1.0f;
        }

        if(direction.y >= (ROWS-1)){
            direction.y = -1.0f;
        }
        else if(direction.y <= -(ROWS-1)){
            direction.y = 1.0f;
        }
    }
    return direction;
}

void SnakeNextMove(Snake *snake){

    if(!snake->buffer_direction.x && !snake->buffer_direction.y){
        snake->moving = false;
        return;
    }

    snake->head->direction = snake->buffer_direction;
    
    snake->head->previous_position = snake->head->current_position;
    
    snake->head->next_position = (Vector2){
        snake->head->current_position.x + snake->head->direction.x * TILE_SIZE,
        snake->head->current_position.y + snake->head->direction.y * TILE_SIZE
    };

    snake->tail->direction = GetTailDirection(snake->tail);

    snake->tail->previous_position = snake->tail->current_position;

    snake->tail->next_position = (Vector2){
        snake->tail->current_position.x + snake->tail->direction.x * TILE_SIZE,
        snake->tail->current_position.y + snake->tail->direction.y * TILE_SIZE
    };
}

void input(Snake *snake,SDL_Event event){
    Vector2 new_direction = {0.0f};

    if(event.key.keysym.scancode == SDL_SCANCODE_W)      new_direction.y = -1.0f;
    else if(event.key.keysym.scancode == SDL_SCANCODE_S) new_direction.y = 1.0f;
    else if(event.key.keysym.scancode == SDL_SCANCODE_A) new_direction.x = -1.0f;
    else if(event.key.keysym.scancode == SDL_SCANCODE_D) new_direction.x = 1.0f;

    if(!new_direction.x && !new_direction.y) return;

    snake->buffer_direction = new_direction;

    if(!snake->moving){
        snake->moving = true;
        SnakeNextMove(snake);
    }
}

void PutTailOnHead(Snake *snake){
    Piece *tail = snake->tail;

    snake->tail = tail->previous;
    snake->tail->next = NULL;

    snake->head->previous = tail;
    tail->next = snake->head;
    tail->previous = NULL;
    tail->current_position = snake->head->current_position;

    snake->head = tail;
    snake->body = snake->head;
}

void PieceMove(Piece *piece,float delta_time){
    float distanci_x = fabsf(piece->next_position.x - piece->current_position.x);
    float distanci_y = fabsf(piece->next_position.y - piece->current_position.y);

    if(distanci_x > VELOCITY * delta_time){
        piece->current_position.x += piece->direction.x * VELOCITY * delta_time;
    }
    else if(distanci_y > VELOCITY * delta_time){
        piece->current_position.y += piece->direction.y * VELOCITY * delta_time;
    }
    else{
        piece->current_position = piece->next_position;
        piece->direction = (Vector2){0.0f};
    }

    if(piece->current_position.x < 0.0f && piece->direction.x < 0.0f){
        piece->current_position.x += COLUMNS * TILE_SIZE;
        piece->next_position.x += COLUMNS * TILE_SIZE;
    }
    else if(piece->current_position.x+TILE_SIZE > COLUMNS * TILE_SIZE && piece->direction.x > 0.0f){
        piece->current_position.x -= COLUMNS * TILE_SIZE;
        piece->next_position.x -= COLUMNS * TILE_SIZE;
    }

    if(piece->current_position.y < 0.0f && piece->direction.y < 0.0f){
        piece->current_position.y += ROWS * TILE_SIZE;
        piece->next_position.y += ROWS * TILE_SIZE;
    }
    else if(piece->current_position.y+TILE_SIZE > ROWS * TILE_SIZE && piece->direction.y > 0.0f){
        piece->current_position.y -= ROWS * TILE_SIZE;
        piece->next_position.y -= ROWS * TILE_SIZE;
    }
}

void SnakeMove(Snake *snake,Vector2 *point,float delta_time){
    if(!snake->moving) return;

    PieceMove(snake->head,delta_time);
    PieceMove(snake->tail,delta_time);

    if((!snake->head->direction.x && !snake->head->direction.y) && (!snake->tail->direction.x && !snake->tail->direction.y)){
        
        PutTailOnHead(snake);

        if(snake->head->current_position.x == point->x && snake->head->current_position.y == point->y){
            GetPointPosition(point);

            Piece new_piece = {{0.0f},snake->head->previous_position,snake->head->previous_position,snake->head->previous_position,NULL,NULL};

            snake->tail = AddPiece(&snake->body,new_piece);
        }

        SnakeNextMove(snake);
    }
}

void DrawPoint(Renderer *renderer,Vector2 *point,Vector2 translate,Texture *point_texture){
    Vector2 position = GetIsometricPosition(point->x,point->y);
    position.x += translate.x;
    position.y += translate.y;
    Blit(renderer,point_texture,NULL,&(Rect){position.x,position.y,BLOCK_SIZE,BLOCK_SIZE});
}

int main(int n_args,char **args){
    SDL_Init(SDL_INIT_EVERYTHING);

    srand(time(0));

    int width = 1200;
    int height = 600;

    SDL_Window *window = SDL_CreateWindow("Snake",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    Renderer *renderer = CreateRenderer(window,false);

    Texture *floor_texture = CreateTexture(renderer,BLOCK_SIZE,BLOCK_SIZE,PIXEL_FORMAT_RGBA,false,true);
    RendererSetTarget(renderer,floor_texture);
    DrawBlock(renderer,(Vector2){BLOCK_SIZE*0.5f,BLOCK_SIZE*0.5f},(Color){0,128,50,255},(Color){0,153,53,255});
    
    Texture *piece_texture = CreateTexture(renderer,BLOCK_SIZE,BLOCK_SIZE,PIXEL_FORMAT_RGBA,false,true);
    RendererSetTarget(renderer,piece_texture);
    DrawBlock(renderer,(Vector2){BLOCK_SIZE*0.5f,BLOCK_SIZE*0.5f},(Color){200,0,0,255},(Color){200,0,0,255});

    Texture *point_texture = CreateTexture(renderer,BLOCK_SIZE,BLOCK_SIZE,PIXEL_FORMAT_RGBA,false,true);
    RendererSetTarget(renderer,point_texture);
    DrawBlock(renderer,(Vector2){BLOCK_SIZE*0.5f,BLOCK_SIZE*0.5f},(Color){200,150,0,255},(Color){200,150,0,255});

    RendererSetTarget(renderer,NULL);

    bool run = true;
    SDL_Event event;
    Vector2 translate = {width*0.5f - TILE_SIZE * 0.5f,height*0.5f - ROWS * TILE_SIZE * 0.5f};
    float current_time = SDL_GetTicks();
    float last_time = current_time;
    float delta_time = 0;

    Snake *snake = CreateSnake();

    Vector2 point;
    GetPointPosition(&point);

    while(run){
        
        current_time = SDL_GetTicks();
        delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        SDL_PollEvent(&event);

        if(event.type == SDL_QUIT){
            run = false;
        }
        else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED){
            SDL_GetWindowSize(window,&width,&height);
            translate = (Vector2){width*0.5f - TILE_SIZE * 0.5f,height*0.5f - ROWS * TILE_SIZE * 0.5f};
        }
        else if(event.type == SDL_KEYDOWN){
            input(snake,event);
        }

        ClearRGBA(renderer,0,0,0,255);
        SnakeMove(snake,&point,delta_time);
        DrawFloor(renderer,translate,floor_texture);
        DrawPoint(renderer,&point,translate,point_texture);
        DrawSnake(renderer,snake,translate,piece_texture);
        Flip(renderer);
    }

    SnakeFree(snake);

    TextureFree(floor_texture);
    TextureFree(piece_texture);
    TextureFree(point_texture);

    RendererFree(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}